/**
   @file CalcStarPlus.cpp
   @brief This program parses a circuit in VPR benchmark format into a Circuit object.

   System:         Star+ CUDA (C++)
   Language: C++

   (c) Copyright Christian Fobel 2010

   Author: Christian Fobel
   E-Mail: cfobel@uoguelph.ca

   Compiler Options: -lboost_regex -lboost_filesystem

   Requires: Boost C++ Libraries >= 1.40.0 (http://www.boost.org/doc/)
*/

#include "Circuit.hpp"
#include "Logging.hpp"

#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string/join.hpp>
namespace po = boost::program_options;


using namespace circuit;


struct Parameters {
    fs::path profile_path;
    fs::path output_path;
    string output_level;
    int block_count;
    bool exact_values;

    bool random() const {
        return profile_path != "";
    }

    Parameters() : output_level(""), profile_path(fs::path()),
                    output_path(output_path),
                    block_count(4096), exact_values(true) {
        validate();
    }
    Parameters(fs::path profile_path, fs::path output_path,
                string output_level,
                int block_count, bool exact_values) :
                output_path(output_path),
                output_level(output_level), profile_path(profile_path),
                block_count(block_count), exact_values(exact_values) {
        validate();
    }
    void validate() const {
        if(profile_path != "" && !fs::exists(profile_path)) {
            throw po::validation_error(po::validation_error::invalid_option_value,
                    (_("Circuit profile doesn't exist or cannot be read: %s")
                        % profile_path).str());
        }
        if(output_path == "") {
            throw po::validation_error(po::validation_error::invalid_option_value,
                    (_("Invalid output path: %s")
                        % output_path).str());
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
        ("output-level,v", po::value<string>()->default_value("NOTICE"),
            "Output level (DEBUG, INFO, NOTICE, WARN, ERROR)")
        ("output-path,o", po::value<fs::path>(), "Output path")
        ("block-count,b", po::value<int>()->default_value(4096),
            "Number of blocks to add to random circuit.")
        ("exact-values,e",
            "Values from profile should be treated as exact net counts.  "
            "Otherwise, the values will be treated as probabilities.")
    ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
            ("profile", po::value<fs::path>()->default_value(""),
                "Circuit profile path");

    po::variables_map vm;
    po::positional_options_description posopts;
    posopts.add("profile", 1);

    po::options_description usage_options;
    usage_options.add(generic).add(config);

    po::options_description cmdline_options = usage_options;
    cmdline_options.add(hidden);

    try {
        po::store(po::command_line_parser(ac, av)
                .options(cmdline_options)
                .positional(posopts).run(), vm);

        // the call to the notify function run_count the user-specified
        // notify functions and stores the values into regular
        // variables, if needed.
        po::notify(vm);

        if (vm.count("help")) {
            cout << _("Usage: %s [options] profile_path") % av[0] << endl;
            cout << usage_options << "\n";
            exit(1);
        }

        fs::path output_path;
        if(vm.count("output-path") == 0) {
            output_path = fs::path();
        } else {
            output_path = vm["output-path"].as<fs::path>();
        }

        return Parameters(vm["profile"].as<fs::path>(),
                            output_path,
                            vm["output-level"].as<string>(),
                            vm["block-count"].as<int>(),
                            vm.count("exact-values"));
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
    Circuit c;

    Parameters params = parse_args(argc, argv);

    fs::path p = params.profile_path;
    logger.set_priority(params.output_level);
    logger.notice(_("Logging level: %s (%d)")
            % params.output_level
            % logger.get_level_num(params.output_level));

    c.random_init(params.profile_path, params.exact_values,
                    params.block_count);
    logger.notice(_("Generating random circuit from file: %s")
            % params.profile_path);
    if(params.exact_values) {
        logger.notice("  \\-> interpreting as exact net counts");
    } else {
        logger.notice("  \\-> interpreting as net count probabilities");
    }

    fs::ofstream ostream;
    ostream.open(params.output_path, std::ios_base::out);

    ostream << c.dump_vpr_benchmark();

    ostream.close();

    return 0;
}
