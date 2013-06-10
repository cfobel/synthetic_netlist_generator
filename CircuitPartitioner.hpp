#ifndef ___CIRCUIT_PARTITIONER_HPP___
#define ___CIRCUIT_PARTITIONER_HPP___

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <cuda_runtime.h>

#include "CircuitPartitionerCuda.hpp"
#include "Circuit.hpp"
#include "CudaInfo.hpp"
#include "shared.hpp"


using namespace circuit;
using namespace cuda_info;


#define _ boost::format

struct kl_properties_t {
    vector<net_range> cuda_net_samples;
    vector<net_range> cpu_net_samples;
    int net_count;
    int kl;

    kl_properties_t(int kl, int net_count) : kl(kl), net_count(net_count){}
};


struct NetData {
    int kl;
    irange block_ids;
    coord_range block_coords;

    NetData() {}
    NetData(int kl, irange block_ids, coord_range block_coords) 
        : kl(kl), block_ids(block_ids), block_coords(block_coords) {}
    string toString(int id=-1);
    double starplus();
};


extern "C"
void configure_cuda_cache(kernel_ptr kernel, 
            cudaFuncCache cache_preference);

/******************************************************************************/
/** Circuit Partitioners                                                      */

class CircuitPartitioner {
public:
    CudaDevice &cuda_device;
    CudaKernel cuda_kernel;
    vector<kl_properties_t> kl_properties;
    int net_count_threshold;
    int max_nets_per_block;
    cudaFuncCache cache_pref;
    kernel_ptr kernel;

    Circuit &c;
    int min_kl;
    int max_kl;
    static_host_data host_data;

    cuda_data_addrs d_data;
    vector<ushort2> block_coords;
    vector<float> wire_lengths;
    int total_block_count;
    int max_shared_mem_available;
    int max_shared_mem;
    int max_sample_size;
    int min_sample_size;
    int threads;
    double coords_copy_duration;

    vector<BlockMem> block_offsets;
    vector<block_id_map> block_mapping_list;
    vector<net_range> net_range_list;
    vector<NetData> net_data_list;

    void update_cuda_block_coords();
    void fill_host_data();
    void load_static_cuda_data();
    void partition_nets();

    void cuda_alloc_and_copy(void** dst, void* src, int size, string label="");
    void cuda_alloc(void** dst, int size, string label="");
    int _calc_sample_shared_mem(int sample_size, int mem_per_net, int overhead);
    virtual void copy_block_coords_cuda_data();

    virtual int mem_size_kl_mapping() {
        return (host_data.kl_mapping.size() * sizeof(host_data.kl_mapping[0]));
    }
    virtual int mem_size_sizes() {
        return (host_data.sizes.size() * sizeof(host_data.sizes[0]));
    }
    virtual int mem_size_block_ids() {
        return (host_data.block_ids.size() * sizeof(host_data.block_ids[0]));
    }
    virtual int mem_size_wire_length() {
        return (host_data.wire_length_size * sizeof(float));
    }
    virtual void check_mem_size() {
        int gmem_reqd = 
            mem_size_sizes() +
            mem_size_kl_mapping() +
            mem_size_block_ids() +
            mem_size_wire_length();
        size_t mem_free, total;

        cuMemGetInfo(&mem_free, &total);
        if(gmem_reqd > mem_free) {
            printf("CUDA global memory: %d/%d bytes\n", (int)mem_free, (int)total);
            printf("CUDA memory reqd:   %d\n", gmem_reqd);
            printf("  kl_mapping:   %d bytes\n", mem_size_kl_mapping());
            printf("  sizes:        %d bytes\n", mem_size_sizes());
            printf("  block_ids:    %d bytes\n", mem_size_block_ids());
            printf("  wire_length:  %d bytes\n", mem_size_wire_length());
            fprintf(stderr, "ERROR: not enough global CUDA memory\n");
            exit(EXIT_FAILURE);
        }
    }
    virtual void copy_kl_mapping() {
        // Allocate total memory required for 'kl_mapping' for all CUDA blocks.
        cuda_alloc_and_copy((void**)&d_data.kl_mapping, 
                            (void*)&host_data.kl_mapping[0],
                            mem_size_kl_mapping(), string("kl_mapping"));
    }
    virtual void copy_sizes() {
        // Allocate total memory required for 'sizes' for all CUDA blocks.
        cuda_alloc_and_copy((void**)&d_data.sizes, 
                            (void*)&host_data.sizes[0],
                            mem_size_sizes(), "sizes");
    }
    virtual void copy_block_ids() {
        // Allocate memory required for block_ids (for all CUDA blocks)
        cuda_alloc_and_copy((void**)&d_data.block_ids, 
                            (void*)&host_data.block_ids[0],
                            mem_size_block_ids(), "block_ids");
    }



    virtual void copy_static_cuda_data();
    virtual int calc_netlist_sample_size(int num_nets, int kl);
    virtual int shared_mem_per_net(int kl) {
        return 0;
    }
    virtual int shared_mem_overhead(int kl_mapping_size) {
        // No overhead for naive implementation
        return 0;
    }

    CircuitPartitioner(Circuit &circuit, 
                CudaDevice &cuda_device,
                cudaFuncCache cache_pref,
                kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : c(circuit), 
            cuda_device(cuda_device),
            net_count_threshold(net_count_threshold),
            max_nets_per_block(max_nets_per_block),
            max_sample_size(0), 
            min_sample_size(std::numeric_limits<int>::max()),
            min_kl(c.get_min_kl()), max_kl(c.get_max_kl()),
            max_shared_mem(0), cuda_kernel(CudaKernel(kernel)), 
            cache_pref(cache_pref),
            kernel(kernel) {
        max_shared_mem_available = 
                cuda_device.get_max_shared_memory(cache_pref);
        configure_cuda_cache(kernel, cache_pref);
    }

    void reset() {
        kl_properties.clear();
        block_coords.clear();
        wire_lengths.clear();
        block_offsets.clear();
        block_mapping_list.clear();
        net_range_list.clear();
        net_data_list.clear();
        d_data.free();
    }

    ~CircuitPartitioner() {
        reset();
    }
};


class SharedPartitioner : public CircuitPartitioner {
public:
    SharedPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : CircuitPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, max_nets_per_block,
                                net_count_threshold) {}
    int shared_mem_per_net(int kl);
    int shared_mem_overhead(int kl_mapping_size);
};


class SharedCoordsPartitioner : public CircuitPartitioner {
public:
    SharedCoordsPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : CircuitPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, 
                                max_nets_per_block, 
                                net_count_threshold) {}
    int shared_mem_per_net(int kl);
    int shared_mem_overhead(int kl_mapping_size);
};


class ConstantCrossPartitioner : public CircuitPartitioner {
public:
    ConstantCrossPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : CircuitPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, 
                                max_nets_per_block, 
                                net_count_threshold) {}
    void copy_static_cuda_data();
};


class ConstantKlPartitioner : public CircuitPartitioner {
public:
    ConstantKlPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : CircuitPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, 
                                max_nets_per_block, 
                                net_count_threshold) {}
    void copy_kl_mapping();
};


class SharedCoordsConstantKlPartitioner : public SharedCoordsPartitioner {
public:
    SharedCoordsConstantKlPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : SharedCoordsPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, 
                                max_nets_per_block, 
                                net_count_threshold) {}
    void copy_kl_mapping();
};


class SharedConstantCrossPartitioner : public CircuitPartitioner {
public:
    SharedConstantCrossPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : CircuitPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, 
                                max_nets_per_block, 
                                net_count_threshold) {}
    void copy_static_cuda_data();
    int shared_mem_per_net(int kl);
    int shared_mem_overhead(int kl_mapping_size);
};


class SharedCoordsConstantCrossPartitioner : public CircuitPartitioner {
public:
    SharedCoordsConstantCrossPartitioner(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            kernel_ptr kernel, 
                int max_nets_per_block, int net_count_threshold=0)
        : CircuitPartitioner(circuit, cuda_device,
                                cache_pref,
                                kernel, 
                                max_nets_per_block, 
                                net_count_threshold) {}
    void copy_static_cuda_data();
    int shared_mem_per_net(int kl);
    int shared_mem_overhead(int kl_mapping_size);
};

/*---------------------------------------------------------------------------*/



/******************************************************************************/
/** Wirelength Calculators                                                    */

class WirelengthCalculator {
protected:
    Circuit &c;
public:
    WirelengthCalculator(Circuit &circuit) 
        : c(circuit) {}
    virtual double wirelength(kernel_durations_t &duration) = 0;
    virtual double wirelength_cpu(double &duration) = 0;
    virtual ~WirelengthCalculator() {};
};


class CudaCalculator : WirelengthCalculator {
    virtual double nets_sample_wl(net_range &nets, double &duration) = 0;
protected:
    Circuit &c;
    CudaDevice &cuda_device;
    cudaFuncCache cache_pref;
    CircuitPartitioner* part;
    virtual kernel_durations_t _launch_kernel();
    int net_count_threshold;
    int max_nets_per_block;
public:
    CudaCalculator(Circuit &circuit, 
            CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
            : WirelengthCalculator(circuit),
                net_count_threshold(net_count_threshold),
                max_nets_per_block(max_nets_per_block),
                cuda_device(cuda_device),
                cache_pref(cache_pref),
                c(circuit) {}
    void init() {
        part->partition_nets();
        part->load_static_cuda_data();
        part->update_cuda_block_coords();
    }

    void init(CircuitPartitioner* p) {
        part = p;
        init();
    }
    void set_max_nets_per_block(int max_nets) { 
        max_nets_per_block = max_nets; 
        part->max_nets_per_block = max_nets;
        part->reset();
    }
    void set_net_count_threshold(int threshold) { 
        net_count_threshold = threshold; 
        part->net_count_threshold = threshold;
        part->reset();
    }

    virtual double wirelength(kernel_durations_t &duration) = 0;
    virtual double wirelength_cpu(double &duration) = 0;
    int get_total_block_count() const { return part->total_block_count; }
    double get_coords_copy_duration() const { return part->coords_copy_duration; }
    int get_cuda_threads() const { return part->threads; }
    int get_max_sample_size() const { return part->max_sample_size; }
    int get_min_sample_size() const { return part->min_sample_size; }
    int get_max_nets_per_block() const { return part->max_nets_per_block; }
    int get_max_shared_mem() const { return part->max_shared_mem; }
    virtual ~CudaCalculator() {
        delete part;
    }
};


class StarPlusCalculator : public CudaCalculator {
    double nets_sample_wl(net_range &nets, double &duration);
public:
    StarPlusCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block, 
            int net_count_threshold)
        : CudaCalculator(circuit, cuda_device,
                                cache_pref,
                max_nets_per_block,
                net_count_threshold) {}

    double wirelength(kernel_durations_t &duration);
    double wirelength_cpu(double &duration);
};


class HPWLCalculator : public CudaCalculator {
    double nets_sample_wl(net_range &nets, double &duration);
public:
    HPWLCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : CudaCalculator(circuit, cuda_device,
                                cache_pref,
                max_nets_per_block,
                net_count_threshold) {}

    double wirelength(kernel_durations_t &duration);
    double wirelength_cpu(double &duration);
};


class SharedStarPlusCalculator : public StarPlusCalculator {
protected:
    Circuit &c;
public:
    SharedStarPlusCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : StarPlusCalculator(circuit, cuda_device,
                                cache_pref,
                max_nets_per_block,
                net_count_threshold), c(circuit) {
        init(new SharedPartitioner(c, cuda_device,
                                    cache_pref,
                    (kernel_ptr)&starplus_shared,
                    max_nets_per_block, net_count_threshold));
    }
};


class SharedCoordsStarPlusCalculator : public StarPlusCalculator {
protected:
    Circuit &c;
public:
    SharedCoordsStarPlusCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : StarPlusCalculator(circuit, cuda_device,
                                cache_pref,
                max_nets_per_block,
                net_count_threshold), c(circuit) {
        init(new SharedCoordsPartitioner(c, cuda_device,
                                    cache_pref,
                    (kernel_ptr)&starplus_shared_coords,
                    max_nets_per_block, net_count_threshold));
    }
};


class ConstantKlStarPlusCalculator : public StarPlusCalculator {
protected:
    Circuit &c;
    kernel_durations_t _launch_kernel();
public:
    ConstantKlStarPlusCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : StarPlusCalculator(circuit, cuda_device,
                                cache_pref,
                max_nets_per_block,
                net_count_threshold), c(circuit) {
        init(new ConstantKlPartitioner(c, cuda_device,
                                    cache_pref,
                    (kernel_ptr)&starplus_constant_kl,
                    max_nets_per_block, net_count_threshold));
    }
};


class SharedCoordsConstantKlStarPlusCalculator : public StarPlusCalculator {
protected:
    Circuit &c;
    kernel_durations_t _launch_kernel();
public:
    SharedCoordsConstantKlStarPlusCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,

                                        int max_nets_per_block,
                                        int net_count_threshold)
        : StarPlusCalculator(circuit, cuda_device,
                                cache_pref,
                max_nets_per_block,
                net_count_threshold), c(circuit) {
        init(new SharedCoordsConstantKlPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&starplus_shared_coordsconstant_kl,
                    max_nets_per_block, net_count_threshold));
    }
};


class SharedHPWLCalculator : public HPWLCalculator {
public:
    SharedHPWLCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : HPWLCalculator(circuit, cuda_device,
                cache_pref,
                max_nets_per_block,
                net_count_threshold) {
        init(new SharedPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&hpwl_shared,
                    max_nets_per_block, net_count_threshold));
    }
};


class NaiveStarPlusCalculator : public StarPlusCalculator {
protected:
    Circuit &c;
public:
    NaiveStarPlusCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : StarPlusCalculator(circuit, cuda_device,
                cache_pref,
                max_nets_per_block,
                net_count_threshold), c(circuit) {
        init(new CircuitPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&starplus_naive,
                    max_nets_per_block, net_count_threshold));
    }
};


class NaiveHPWLCalculator : public HPWLCalculator {
public:
    NaiveHPWLCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : HPWLCalculator(circuit, cuda_device,
                cache_pref,
                max_nets_per_block,
                net_count_threshold) {
        init(new CircuitPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&hpwl_naive,
                    max_nets_per_block, net_count_threshold));
    }
};


class ConstantCrossHPWLCalculator : public HPWLCalculator {
public:
    ConstantCrossHPWLCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : HPWLCalculator(circuit, cuda_device,
                cache_pref,
                max_nets_per_block,
                net_count_threshold) {
        init(new ConstantCrossPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&hpwl_constant_cross,
                    max_nets_per_block, net_count_threshold));
    }
};


class SharedConstantCrossHPWLCalculator : public HPWLCalculator {
public:
    SharedConstantCrossHPWLCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : HPWLCalculator(circuit, cuda_device,
                cache_pref,
                max_nets_per_block,
                net_count_threshold) {
        init(new SharedConstantCrossPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&hpwl_shared_constant_cross,
                    max_nets_per_block, net_count_threshold));
    }
};


class SharedCoordsConstantCrossHPWLCalculator : public HPWLCalculator {
public:
    SharedCoordsConstantCrossHPWLCalculator(Circuit &circuit, CudaDevice &cuda_device,
            cudaFuncCache cache_pref,
            int max_nets_per_block,
            int net_count_threshold)
        : HPWLCalculator(circuit, cuda_device,
                cache_pref,
                max_nets_per_block,
                net_count_threshold) {
        init(new SharedCoordsConstantCrossPartitioner(c, cuda_device,
                    cache_pref,
                    (kernel_ptr)&hpwl_shared_coords_constant_cross,
                    max_nets_per_block, net_count_threshold));
    }
};

/*---------------------------------------------------------------------------*/

#endif
