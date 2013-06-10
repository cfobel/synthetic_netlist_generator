/**
   @file CircuitInfo.hpp
   @brief Header file containing definitions for CircuitInfo class
  
   System:         Star+ CUDA (C++)
   Language: C++
  
   (c) Copyright Christian Fobel 2010
  
   Author: Christian Fobel
   E-Mail: cfobel@uoguelph.ca
  
   Compiler Options: -lboost_regex -lboost_filesystem

   Requires: Boost C++ Libraries >= 1.40.0 (http://www.boost.org/doc/)
*/  

#ifndef ___CIRCUIT_INFO_HPP___
#define ___CIRCUIT_INFO_HPP___

#include <map>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/serialization/map.hpp> 
#include <boost/algorithm/string/join.hpp>
#include "Circuit.hpp"
#include "Logging.hpp"

extern Logger logger;

namespace circuit {

//! A convenience shortcut to the Boost.Format function
/*! @see http://www.boost.org/doc/libs/1_40_0/libs/format/doc/format.html */
#define _ boost::format

using namespace std;
typedef map<int,int> int_map;

class CircuitInfo {
    Circuit &circuit;
    bool initialized;
    PdfMap pdf_map;
    void prep_data() {
        net_list_map const &nets_by_kl = circuit.get_all_nets_by_kl();
        double nets_count = circuit.nets_count();
        
        BOOST_FOREACH(net_list_map::value_type const &item, nets_by_kl) {
            pdf_map[item.first] = item.second.size() / nets_count;
        }
    }
public:
    CircuitInfo(Circuit &c) 
        : circuit(c), initialized(false) {}
    PdfMap const &get_data() {
        if(!initialized) {
            prep_data();
            initialized = true;
        }
        return pdf_map;
    }
    int get_max_fanout() const {
        return circuit.get_max_kl();
    }
    int clbs_count() const { return circuit.clbs_count(); }
    int nets_count() const { return circuit.nets_count(); }
    string get_label() const { return circuit.get_label(); }
    net_list_map const &get_all_nets_by_kl() { return circuit.get_all_nets_by_kl(); }
    string toString() const {
        vector<string> msg;
        vector<string> msg2;

        net_list_map const &nets_by_kl = circuit.get_all_nets_by_kl();
        msg.push_back(circuit.toString());
        msg.push_back((_("  Min kl: %d") % circuit.get_min_kl()).str()); 
        msg.push_back((_("  Max kl: %d") % circuit.get_max_kl()).str()); 
        msg.push_back("  [kl]:   net count  (first 5 nets)"); 
        BOOST_FOREACH(net_list_map::value_type const &item, nets_by_kl) {
            int net_count = item.second.size();
            msg2.clear();

            for(int i = 0; i < net_count && i < 5; i++) {
                msg2.push_back(item.second[i]->get_label());
            }
            string kl_summary = (_("    [%d]: %6d (%s") 
                        % item.first % item.second.size()
                        % boost::algorithm::join(msg2, ", ")).str();
            if(net_count > 1) {
                msg.push_back(kl_summary + ",...)");
            } else {
                msg.push_back(kl_summary + ")");
            }
        }
        return boost::algorithm::join(msg, "\n");
    }
};

class CircuitProfile {
private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & pdf_map;
        ar & net_to_block_ratio;
    }
    PdfMap pdf_map;
    double net_to_block_ratio;
public:
    CircuitProfile(){};
    CircuitProfile(PdfMap pdf_map, double net_to_block_ratio) :
        pdf_map(pdf_map), net_to_block_ratio(net_to_block_ratio)
    {}
    PdfMap const &get_pdf_map() { return pdf_map; }
    double get_net_to_block_ratio() { return net_to_block_ratio; }
};

class CircuitInfoManager {
    boost::ptr_vector<CircuitInfo> circuits;
public:
    void add_circuit(Circuit &c) {
        circuits.push_back(new CircuitInfo(c));
    }

    CircuitProfile get_circuit_profile() {
        PdfMap combined_pdf = get_combined_pdf();
        
        double total_net_count = 0;
        double total_clbs_count = 0;
        BOOST_FOREACH(CircuitInfo &info, circuits) {
            total_net_count += info.nets_count();
            total_clbs_count += info.clbs_count();
        }
        return CircuitProfile(combined_pdf, 
                    total_net_count / total_clbs_count);
    }

    PdfMap get_combined_pdf() {
        logger.notice("start: get_combined_pdf()");
        int max_fanout = 0;
        PdfMap combined;

        BOOST_FOREACH(CircuitInfo &info, circuits) {
            if(info.get_max_fanout() > max_fanout) {
                max_fanout = info.get_max_fanout();
            }
        }
        logger.notice(_("  max_fanout: %d") % max_fanout);

        BOOST_FOREACH(int i, xrange(max_fanout + 1)) {
            double total = 0;
            logger.notice(_("  processing kl: %d") % i);
            BOOST_FOREACH(CircuitInfo &info, circuits) {
                PdfMap const &pdf = info.get_data();
                if(pdf.count(i) > 0) {
                    total += pdf.find(i)->second;
                }
            }
            if(total > 0) {
                logger.notice(_("  setting combined [%d]: %d") % i % total);
                combined[i] = total;
            }
        }

        double combined_total = 0;
        BOOST_FOREACH(PdfMap::value_type &item, combined) {
            combined_total += item.second;
        }
        BOOST_FOREACH(PdfMap::value_type &item, combined) {
            item.second /= combined_total;
        }

        /*
        BOOST_FOREACH(PdfMap::value_type &item, combined) {
            cout << _("[%d]: %.5f") % item.first % item.second << endl;
        }
        */
        logger.notice("DONE: get_combined_pdf()");
        return combined;
    }

    string get_info() {
        vector<string> msg;
        vector<string> msg2;

        BOOST_FOREACH(CircuitInfo &info, circuits) {
            msg.push_back(info.toString());
            msg.push_back("\n");
        }
        return boost::algorithm::join(msg, "\n");
    }
};


}
#endif
