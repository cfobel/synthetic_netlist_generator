/**
   @file CircuitProfileInfo.cpp
   @brief This program parses a circuit profile and displays info about it.

   System:         Star+ CUDA (C++)
   Language: C++

   (c) Copyright Christian Fobel 2011

   Author: Christian Fobel
   E-Mail: cfobel@uoguelph.ca

   Compiler Options: -lboost_regex -lboost_filesystem

   Requires: Boost C++ Libraries >= 1.40.0 (http://www.boost.org/doc/)
*/

#include <boost/algorithm/string/join.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/program_options.hpp>

#include "CircuitInfo.hpp"
#include "Logging.hpp"

namespace po = boost::program_options;

using namespace circuit;


struct Parameters {
    fs::path profile_path;
    string output_level;
    int block_count;
    bool exact_values;

    bool random() const {
        return profile_path != "";
    }

    Parameters() : output_level(""), profile_path(fs::path()) {
        validate();
    }
    Parameters(fs::path profile_path,
                string output_level) :
                output_level(output_level), profile_path(profile_path) {
        validate();
    }
    void validate() const {
        if(profile_path == "") {
            throw po::validation_error(po::validation_error::invalid_option_value, "Must specify a Circuit profile path!");
        } else if(!fs::exists(profile_path)) {
            throw po::validation_error(po::validation_error::invalid_option_value,
                    (_("Circuit profile doesn't exist or cannot be read: %s")
                        % profile_path).str());
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

        return Parameters(vm["profile"].as<fs::path>(),
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

    fs::path p = params.profile_path;
    logger.set_priority(params.output_level);
    logger.notice(_("Logging level: %s (%d)")
            % params.output_level
            % logger.get_level_num(params.output_level));

    fs::ifstream istream;
    istream.open(params.profile_path, std::ios_base::in);
    CircuitProfile profile;
    boost::archive::text_iarchive ar(istream);
    ar & profile;
    istream.close();

    PdfMap const &pdf = profile.get_pdf_map();
    vector<string> cardinalities;
    vector<string> percentages;

    BOOST_FOREACH(PdfMap::value_type const &item, pdf) {
        logger.notice(_("%d: %.4f") % item.first % item.second);
        cardinalities.push_back((_("%d") % item.first).str());
        percentages.push_back((_("%.4f") % item.second).str());
    }

    logger.notice(_("Cardinalities:\t[%s]") % boost::algorithm::join(cardinalities, ","));
    logger.notice(_("Percentages:\t[%s]") % boost::algorithm::join(percentages, ","));

    return 0;
}
