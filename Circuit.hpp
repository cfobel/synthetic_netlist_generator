/**
   @file Circuit.hpp
   @brief Header file containing definitions for Circuit, Net, and Block classes, etc.
  
   System:         Star+ CUDA (C++)
   Language: C++
  
   (c) Copyright Christian Fobel 2010
  
   Author: Christian Fobel
   E-Mail: cfobel@uoguelph.ca
  
   Compiler Options: -lboost_regex -lboost_filesystem

   Requires: Boost C++ Libraries >= 1.40.0 (http://www.boost.org/doc/)
*/  

#ifndef ___CIRCUIT_HPP___
#define ___CIRCUIT_HPP___

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <limits>
#include <map>
#include "VPRNetParser.hpp"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/regex.hpp>

/* Need random libraries for generating initial block positions */
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/format.hpp>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include "xrange.hpp"


#include <boost/detail/workaround.hpp>

#include <boost/range/sub_range.hpp>
#include <boost/range/as_literal.hpp> 


namespace fs = boost::filesystem;

namespace circuit {

using namespace std;
typedef map<int,double> PdfMap;

//! A convenience shortcut to the Boost.Format function
/*! @see http://www.boost.org/doc/libs/1_40_0/libs/format/doc/format.html */
#define _ boost::format

typedef string block_label;
enum block_type {CLB, INPUT, OUTPUT};


//! A list/vector structure to store a set of coordinate pairs.
typedef vector<boost::tuple<int, int> > coord_set;


/** @class Block
*
*
@brief This class represents an input, output or CLB from a VPR benchmark.
*
*/
class Block {
    block_type type;
    block_label label;
public:
    int x;
    int y;
    Block(block_label label, block_type type) : 
            label(label), type(type), x(0), y(0) {}
    Block(block_label label, block_type type, int x, int y) : 
            label(label), type(type), x(x), y(y) {}

    block_type get_type() const { return type; }
    block_label get_label() const { return label; }
    string toString() const;

};

//! A reference type for Block objects.
typedef boost::shared_ptr<Block> block_ptr;
//! A map data structure to store Block object references.
typedef map<block_label, block_ptr > block_map;
//! A map data structure to store Block object references tied to an integer ID.
typedef map<Block*, int> block_id_map;
//! A list/vector structure to store Block object references.
typedef std::vector<block_ptr> block_list;
//! A basic tuple to store the results from a parsed block/pins pair
//typedef boost::tuple<block_ptr, vector<string> > parse_result;
struct ParseResult {
    block_ptr block;
    vector<string> pins;
};



/** @class Net
*
*
@brief This class represents a net from a VPR benchmark.
*
*/
class Net {
    string label;
    block_list blocks;
public:
    Net(string label) : label(label) {}
    void add_block(block_ptr const &block);
    void add_blocks(block_list const &blocks);
    inline double wirelength(double alpha=1.0, double beta=1.59);
    inline double hpwl();
    string toString();
    int pin_count() const;
    string get_label() const { return label; }
    block_list const &get_blocks() { return blocks; }
};



//! A reference type for Net objects.
typedef boost::shared_ptr<Net> net_ptr;
//! A map data structure to store Net object references.
typedef map<string, net_ptr > net_map;
//! A vector data structure to store Net object references.
typedef vector<net_ptr> net_list;
//! A sub_range type to iterate over portions of a net_list object.
typedef boost::sub_range<net_list> net_range;
//! A reference type for net_list objects.
typedef net_list* net_list_ptr;
//! A map data structure to store net_list references.
typedef map<int, net_list> net_list_map;

//! Regular expression to match an input pin.
const boost::regex re_input("^\\.input\\s*([^\\s]*)");
//! Regular expression to match an output pin.
const boost::regex re_output("^\\.output\\s*([^\\s]*)");
//! Regular expression to match a clb.
const boost::regex re_clb("^\\.clb\\s*([^\\s]*)");
//! Regular expression to match a pin list.
const boost::regex re_pinlist("^pinlist:\\s*(.*?)\\s*$");

/** @class Circuit
*
*
@brief This class represents an entire circuit from a VPR benchmark.
*
*/
class Circuit {
    boost::mt19937* random_gen;
    
    //! Seed used when randomly initializing block positions
    int seed;
    //! Label for circuit
    string label;
    //! Label->Block map for inputs
    /*! @see Block */
    block_map inputs;
    //! Label->Block map for outputs
    /*! @see Block */
    block_map outputs;
    //! Label->Block map for CLBs
    /*! @see Block */
    block_map blocks;
    //! Label->Net map for all nets
    /*! @see Net */
    net_map nets;

    net_map input_nets;
    net_map output_nets;
    //! Cardinality (kl)->net_list map for all nets
    /*! @see Net, net_list */
    net_list_map nets_by_kl;
    //! Minimum cardinality of all nets
    int min_kl;
    //! Maximum cardinality of all nets
    int max_kl;
    void init_by_kl();
    void init_random() {
        random_gen = new boost::mt19937(seed);
    }
    void get_coord_set(coord_set &coords, int nx, int ny);
public:
    Circuit() : seed(0), min_kl(std::numeric_limits<int>::max()), max_kl(0) {
        init_random();
    }
    Circuit(int seed) : seed(seed), min_kl(std::numeric_limits<int>::max()), max_kl(0) {
        init_random();
    }
    void parse_file(fs::path input_file);
    void process_block(const string &block_label, block_type type, 
                        const vector<string> &pins);
    void add_block(block_ptr const &block);
    void add_net(net_ptr const &net);
    void init_block_positions();
    double wirelength(double &duration, double alpha=1.0, double beta=1.59);
    double hpwl(double &duration);

    int inputs_count() const { return inputs.size(); }
    int outputs_count() const { return outputs.size(); }
    int clbs_count() const { return blocks.size(); }
    int nets_count() const { return nets.size(); }
    int kl_nets_count(int kl) const { 
        if(nets_by_kl.count(kl)) {
            net_list_map::const_iterator iter = nets_by_kl.find(kl);
            return iter->second.size();
        }
        return 0;
    }

    string get_label() const { return label; }
    int get_min_kl() const { return min_kl; }
    int get_max_kl() const { return max_kl; }

    string toString() const;
    string dump_vpr_benchmark() const;
    string get_coords_list() const;
    net_list_map const &get_all_nets_by_kl() {
        return nets_by_kl;
    }
    void print_kl_counts();
    int get_nets_by_kl(int kl, net_list_ptr &list_ptr);
    void random_init(fs::path pdf_path, bool exact_values, int block_count=4096);
    void random_init(PdfMap const &net_counts, int block_count=4096);
};


boost::tuple<int, int> calc_fpga_dims(
            int num_clbs, int num_p_inputs, 
            int num_p_outputs, double aspect_ratio=1.0, 
            double io_rat=2.0);


ostream& operator<<(ostream& out, Net &obj);
ostream& operator<<(ostream& out, Block &obj);
ostream& operator<<(ostream& out, Circuit &obj);

}

#define BLOCK_INPUT     5
#define BLOCK_OUTPUT    10
#define BLOCK_CLB       15

#endif
