#include <cuda.h>
#include <cuda_runtime.h>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/ptr_container/ptr_vector.hpp> 
#include <boost/foreach.hpp>
#include "Logging.hpp"
#include "CircuitPartitioner.hpp"
#include "timing.hpp"
#include "hpwl.hpp"
#include "shared.hpp"


extern Logger logger;

#define slice(iter, start, end)      boost::make_iterator_range(iter, start, end - iter.size())

extern "C"
//! Simple utility function to check for CUDA runtime errors
void checkCUDAError(const char* msg);

extern "C"
// Wrapper for the __global__ call that sets up the kernel call
kernel_durations_t launch_shared_kernel(shared_kernel_ptr kernel,
                        int threads, int blocks,
                        int shared_mem_size,
                        int min_kl,
                        int max_kl,
                        cuda_data_addrs &d_data,
                        std::vector<float> &wire_lengths);


extern "C"
// Wrapper for the __global__ call that sets up the kernel call
kernel_durations_t launch_constant_kl_kernel(constant_kl_kernel_ptr kernel,
                        int threads, int blocks,
                        int shared_mem_size,
                        int min_kl,
                        int max_kl,
                        cuda_data_addrs &d_data,
                        std::vector<float> &wire_lengths);


void CircuitPartitioner::partition_nets() {
    int net_count;
    logger.debug(_("  Using max_nets_per_block: %d") 
            % max_nets_per_block);

    BOOST_FOREACH(int kl, xrange(min_kl, max_kl + 1)) {
        int samples_count;
        int sample_size;
        net_count = c.kl_nets_count(kl);
        if(net_count == 0) continue;

        net_list_ptr nl_ptr;
        c.get_nets_by_kl(kl, nl_ptr);
        // Convert pointer to a reference for convenience;
        net_list &nets = *nl_ptr;

        kl_properties.push_back(kl_properties_t(kl, net_count));
        kl_properties_t &kl_prop = kl_properties[kl_properties.size() - 1];

        vector<net_range> &cuda_nets = kl_prop.cuda_net_samples;
        vector<net_range> &cpu_nets = kl_prop.cpu_net_samples;

        kl_prop.net_count = net_count;
        int net_idx = 0;
        while(net_count > 0) {
            sample_size = calc_netlist_sample_size(net_count, kl);
            if(sample_size < min_sample_size) {
                min_sample_size = sample_size;
            }
            if(sample_size > max_sample_size) {
                max_sample_size = sample_size;
            }
            samples_count = net_count / sample_size;
            BOOST_FOREACH(int k, xrange(samples_count)) {
                net_range nets_sample = slice(nets, net_idx, net_idx + sample_size);

                if(sample_size < net_count_threshold) {
                    cpu_nets.push_back(nets_sample);
                } else {
                    cuda_nets.push_back(nets_sample);
                }
                net_count -= sample_size;
                net_idx += sample_size;
            }
        }
    }
}


void CircuitPartitioner::load_static_cuda_data() {
    total_block_count = 0;

    host_data.kl_mapping.resize(max_kl - min_kl + 1);
    kl_mapping_t &kl_mapping = host_data.kl_mapping;

    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        vector<net_range> &cuda_nets = kl_prop.cuda_net_samples;
        int block_count = cuda_nets.size();
        kl_mapping[kl_prop.kl - min_kl].x = block_count;
        if(block_count > 0) {
            // Store the maximum net sample size for the current kl
            kl_mapping[kl_prop.kl - min_kl].y = cuda_nets[0].size();
            logger.info(_("kl: %d -> max sample size: %d")
                    % kl_prop.kl % kl_mapping[kl_prop.kl - min_kl].y);
        }
        total_block_count += block_count;
    }
    if(total_block_count == 0) return;

    threads = min((1 << (int)ceil(log(kl_properties[0].cuda_net_samples[0].size()) / log(2))),
            cuda_kernel.get_kernel_max_threads_per_block());

    vector<string> msg;
    int kl = min_kl;
    msg.push_back("kl_mapping:");
    BlockMem total_mem_reqd(0);
    BOOST_FOREACH(ushort2 value, kl_mapping) {
        if(value.x == 0) {
            kl++;
            continue;
        }
        BlockMem reqd_mem = calc_reqd_mem(value.y, kl, value.x);
        total_mem_reqd += reqd_mem;
        msg.push_back((_("  kl[%d]: %d CUDA blocks "
                        "(sizes: %d, block_ids: %d, "
                        "block_coords: %d, wire_length: %d)")
                        % kl % value.x
                        % reqd_mem.sizes
                        % reqd_mem.block_ids
                        % reqd_mem.block_coords
                        % reqd_mem.wire_length
                        ).str());
        kl++;
    }
    logger.info(boost::algorithm::join(msg, "\n"));

    logger.debug(_("Total instance counts required: "
                    "(sizes: %d, block_ids: %d, "
                    "block_coords: %d, wire_length: %d)")
                    % total_mem_reqd.sizes
                    % total_mem_reqd.block_ids
                    % total_mem_reqd.block_coords
                    % total_mem_reqd.wire_length);

    block_offsets.resize(total_block_count);

    // Simulate address offset calculation to be performed by a thread in each
    // block.
    BOOST_FOREACH(int blockIdx, xrange(total_block_count)) {
        int kl_base_blockIdx;
        int kl = calc_kl(kl_mapping, min_kl, blockIdx, kl_base_blockIdx);
        BlockMem base_offsets = calc_mem_offsets(kl_mapping, kl, min_kl);
        BlockMem offsets = calc_reqd_mem(kl_mapping[kl - min_kl].y, kl, 
                                        blockIdx - kl_base_blockIdx);
        block_offsets[blockIdx] = offsets + base_offsets;
    }

    // Gather data:
    // sizes
    host_data.sizes.resize(total_mem_reqd.sizes);
    // block_ids
    host_data.block_ids.resize(total_mem_reqd.block_ids);

    // wire_length
    //host_data.wire_length.resize(total_mem_reqd.wire_length);
    host_data.wire_length_size = total_mem_reqd.wire_length;
    wire_lengths.resize(host_data.wire_length_size);

    fill_host_data();

    block_coords.resize(total_mem_reqd.block_coords);
}


/*!  Select a sample of nets from the circuit and generate arrays
containing information about the positions of blocks connected
to and the connectivity information about the selected nets.
Finally, group these arrays together into a CudaData object.  */
void CircuitPartitioner::fill_host_data() {
    block_mapping_list.resize(block_offsets.size());

    int blkIdx = 0;
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        vector<net_range> &cuda_nets = kl_prop.cuda_net_samples;

        BOOST_FOREACH(net_range &nets_sample, cuda_nets) {
            logger.debug(_("processing block: %d/%d") % blkIdx % total_block_count);

            BlockMem &mem_offsets = block_offsets[blkIdx];

            int num_nets = nets_sample.size();
            set<Block*> unique_blocks;
            /* Build up unique set of block IDs connected to any of the nets
            * in the current sample.  Also, form a list of the number of
            * blocks connected to each net. */
            int i = 0;
            BOOST_FOREACH(net_ptr const &net, nets_sample) {
                block_list const &blocks = net->get_blocks();
                BOOST_FOREACH(block_ptr const &block, blocks) {
                    unique_blocks.insert(block.get());
                }
            }

            /* collect all data in the form:
            *   [block ids (net 0)]...[block ids (net n)][block coords] */

            irange sizes = boost::make_iterator_range(host_data.sizes, 
                        mem_offsets.sizes, 
                        mem_offsets.sizes + 2 - host_data.sizes.size());
            sizes[0] = num_nets * kl_prop.kl;
            sizes[1] = num_nets * kl_prop.kl; 
            /* This maps a list of block IDs to the corresponding block
            * objects for fast lookup. */
            block_id_map &block_mapping = block_mapping_list[blkIdx];
            i = 0;

            BOOST_FOREACH(Block* p_block, unique_blocks) {
                block_mapping[p_block] = i++;
            }

            irange block_ids = boost::make_iterator_range(
                                host_data.block_ids,
                                mem_offsets.block_ids, 
                                mem_offsets.block_ids
                                    + (num_nets * kl_prop.kl)
                                    - host_data.block_ids.size());

            // Write the block IDs connected to each net into contiguous memory.
            i = 0;
            BOOST_FOREACH(net_ptr const &net, nets_sample) {
                int start = i * kl_prop.kl;
                int end = start + kl_prop.kl;
                int j = 0;

                block_list const &blocks = net->get_blocks();
                irange ids = boost::make_iterator_range(block_ids, 
                                start, end - block_ids.size());
                BOOST_FOREACH(block_ptr const &block, blocks) {
                    ids[j] = block_mapping[block.get()];
                    j++;
                }
                i++;
            }
            blkIdx++;
        }
    }
    copy_static_cuda_data();
    //verify_cuda_data();
}


void CircuitPartitioner::update_cuda_block_coords() {
    if(total_block_count == 0) return;
    int blkIdx = 0;
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        vector<net_range> &cuda_nets = kl_prop.cuda_net_samples;

        BOOST_FOREACH(net_range &nets_sample, cuda_nets) {
            BlockMem &mem_offsets = block_offsets[blkIdx];

            int num_nets = nets_sample.size();

            /* Store the x and y coordinates in contiguous memory in the
            * following form:
            *        [x,y coord of all blocks] */
            int coords_size = num_nets * kl_prop.kl;

            coord_range coords = slice(block_coords, mem_offsets.block_coords,
                                    mem_offsets.block_coords + coords_size);
            logger.debug(_("blkIdx[%d] block_coords offset = %d, "
                            "coords_size = %d") 
                    % blkIdx
                    % mem_offsets.block_coords
                    % coords_size);

            BOOST_FOREACH(block_id_map::value_type item, 
                            block_mapping_list[blkIdx]) {
                Block* const &p_block = item.first;
                coords[item.second].x = p_block->x;
                coords[item.second].y = p_block->y;
            }
            blkIdx++;
        }
    }

    copy_block_coords_cuda_data();
}


void CircuitPartitioner::copy_block_coords_cuda_data() {
    int block_coords_mem = block_coords.size() * sizeof(ushort2);
    if(d_data.block_coords == 0) {
        cudaMalloc((void**)&d_data.block_coords, block_coords_mem);
        checkCUDAError("cudaMalloc: block_coords");
    }

    // Copy data to device:
    timespec start, end, delta;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    cudaMemcpy(d_data.block_coords,
                &block_coords[0],
                block_coords_mem,
                cudaMemcpyHostToDevice);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    delta = time_diff(start, end);
    coords_copy_duration = delta.tv_sec + delta.tv_nsec * 1e-9;
    checkCUDAError("cudaMemcpy: block_coords");
}

void CircuitPartitioner::cuda_alloc(void** dst, int size, string label) {
    cudaMalloc(dst, size);
    checkCUDAError(("cudaMalloc: " + label).c_str());
}


void CircuitPartitioner::cuda_alloc_and_copy(void** dst, void* src, int size, 
                                                string label) {
    cuda_alloc(dst, size, label);

    // Copy data to device:
    cudaMemcpy(*dst, src, size, cudaMemcpyHostToDevice);
    checkCUDAError(("cudaMemcpy: " + label).c_str());
}


void CircuitPartitioner::copy_static_cuda_data() {
    copy_kl_mapping();
    copy_sizes();
    copy_block_ids();

    // Output arrays:
    // Allocate memory required for wire_length (one item per CUDA block)
    cuda_alloc((void**)&d_data.wire_length, mem_size_wire_length(), "wire_length");
}


int CircuitPartitioner::_calc_sample_shared_mem(int sample_size, int mem_per_net, int overhead) {
    int two_power_sample_size = (1 << (int)ceil(log(sample_size) / log(2)));
    int result = sample_size * mem_per_net 
            + min(two_power_sample_size, cuda_kernel.get_kernel_max_threads_per_block()) * sizeof(float)
            + overhead;
    /*
    logger.notice(_("foo(%d, %d, %d) = %d") 
            % sample_size
            % mem_per_net
            % overhead
            % result);
            */
    return result;
}


int CircuitPartitioner::calc_netlist_sample_size(int num_nets, int kl) {
    int cuda_reg_count = cuda_kernel.get_kernel_reg_count();
    int cuda_shared_mem = cuda_kernel.get_kernel_shared_mem();
    int MAX_REGS_PER_BLOCK = cuda_device.get_max_regs_per_block();
    int min_kl = c.get_min_kl();
    int max_kl = c.get_max_kl();
    int kl_mapping_size = max_kl - min_kl + 1;

    logger.debug(_("cuda_reg_count:  %s\n cuda_shared_mem: %s") 
                % cuda_reg_count % cuda_shared_mem);
    
    /* Calculate the maximum amount shared memory available for
    * dynamic allocation (this is needed for allocating shared
    * memory arrays on CUDA device). */
    int max_memory = max_shared_mem_available - cuda_shared_mem;
    int overhead = shared_mem_overhead(kl_mapping_size);
    max_memory -= overhead;
    int mem_per_net = shared_mem_per_net(kl);

    /* Based on the amount of shared mem available, calculate the
    * maximum number of nets we can calculate the wirelength for
    * in a single pass (i.e., the max net sample size). */
    logger.debug(_("calc sample power: kl[%d] log(%s / %s, 2)")
            % kl % max_memory % mem_per_net);

    if(mem_per_net > max_memory) {
        logger.error(_("Cannot fit calculation for cardinality %d onto GPU") % kl);
        return -1;
    }

    const int MAX_THREADS_PER_BLOCK = cuda_kernel.get_kernel_max_threads_per_block();
    int sample_size;
    if(mem_per_net == 0) {
        sample_size = num_nets;
    } else {
        sample_size = max_memory / mem_per_net;
    }

    if(sample_size > num_nets) {
        /* We have fewer nets to calculate for than the max sample
        * size.  Thus, we can set the sample size to the smallest
        * power of two larger than our # of nets. */
        logger.debug(_("sample_size[%d] > num_nets[%d]") % sample_size % num_nets);
        sample_size = num_nets;
    }
    uint32_t max_id_value = std::numeric_limits<unsigned short>::max();

    if(sample_size * kl > max_id_value) {
        /* We have fewer nets to calculate for than the max sample
        * size.  Thus, we can set the sample size to the smallest
        * power of two larger than our # of nets. */
        logger.debug(_("sample_size * kl: %d > max_id_value: %d") 
                % (sample_size * kl) % max_id_value);
        sample_size = max_id_value / kl;
    }

    if(max_nets_per_block > 0 && sample_size > max_nets_per_block) {
        /* We have fewer nets to calculate for than the max sample
        * size.  Thus, we can set the sample size to the smallest
        * power of two larger than our # of nets. */
        logger.debug(_("sample_size: %d > max_nets_per_block: %d") 
                % sample_size % max_nets_per_block);
        sample_size = max_nets_per_block;
    }

    int required_mem = _calc_sample_shared_mem(sample_size, mem_per_net, overhead);
    while(required_mem > max_memory) {
        sample_size--;
        required_mem = _calc_sample_shared_mem(sample_size, mem_per_net, overhead);
    }
    logger.debug(_("using a sample size of: %d (%d)") 
                    % sample_size % required_mem);

    if(required_mem > max_shared_mem) {
        logger.debug(_("increasing max_shared_mem: %d -> %d")
                    % max_shared_mem % required_mem);
        max_shared_mem = required_mem;
    }

    return sample_size;
}


void cuda_data_addrs::free() {
    cudaFree(kl_mapping);
    cudaFree(sizes);
    cudaFree(block_ids);
    cudaFree(wire_length);
    cudaFree(block_coords);
    kl_mapping = 0;
    sizes = 0;
    block_ids = 0;
    wire_length = 0;
    block_coords = 0;
}


/*<<<<<<<< SharedPartitioner -------------------------------------------*/

int SharedPartitioner::shared_mem_per_net(int kl) {
    return  // For block ids.
            kl * sizeof(uint16_t)
            // For block coordinates.
            + kl * sizeof(ushort2)
            ;
}

int SharedPartitioner::shared_mem_overhead(int kl_mapping_size) {
    return  // For kl mapping
            kl_mapping_size * sizeof(kl_mapping_t::value_type) +
            // For sizes
            2 * sizeof(uint16_t)
            // For max extra mem required to align
            // shared memory arrays.
            + 4 * 4
            + 64
            ;
}

/*>>>>>>>> SharedPartitioner -------------------------------------------*/


/*<<<<<<<< SharedCoordsPartitioner -------------------------------------------*/

int SharedCoordsPartitioner::shared_mem_per_net(int kl) {
    return  // For block coordinates.
            kl * sizeof(ushort2);
}

int SharedCoordsPartitioner::shared_mem_overhead(int kl_mapping_size) {
    return  // For max extra mem required to align
            // shared memory arrays.
            2 * 4
            + 64
            ;
}

/*>>>>>>>> SharedCoordsPartitioner -------------------------------------------*/


/*<<<<<<<< ConstantKlPartitioner ---------------------------------------------*/

extern const char* ex_d_kl_mapping;

void ConstantKlPartitioner::copy_kl_mapping() {
    cudaMemcpyToSymbol(ex_d_kl_mapping, &host_data.kl_mapping[0],
                            mem_size_kl_mapping(), 0, cudaMemcpyHostToDevice);
    checkCUDAError("cudaMemcpyHostToDevice: kl_mapping");
}

/*>>>>>>>> ConstantKlPartitioner ---------------------------------------------*/


/*<<<<<<<< SharedCoordsConstantKlPartitioner ------------------------------*/

extern const char* ex_d_shared_coords_kl_mapping;

void SharedCoordsConstantKlPartitioner::copy_kl_mapping() {
    cudaMemcpyToSymbol(ex_d_shared_coords_kl_mapping, &host_data.kl_mapping[0],
                            mem_size_kl_mapping(), 0, cudaMemcpyHostToDevice);
    checkCUDAError("cudaMemcpyHostToDevice: kl_mapping");
}

/*>>>>>>>> SharedCoordsConstantKlPartitioner ------------------------------*/


/*<<<<<<<< ConstantCrossPartitioner ------------------------------------------*/

extern const char* ex_d_cross_count;

void ConstantCrossPartitioner::copy_static_cuda_data() {
    cudaMemcpyToSymbol(ex_d_cross_count, cross_count, 
                        sizeof(cross_count), 0, cudaMemcpyHostToDevice);
    checkCUDAError("cudaMemcpyHostToDevice: cross_count");

    CircuitPartitioner::copy_static_cuda_data();
}

/*>>>>>>>> ConstantCrossPartitioner ------------------------------------------*/


/*<<<<<<<< SharedConstantCrossPartitioner ------------------------------------------*/

int SharedConstantCrossPartitioner::shared_mem_per_net(int kl) {
    return  // For block ids.
            kl * sizeof(uint16_t)
            // For block coordinates.
            + kl * sizeof(ushort2)
            ;
}

int SharedConstantCrossPartitioner::shared_mem_overhead(int kl_mapping_size) {
    return  // For kl mapping
            kl_mapping_size * sizeof(kl_mapping_t::value_type) +
            // For sizes
            2 * sizeof(uint16_t)
            // For max extra mem required to align
            // shared memory arrays.
            + 4 * 4
            + 64
            ;
}

extern const char* ex_d_shared_cross_count;

void SharedConstantCrossPartitioner::copy_static_cuda_data() {
    cudaMemcpyToSymbol(ex_d_shared_cross_count, cross_count, 
                        sizeof(cross_count), 0, cudaMemcpyHostToDevice);
    checkCUDAError("cudaMemcpyHostToDevice: cross_count");

    CircuitPartitioner::copy_static_cuda_data();
}

/*>>>>>>>> SharedConstantCrossPartitioner ------------------------------------------*/


/*<<<<<<<< SharedCoordsConstantCrossPartitioner ------------------------------------*/

int SharedCoordsConstantCrossPartitioner::shared_mem_per_net(int kl) {
    return  // For block coordinates.
            kl * sizeof(ushort2)
            ;
}

int SharedCoordsConstantCrossPartitioner::shared_mem_overhead(int kl_mapping_size) {
    return  // For max extra mem required to align
            // shared memory arrays.
            2 * 4
            + 64
            ;
}

extern const char* ex_d_shared_coords_cross_count;

void SharedCoordsConstantCrossPartitioner::copy_static_cuda_data() {
    cudaMemcpyToSymbol(ex_d_shared_coords_cross_count, cross_count, 
                        sizeof(cross_count), 0, cudaMemcpyHostToDevice);
    checkCUDAError("cudaMemcpyHostToDevice: cross_count");

    CircuitPartitioner::copy_static_cuda_data();
}

/*>>>>>>>> SharedCoordsConstantCrossPartitioner ------------------------------------*/


kernel_durations_t CudaCalculator::_launch_kernel() {
    CircuitPartitioner &p = *part;
    return launch_shared_kernel((shared_kernel_ptr)p.kernel, 
                    p.threads, 
                    p.total_block_count,
                    p.max_shared_mem,
                    p.min_kl, p.max_kl, 
                    p.d_data,
                    p.wire_lengths);
}


double StarPlusCalculator::wirelength_cpu(double &duration) {
    CircuitPartitioner &p = *part;

    if(p.total_block_count == 0) return 0;
    int blkIdx = 0;
    double starplus = 0;
    timespec start, end, delta;

    vector<kl_properties_t> &kl_properties = p.kl_properties;
    vector<ushort2> &block_coords = p.block_coords;
    vector<BlockMem> &block_offsets = p.block_offsets;
    static_host_data &host_data = p.host_data;

    vector<string> msg;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        vector<net_range> &cuda_nets = kl_prop.cuda_net_samples;
        int kl = kl_prop.kl;

        BOOST_FOREACH(net_range &nets_sample, cuda_nets) {
            double blk_starplus = 0;
            BlockMem &mem_offsets = block_offsets[blkIdx];

            irange sizes = slice(host_data.sizes, 
                        mem_offsets.sizes, 
                        mem_offsets.sizes + 2);
            irange block_ids_all = slice(host_data.block_ids, 
                        mem_offsets.block_ids, 
                        mem_offsets.block_ids + sizes[0]);
            int num_nets = nets_sample.size();

            int coords_size = sizes[1];
            coord_range r_block_coords = slice(block_coords, 
                        mem_offsets.block_coords, 
                        mem_offsets.block_coords + coords_size);

            BOOST_FOREACH(int i, xrange(num_nets)) {
                irange block_ids = slice(block_ids_all, 
                            i * kl, 
                            (i + 1) * kl);
                uint64_t x = 0;
                uint64_t x_sqr = 0;
                uint64_t y = 0;
                uint64_t y_sqr = 0;

                BOOST_FOREACH(uint16_t id, block_ids) {
                    ushort2 coord = r_block_coords[id];
                    uint64_t val_temp = coord.x;
                    x += val_temp;
                    x_sqr += val_temp * val_temp;

                    val_temp = coord.y;
                    y += val_temp;
                    y_sqr += val_temp * val_temp;
                }
                double wl = sqrt(x_sqr - (float)(x * x) / kl + 1.59) +
                                        sqrt(y_sqr - (float)(y * y) / kl + 1.59);
                blk_starplus += wl;
            }
            starplus += blk_starplus;
            //msg.push_back((_("%.1f") % blk_starplus).str());
            blkIdx++;
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    /*
    logger.debug(_("  CPU wirelength:  %.1f = %s")
                % starplus
                % boost::algorithm::join(msg, "+")
            );
    */

    delta = time_diff(start, end);
    duration = delta.tv_sec + delta.tv_nsec * 1e-9;

    double sub_duration;
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        BOOST_FOREACH(net_range &nets_sample, kl_prop.cpu_net_samples) {
            starplus += nets_sample_wl(nets_sample, sub_duration);
            duration += sub_duration;
        }
    }
    return starplus;
}


double StarPlusCalculator::wirelength(kernel_durations_t &duration) {
    double total_wirelength = 0;
    timespec start, end, delta;
    CircuitPartitioner &p = *part;
    vector<kl_properties_t> &kl_properties = p.kl_properties;

    duration.calc = 0;
    duration.wirelength_copy = 0;
    if(p.total_block_count > 0) {
        kernel_durations_t durations = _launch_kernel();
        duration.calc += durations.calc;
        duration.wirelength_copy += durations.wirelength_copy;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        BOOST_FOREACH(float wl, p.wire_lengths) {
            total_wirelength += wl;
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        delta = time_diff(start, end);
        duration.calc += delta.tv_sec + delta.tv_nsec * 1e-9;
    }
    vector<string> msg;

    BOOST_FOREACH(float wl, p.wire_lengths) {
        msg.push_back((_("%.1f") % wl).str());
    }
    logger.debug(_("  CUDA wirelength: %.1f = %s")
                % total_wirelength
                % boost::algorithm::join(msg, "+")
            );

    double sub_duration;
    double cpu_wirelength = 0;
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        BOOST_FOREACH(net_range &nets_sample, kl_prop.cpu_net_samples) {
            cpu_wirelength += nets_sample_wl(nets_sample, sub_duration);
            duration.calc += sub_duration;
        }
    }

    total_wirelength += cpu_wirelength;
    return total_wirelength;
}


double StarPlusCalculator::nets_sample_wl(
        net_range &nets, double &duration) {
    double alpha = 1.0;
    double beta = 1.59;
    timespec start, end, delta;
    int net_count = nets.size();

    double total_wirelength = 0;

    uint32_t x, y;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    BOOST_FOREACH(net_ptr const &net, nets) {
        double x_sum = 0;
        double y_sum = 0;
        double x_squared_sum = 0;
        double y_squared_sum = 0;

        block_list const &blocks = net->get_blocks();
        int num_blocks = blocks.size();
        BOOST_FOREACH(block_ptr const &block, blocks) {
            x_sum += block->x;
            y_sum += block->y;

            x_squared_sum += block->x * block->x;
            y_squared_sum += block->y * block->y;
        }

        double starplus_x = alpha * sqrt(x_squared_sum - (x_sum * x_sum / num_blocks) + beta);
        double starplus_y = alpha * sqrt(y_squared_sum - (y_sum * y_sum / num_blocks) + beta);

        total_wirelength += starplus_x + starplus_y;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    delta = time_diff(start, end);
    duration = delta.tv_sec + delta.tv_nsec * 1e-9;
    return total_wirelength;
}


double HPWLCalculator::wirelength_cpu(double &duration) {
    CircuitPartitioner &p = *part;

    if(p.total_block_count == 0) return 0;
    int blkIdx = 0;
    double hpwl = 0;
    double crossing;

    vector<kl_properties_t> &kl_properties = p.kl_properties;
    vector<ushort2> &block_coords = p.block_coords;
    vector<BlockMem> &block_offsets = p.block_offsets;
    static_host_data &host_data = p.host_data;

    timespec start, end, delta;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        vector<net_range> &cuda_nets = kl_prop.cuda_net_samples;
        int kl = kl_prop.kl;

        if(kl > 50) {
            crossing = 2.7933 + 0.02616 * (kl - 50);
        } else {
            crossing = cross_count[kl - 1];
        }
        BOOST_FOREACH(net_range &nets_sample, cuda_nets) {
            double blk_hpwl = 0;
            BlockMem &mem_offsets = block_offsets[blkIdx];

            irange sizes = slice(host_data.sizes, 
                        mem_offsets.sizes, 
                        mem_offsets.sizes + 2);
            irange block_ids_all = slice(host_data.block_ids, 
                        mem_offsets.block_ids, 
                        mem_offsets.block_ids + sizes[0]);
            int num_nets = nets_sample.size();

            int coords_size = sizes[1];
            coord_range r_block_coords = slice(block_coords, mem_offsets.block_coords, 
                                        mem_offsets.block_coords + coords_size);

            BOOST_FOREACH(int i, xrange(num_nets)) {
                irange block_ids = slice(block_ids_all, 
                            i * kl, 
                            (i + 1) * kl);
                uint32_t min_x = std::numeric_limits<uint32_t>::max();
                uint32_t max_x = 0;
                uint32_t min_y = std::numeric_limits<uint32_t>::max();
                uint32_t max_y = 0;

                BOOST_FOREACH(uint16_t id, block_ids) {
                    ushort2 val_temp = r_block_coords[id];
                    if(val_temp.x < min_x) min_x = val_temp.x;
                    if(val_temp.x > max_x) max_x = val_temp.x;
                    if(val_temp.y < min_y) min_y = val_temp.y;
                    if(val_temp.y > max_y) max_y = val_temp.y;
                }
                double wl = ((max_x - min_x) + (max_y - min_y)) / 2.0;
                wl *= crossing;
                blk_hpwl += wl;
            }
            hpwl += blk_hpwl;
            blkIdx++;
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    delta = time_diff(start, end);
    duration = delta.tv_sec + delta.tv_nsec * 1e-9;

    double sub_duration;
    BOOST_FOREACH(kl_properties_t &kl_prop, kl_properties) {
        BOOST_FOREACH(net_range &nets_sample, kl_prop.cpu_net_samples) {
            hpwl += nets_sample_wl(nets_sample, sub_duration);
            duration += sub_duration;
        }
    }
    return hpwl;
}


double HPWLCalculator::wirelength(kernel_durations_t &duration) {
    CircuitPartitioner &p = *part;

    double total_wirelength = 0;
    timespec start, end, delta;

    duration.calc = 0;
    duration.wirelength_copy = 0;
    if(p.total_block_count > 0) {
        kernel_durations_t durations = _launch_kernel();
        duration.calc += durations.calc;
        duration.wirelength_copy += durations.wirelength_copy;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        BOOST_FOREACH(float wl, p.wire_lengths) {
            total_wirelength += wl;
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        delta = time_diff(start, end);
        duration.calc += delta.tv_sec + delta.tv_nsec * 1e-9;
    }

    double sub_duration;
    BOOST_FOREACH(kl_properties_t &kl_prop, p.kl_properties) {
        BOOST_FOREACH(net_range &nets_sample, kl_prop.cpu_net_samples) {
            total_wirelength += nets_sample_wl(nets_sample, sub_duration);
            duration.calc += sub_duration;
        }
    }
    return total_wirelength;
}


double HPWLCalculator::nets_sample_wl(net_range &nets, 
                                                double &duration) {
    timespec start, end, delta;
    int net_count = nets.size();

    uint32_t x, y;
    double hpwl = 0;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    BOOST_FOREACH(net_ptr const &net, nets) {
        int num_blocks = net->pin_count();
        uint32_t min_x = std::numeric_limits<uint32_t>::max();
        uint32_t max_x = 0;
        uint32_t min_y = std::numeric_limits<uint32_t>::max();
        uint32_t max_y = 0;
        double crossing;
        double hpwl_part = 0;

        if(num_blocks > 50) {
            crossing = 2.7933 + 0.02616 * (num_blocks - 50);
        } else {
            crossing = cross_count[num_blocks - 1];
        }

        block_list const &blocks = net->get_blocks();
        BOOST_FOREACH(block_ptr const &block, blocks) {
            if(block->x < min_x) min_x = block->x;
            if(block->x > max_x) max_x = block->x;
            if(block->y < min_y) min_y = block->y;
            if(block->y > max_y) max_y = block->y;
        }

        hpwl_part = ((max_x - min_x) + (max_y - min_y)) / 2.0;
        hpwl += hpwl_part * crossing;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    delta = time_diff(start, end);
    duration = delta.tv_sec + delta.tv_nsec * 1e-9;
    return hpwl;
}


kernel_durations_t ConstantKlStarPlusCalculator::_launch_kernel() {
    CircuitPartitioner &p = *part;
    return launch_constant_kl_kernel((constant_kl_kernel_ptr)p.kernel, 
                    p.threads, 
                    p.total_block_count,
                    p.max_shared_mem,
                    p.min_kl, p.max_kl, 
                    p.d_data,
                    p.wire_lengths);
}


kernel_durations_t SharedCoordsConstantKlStarPlusCalculator::_launch_kernel() {
    CircuitPartitioner &p = *part;
    return launch_constant_kl_kernel(
                    (constant_kl_kernel_ptr)p.kernel, 
                    p.threads, 
                    p.total_block_count,
                    p.max_shared_mem,
                    p.min_kl, p.max_kl, 
                    p.d_data,
                    p.wire_lengths);
}
