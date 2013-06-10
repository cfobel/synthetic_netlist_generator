/**
   @file Circuit.cpp
   @brief Code for the Circuit, Net and Block classes.

   System:         Star+ CUDA (C++)
   Language: C++

   (c) Copyright Christian Fobel 2010
  Author: Christian Fobel
   E-Mail: cfobel@uoguelph.ca

   Compiler Options: -lboost_regex -lboost_filesystem

   Requires: Boost C++ Libraries >= 1.40.0 (http://www.boost.org/doc/)

   Todo:
   @todo Add code to gather data required for calculation of Star+ using CUDA.
*/

#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include <boost/assign/list_of.hpp>       // for 'list_of()' and 'ref_list_of()'
#include <boost/assign/list_inserter.hpp> // for 'push_back()'
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Circuit.hpp"
#include "CircuitInfo.hpp"
#include "Logging.hpp"
#include "RouletteWheel.hpp"
#include "timing.hpp"
#include "hpwl.hpp"

extern Logger logger;

namespace circuit {

using namespace boost::assign;

//! Operator overload for outputting Net representation to ostream object.
ostream& operator<<(ostream& out, Net &obj) {
        return out << obj.toString();
}

//! Operator overload for outputting Block representation to ostream object.
ostream& operator<<(ostream& out, Block &obj) {
        return out << obj.toString();
}

//! Operator overload for outputting Circuit representation to ostream object.
ostream& operator<<(ostream& out, Circuit &obj) {
        return out << obj.toString();
}

//! Calculate the Star+ wirelength estimate for the net.
inline double Net::wirelength(double alpha, double beta) {
    int num_blocks = pin_count();

    if(num_blocks == 0) {
        return 0;
    }

    double x_sum = 0;
    double y_sum = 0;
    double x_squared_sum = 0;
    double y_squared_sum = 0;

    BOOST_FOREACH(block_ptr &block, blocks) {
        x_sum += block->x;
        y_sum += block->y;

        x_squared_sum += block->x * block->x;
        y_squared_sum += block->y * block->y;
    }

    double starplus_x = alpha * sqrt(x_squared_sum - (x_sum * x_sum / num_blocks) + beta);
    double starplus_y = alpha * sqrt(y_squared_sum - (y_sum * y_sum / num_blocks) + beta);

    double result = starplus_x + starplus_y;
    return result;
}


//! Calculate the HPWL wirelength estimate for the net.
double Net::hpwl() {
    int num_blocks = pin_count();

    if(num_blocks == 0) {
        return 0;
    }

    uint32_t min_x = std::numeric_limits<uint32_t>::max();
    uint32_t max_x = 0;
    uint32_t min_y = std::numeric_limits<uint32_t>::max();
    uint32_t max_y = 0;
    double hpwl = 0;
    double crossing;

    if(num_blocks > 50) {
        crossing = 2.7933 + 0.02616 * (num_blocks - 50);
    } else {
        crossing = cross_count[num_blocks - 1];
    }

    BOOST_FOREACH(block_ptr &block, blocks) {
        if(block->x < min_x) min_x = block->x;
        if(block->x > max_x) max_x = block->x;
        if(block->y < min_y) min_y = block->y;
        if(block->y > max_y) max_y = block->y;
    }

    hpwl = ((max_x - min_x) + (max_y - min_y)) / 2.0;
    return hpwl * crossing;
}


int Net::pin_count() const {
    return blocks.size();
}


//! Add a block to the net.
void Net::add_block(block_ptr const &block) {
    blocks.push_back(block);
}


string Net::toString() {
    vector<string> summary;
    summary.push_back((_("Net: %s") % label).str());

    BOOST_FOREACH(block_ptr &blk, blocks) {
        summary.push_back((_("  %s") % *blk).str());
    }
    return boost::algorithm::join(summary, "\n");
}


string Block::toString() const {
    return (_("%d [%d, %d]") % label % x % y).str();
}


string Circuit::toString() const {
    vector<string> summary;
    summary.push_back((_("Circuit: %s") % label).str());

    summary.push_back((_("  Inputs:      %d") % inputs_count()).str());
    summary.push_back((_("  Outputs:     %d") % outputs_count()).str());
    summary.push_back((_("  Nets:        %d") % nets_count()).str());
    summary.push_back((_("  I/O Nets:    %d") % input_nets.size()).str());
    summary.push_back((_("  CLBs:        %d") % clbs_count()).str());
    summary.push_back((_("  Nets/Block:  %.2f") % ((double)nets_count() / clbs_count())).str());

    return boost::algorithm::join(summary, "\n");
}


string Circuit::dump_vpr_benchmark() const {
    vector<string> summary;
    vector<string> pin_list;

    logger.notice("dump_vpr_benchmark()");
    vector<net_list> block_nets(blocks.size());

    block_id_map block_mapping;

    int i = 0;
    BOOST_FOREACH(block_map::value_type item, blocks) {
        block_mapping[item.second.get()] = i++;
    }

    BOOST_FOREACH(net_map::value_type const &item, nets) {
        BOOST_FOREACH(block_ptr const &net_blk, item.second->get_blocks()) {
            block_nets[block_mapping[net_blk.get()]].push_back(item.second);
        }
    }

    i = 0;
    BOOST_FOREACH(block_map::value_type const &blk_item, blocks) {
        summary.push_back((_(".clb %s") % blk_item.first).str());
        pin_list.clear();
        BOOST_FOREACH(net_ptr const &p_net, block_nets[i++]) {
            pin_list.push_back(p_net->get_label());
        }
        summary.push_back((_("pinlist: %s")
                    % boost::algorithm::join(pin_list, " ")).str());
        summary.push_back("");
    }
    return boost::algorithm::join(summary, "\n");
}


string Circuit::get_coords_list() const {
    vector<string> summary;
    summary.push_back("Block coords:");


    int i = 0;
    BOOST_FOREACH(block_map::value_type data, blocks) {
        summary.push_back((_("  [%d]: (%2d,%2d)") % i
                    % data.second->x % data.second->y).str());
        i++;
    }
    return boost::algorithm::join(summary, "\n");
}

//! Add a block to the circuit.
void Circuit::add_net(net_ptr const &net) {
    nets[net->get_label()] = net;
    return;
}


//! Add a block to the circuit.
void Circuit::add_block(block_ptr const &block) {
    if(block->get_type() == INPUT) {
        inputs[block->get_label()] = block_ptr(block);
    } else if(block->get_type() == OUTPUT) {
        outputs[block->get_label()] = block_ptr(block);
    } else if(block->get_type() == CLB) {
        blocks[block->get_label()] = block_ptr(block);
    }
}

//! Parse all blocks and nets from the specified benchmark filepath.
/*! Parse all blocks and nets from the specified benchmark filepath and
 * randomly initialize block positions within a grid whose dimensions are
 * calculated based on criteria taken from VPR.
 * @see calc_fpga_dims */
void Circuit::parse_file(fs::path input_file) {
    timespec start, end, delta;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    label = input_file.string();
    fs::basic_ifstream<char>
        in_stream(input_file, std::ios_base::in);
    net_ptr net;

    // Initialize parser with 32KB read buffer
    VPRNetParser parser(32 << 10);

    parser.init();
    parser.register_input_process_func(boost::bind(&Circuit::process_block, this,
                                                    _1, INPUT, _2));
    parser.register_output_process_func(boost::bind(&Circuit::process_block, this,
                                                    _1, OUTPUT, _2));
    parser.register_clb_process_func(boost::bind(&Circuit::process_block, this,
                                                    _1, CLB, _2));
    parser.parse(in_stream);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    delta = time_diff(start, end);
    double duration = delta.tv_sec + delta.tv_nsec * 1e-9;
    logger.notice(_("Parsing took %.8fs") % duration);

    init_by_kl();
    init_block_positions();
}


void Circuit::process_block(const string &block_label, block_type type,
                            const vector<string> &pins) {
    net_ptr net;
    block_ptr block = block_ptr(new Block(block_label, type));

    add_block(block);

    BOOST_FOREACH(string label, pins) {
        if(input_nets.count(label) > 0) {
            // This is an I/O net... skip it
            continue;
        } else if(type != CLB) {
            // We have an input/output
            net = net_ptr(new Net(label));
            input_nets[net->get_label()] = net;
            continue;
        } else if(nets.count(label) > 0) {
            /* we've seen this net before */
            net = nets[label];
        } else {
            /* we need to add a new net object to the circuit */
            net = net_ptr(new Net(label));
            add_net(net);
        }
        /* if the block is CLB, add it to the net's pin list */
        net->add_block(block);
    }
}


/*! Generate initial coordinates for blocks */
void Circuit::init_block_positions() {
    int nx, ny;
    boost::tie(nx, ny) = calc_fpga_dims(clbs_count(), inputs_count(), outputs_count());
    logger.notice(_("assuming FPGA grid with dimensions (%d blocks): "
                "%d x %d") % clbs_count() % nx % ny);

    /* Generate random list of unique coordinates, one coord for each
     * block in the circuit. */
    coord_set coords(clbs_count());
    get_coord_set(coords, nx, ny);

    int i = 0;
    BOOST_FOREACH(block_map::value_type data, blocks) {
        data.second->x = coords[i].get<0>();
        data.second->y = coords[i].get<1>();
        i++;
    }
}


int Circuit::get_nets_by_kl(int kl, net_list_ptr &list_ptr) {
    int count = kl_nets_count(kl);

    if(count == 0) {
        list_ptr = 0;
        return 0;
    }
    list_ptr = net_list_ptr(&nets_by_kl[kl]);
    return list_ptr->size();
}


//! Calculate the total Star+ wirelength estimate for the circuit.
double Circuit::wirelength(double &duration, double alpha, double beta) {
    double wirelength = 0;

    timespec start, end, delta;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    BOOST_FOREACH(net_map::value_type &data, nets) {
        wirelength += data.second->wirelength();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    delta = time_diff(start, end);
    duration = delta.tv_sec + delta.tv_nsec * 1e-9;
    return wirelength;
}


//! Calculate the total HPWL wirelength estimate for the circuit.
double Circuit::hpwl(double &duration) {
    double wirelength = 0;

    timespec start, end, delta;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    BOOST_FOREACH(net_map::value_type &data, nets) {
        wirelength += data.second->hpwl();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    delta = time_diff(start, end);
    duration = delta.tv_sec + delta.tv_nsec * 1e-9;
    return wirelength;
}


void Circuit::random_init(fs::path pdf_path, bool exact_values, int block_count) {
    fs::ifstream istream;
    istream.open(pdf_path, std::ios_base::in);
    boost::archive::text_iarchive ar(istream);
    //PdfMap pdf;
    CircuitProfile profile;
    ar & profile;
    istream.close();

    PdfMap const &pdf = profile.get_pdf_map();
    double net_to_block_ratio = profile.get_net_to_block_ratio();
    int net_count = block_count * net_to_block_ratio;

    PdfMap net_counts;
    BOOST_FOREACH(PdfMap::value_type const &item, pdf) {
        int kl = item.first;
        if(exact_values) {
            net_counts[kl] = item.second;
        } else {
            net_counts[kl] = item.second * net_count;
        }
    }

    random_init(net_counts, block_count);
}


void Circuit::random_init(PdfMap const &net_counts, int block_count) {
    int nx, ny;
    boost::tie(nx, ny) = calc_fpga_dims(block_count, 0, 0);
    coord_set coords(block_count);
    get_coord_set(coords, nx, ny);


    logger.notice(_("sampled coords (%d)") % coords.size());
    logger.notice(_("assuming FPGA grid with dimensions (%d blocks): %d x %d")
                        % block_count % nx % ny);

    int i = 0;
    BOOST_FOREACH(coord_set::value_type &coord, coords) {
        string label = (_("block_%d") % i++).str();
        block_ptr b(new Block(label, CLB));
        b->x = coord.get<0>();
        b->y = coord.get<1>();
        add_block(b);
    }

    block_list blocks_v(blocks.size());
    i = 0;
    BOOST_FOREACH(block_map::value_type const &item, blocks) {
        blocks_v[i++] = item.second;
    }

    block_map blocks_sample;
    boost::uniform_int<> dist(0, blocks_v.size() - 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(*random_gen, dist);

    int net_id = 0;
    BOOST_FOREACH(PdfMap::value_type const &item, net_counts) {
        int kl = item.first;
        int count = item.second;
        BOOST_FOREACH(int j, xrange(count)) {
            string label = (_("net_%d") % net_id++).str();
            net_ptr net(new Net(label));
            int sample_size = min(kl, block_count - 1);
            blocks_sample.clear();
            while(blocks_sample.size() < sample_size) {
                int block_id = die();
                block_ptr b(blocks_v[block_id]);
                blocks_sample[b->get_label()] = b;
            }
            BOOST_FOREACH(block_map::value_type const &item, blocks_sample) {
                net->add_block(item.second);
            }
            add_net(net);
        }
    }
    init_by_kl();
}


void Circuit::init_by_kl() {
    int removed_count = 0;

    BOOST_FOREACH(net_map::value_type &item, nets) {
        string net_label = item.second->get_label();
        boost::to_lower(net_label);

        if(net_label.find("clk") != string::npos) {
            logger.notice(_("  removing 'clk' net: %s") % net_label);
            nets.erase(item.first);
        }
    }

    int nets_added = 0;
    /* Find nets with a single pin and remove them from the circuit */
    BOOST_FOREACH(net_map::value_type &data, nets) {
        int kl = data.second->pin_count();
        if(kl <= 1) {
            nets.erase(data.first);
            removed_count++;
            // Continue processing the next net.
            continue;
        }
        if(nets_by_kl.count(kl) == 0) {
            // We need to add a net_list object for cardinality kl
            nets_by_kl[kl] = net_list();
        }
        // Add the current net to the net_list for cardinality kl
        nets_by_kl[kl].push_back(data.second);
        nets_added++;
    }
    BOOST_FOREACH(net_map::value_type &data, nets) {
//        logger.error(data.second->get_label());
    }

    BOOST_FOREACH(net_list_map::value_type &data, nets_by_kl) {
        if(data.first > max_kl) max_kl = data.first;
        if(data.first < min_kl) min_kl = data.first;
    }

    //logger.error("<<<<<<<<<<<<<<<<<<< BY KL");
    net_list_ptr nl_ptr;
    int net_kl_count = 0;
    BOOST_FOREACH(int kl, xrange(min_kl, max_kl + 1)) {
        if(nets_by_kl.count(kl) == 0) continue;
        get_nets_by_kl(kl, nl_ptr);

        net_list &nets = *nl_ptr;
        net_kl_count += nets.size();
        BOOST_FOREACH(net_ptr &net, nets) {
            //logger.error(net->get_label());
        }
    }
    logger.debug(_("  Total nets:  %d (net_kl_count: %d, nets_added: %d)")
                                % nets_count() % net_kl_count % nets_added);

    if(removed_count > 0) {
        logger.notice(_("  %d single pin nets were removed.") % removed_count);
    }
}


//! Calculate the X and Y dimensions of the FPGA grid size to use.
/*! Calculate the X and Y dimensions to use based on the number of
 *  inputs, outputs and CLBs, according to what is done in VPR. */
boost::tuple<int, int> calc_fpga_dims(
            int num_clbs, int num_p_inputs,
            int num_p_outputs, double aspect_ratio,
            double io_rat) {

    int ny = ceil(sqrt(num_clbs / aspect_ratio));
    int io_lim = ceil((num_p_inputs + num_p_outputs) / (2 * io_rat * (1 + aspect_ratio)));
    ny = max(ny, io_lim);
    int nx = ceil(ny * aspect_ratio);
    return boost::make_tuple(nx, ny);
}


//! Fill a set of grid positions with random unique coordinates.
/*! Fill a set of grid positions with random unique coordinates within the
 * specified X andy Y dimensions.  */
void Circuit::get_coord_set(coord_set &coords, int nx, int ny) {
    int num_positions = nx * ny;
    coord_set grid_positions(num_positions);
    logger.notice(_("using (nx,ny)=(%d,%d)") % nx % ny);
    boost::uniform_int<> dist(0, num_positions - 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(*random_gen, dist);
    int p1, p2;

    /* Generate all possible grid positions. */
    BOOST_FOREACH(int x, xrange(nx)) {
        BOOST_FOREACH(int y, xrange(ny)) {
            p1 = y + x * ny;
            grid_positions[p1] = boost::make_tuple(x, y);
        }
    }

    /* Shuffle the list of coordinates. */
    boost::tuple<int, int> temp;
    BOOST_FOREACH(int i, xrange(num_positions)) {
        p1 = die();
        p2 = die();
        if(p1 != p2) {
            temp = grid_positions[p1];
            grid_positions[p1] = grid_positions[p2];
            grid_positions[p2] = temp;
        }
    }

    int num_coords = min((int)coords.size(), num_positions);

    /* Select the first n coord values.  Since we just shuffled the
     * coords, this is, in effect, a random sampling without
     * replacement. */
    BOOST_FOREACH(int i, xrange(num_coords)) {
        coords[i] = grid_positions[i];
    }
}


void Circuit::print_kl_counts() {
    int min_kl = get_min_kl();
    int max_kl = get_max_kl();
    int net_count;

    BOOST_FOREACH(int kl, xrange(min_kl, max_kl + 1)) {
        net_count = kl_nets_count(kl);
        logger.notice(_("kl_nets_count(%d) = %d") % kl % net_count);
    }
}


}
