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
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace po = boost::program_options;


using namespace circuit;


typedef vector<fs::path> file_list;

struct Parameters {
    file_list benchmark_paths;
    fs::path output_path;
    string output_level;

    Parameters() :
        benchmark_paths(file_list()),
        output_path(fs::path()),
        output_level("") {
        validate();
    }
    Parameters(file_list benchmark_paths, fs::path output_path,
                string output_level) :
                benchmark_paths(benchmark_paths),
                output_path(output_path),
                output_level(output_level) {
        validate();
    }
    bool valid_output_path() {
        return output_path.string().length() > 0;
    }
    void validate() const {
        if(benchmark_paths.size() == 0) {
            throw po::validation_error(po::validation_error::invalid_option_value, "No benchmark file "
                    "specified.");
        } else {
            BOOST_FOREACH(fs::path const &benchmark_path, benchmark_paths) {
                if(!fs::exists(benchmark_path)) {
                    throw po::validation_error(po::validation_error::invalid_option_value,
                            (_("Benchmark file doesn't exist or cannot be read: %s")
                                % benchmark_path).str());
                }
            }
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
            ("benchmarks", po::value<file_list>()->multitoken(), "List of VPR benchmark path(s).");

    po::variables_map vm;
    po::positional_options_description posopts;
    posopts.add("benchmarks", -1);

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
            cout << _("Usage: %s [options] benchmark_path [benchmark_path]...") % av[0] << endl;
            cout << usage_options << "\n";
            exit(1);
        }
        file_list benchmarks;
        try {
            benchmarks = vm["benchmarks"].as<file_list>();
        } catch(boost::bad_any_cast& e) {
            benchmarks = file_list();
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

        return Parameters(benchmarks,
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

    if(params.output_path == "") {
        ar = new boost::archive::text_oarchive(cout);
        params.output_level = "ERROR";
    } else {
        ostream.open(params.output_path, std::ios_base::out);
        ar = new boost::archive::text_oarchive(ostream);
    }

    logger.set_priority(params.output_level);

    CircuitInfoManager info_manager;
    vector<Circuit> circuits(params.benchmark_paths.size());

    int i = 0;
    BOOST_FOREACH(fs::path &p, params.benchmark_paths) {
        Circuit &c = circuits[i++];
        logger.notice(_("Processing benchmark file: %s") % p);
        c.parse_file(p);
        info_manager.add_circuit(c);
        logger.notice("DONE");
    }

    logger.notice("Generating combined circuit profile...");
    CircuitProfile profile = info_manager.get_circuit_profile();
    logger.notice("DONE");

    *ar & profile;
    if(params.output_path != "") {
        ostream.close();
    }

    return 0;
}
