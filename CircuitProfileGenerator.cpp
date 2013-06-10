/**
   @file CircuitProfiler.cpp
   @brief This program parses one or more VPR benchmark files and generates a cardinality->probability mapping.

   System:         Star+ CUDA (C++)
   Language: C++

   (c) Copyright Christian Fobel 2010

   Author: Christian Fobel
   E-Mail: cfobel@uoguelph.ca

   Compiler Options: -lboost_regex -lboost_filesystem

   Requires: Boost C++ Libraries >= 1.40.0 (http://www.boost.org/doc/)
*/

#include "Circuit.hpp"
#include "CircuitInfo.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/regex.hpp>

namespace po = boost::program_options;


using namespace circuit;


typedef vector<fs::path> file_list;

const boost::regex re_kl_pdf_list("^(\\d+:\\d+(\\.\\d+)?)(,(\\d+:\\d+(\\.\\d+)?))*");

struct Parameters {
    PdfMap pdf;
    string pdf_list;
    fs::path output_path;
    string output_level;

    Parameters() :
        pdf(PdfMap()),
        output_path(fs::path()),
        output_level("") {
        validate();
    }
    Parameters(string pdf_list, fs::path output_path,
                string output_level) :
                pdf_list(pdf_list),
                output_path(output_path),
                output_level(output_level) {
        validate();
        boost::regex re_comma_sep(",");
        boost::regex re_colon_sep(":");
        boost::sregex_token_iterator j;
        boost::sregex_token_iterator i(pdf_list.begin(), pdf_list.end(),
                                            re_comma_sep, -1);

        BOOST_FOREACH(string match, boost::make_iterator_range(i, j)) {
            boost::sregex_token_iterator k(match.begin(), match.end(),
                                            re_colon_sep, -1);
            int kl;
            double p;
            istringstream(*k++) >> kl;
            istringstream(*k) >> p;
            pdf[kl] = p;
        }
    }
    bool valid_output_path() {
        return output_path.string().length() > 0;
    }
    void validate() const {
        boost::cmatch matches;
        bool match = boost::regex_match(pdf_list.c_str(), matches,
                                    re_kl_pdf_list);
        if(!match) {
            throw po::validation_error(po::validation_error::invalid_option_value,
                    (_("Invalid kl PDF list: %s") % pdf_list).str());
        }
    }
};

Parameters parse_args(int ac, char** av) {
    po::options_description generic("Generic options");
    generic.add_options()
        ("help,h", "produce help message")
    ;

    po::options_description config("Configuration options");
    config.add_options()
        ("output-path,o", po::value<fs::path>(), "Output path (default: stdout)")
        ("output-level,v", po::value<string>()->default_value("NOTICE"),
            "Output level (DEBUG, INFO, NOTICE, WARN, ERROR)")
    ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
            ("pdf_list", po::value<string>(), "List of kl probabilities, in format: \"kl:prob,kl:prob,...\".");

    po::variables_map vm;
    po::positional_options_description posopts;
    posopts.add("pdf_list", 1);

    po::options_description usage_options;
    usage_options.add(generic).add(config);

    po::options_description cmdline_options = usage_options;
    cmdline_options.add(hidden);

    try {
        po::store(po::command_line_parser(ac, av)
                .options(cmdline_options)
                .positional(posopts).run(), vm);

        // the call to the notify function runs the user-specified
        // notify functions and stores the values into regular
        // variables, if needed.
        po::notify(vm);

        if (vm.count("help")) {
            cout << _("Usage: %s [options] kl:prob,kl:prob,...") % av[0] << endl;
            cout << usage_options << "\n";
            exit(1);
        }

        fs::path output_path;
        if(vm.count("output-path") == 0) {
            if(vm.count("output-level") && !vm["output-level"].defaulted()) {
                throw logic_error(
                        "Cannot set output level when serializing to stdout.");
            }
            output_path = fs::path();
        } else {
            output_path = vm["output-path"].as<fs::path>();
        }

        return Parameters(vm["pdf_list"].as<string>(),
                output_path,
                vm["output-level"].as<string>());
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        cout << usage_options << "\n";
        exit(-1);
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
        exit(-1);
    }
    return Parameters();
}


Logger logger("NOTICE");

int main(int argc, char* argv[]) {
    Parameters params = parse_args(argc, argv);

    fs::ofstream ostream;
    boost::archive::text_oarchive* ar;

    logger.set_priority(params.output_level);

    if(params.output_path == "") {
        ar = new boost::archive::text_oarchive(cout);
        params.output_level = "ERROR";
    } else {
        ostream.open(params.output_path, std::ios_base::out);
        ar = new boost::archive::text_oarchive(ostream);
    }

    CircuitProfile profile(params.pdf, 1.0);

    *ar & profile;
    if(params.output_path != "") {
        ostream.close();
    }

    return 0;
}
