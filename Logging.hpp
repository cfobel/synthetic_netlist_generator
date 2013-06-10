#ifndef ___LOGGING_HPP___
#define ___LOGGING_HPP___

#include <iostream>
#include <boost/format.hpp>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>

using namespace std;

/*! Wrapper class around log4cpp to allow logging strings or boost 
 * format objects.
 * @see http://www.boost.org/doc/libs/1_44_0/libs/format/index.html */
class Logger {
    log4cpp::Appender *appender;
    log4cpp::PatternLayout *layout;

    void initialize(const string &level) {
        appender = new log4cpp::OstreamAppender("OstreamAppender", &cout);
        //layout = new log4cpp::SimpleLayout();
        layout = new log4cpp::PatternLayout();

        appender->setLayout(layout);
        category.setAppender(appender);
        set_priority(level);
    }

public:
    log4cpp::Category& category;
    Logger() : category(log4cpp::Category::getInstance("Category")) {
        // Set default level to NOTICE
        initialize("NOTICE");
    }
    Logger(const string &level) : category(log4cpp::Category::getInstance("Category")) {
        initialize(level);
    }
    ~Logger() {
        category.removeAllAppenders();
    }

    void emerg(const string &msg) { category.emerg(msg); }
    void fatal(const string &msg) { category.fatal(msg); }
    void alert(const string &msg) { category.alert(msg); }
    void crit(const string &msg) { category.crit(msg); }
    void error(const string &msg) { category.error(msg); }
    void warn(const string &msg) { category.warn(msg); }
    void notice(const string &msg) { category.notice(msg); }
    void info(const string &msg) { category.info(msg); }
    void debug(const string &msg) { category.debug(msg); }

    /* Alternative definitions to accept boost format objects. */
    void emerg(const boost::format &msg) { category.emerg(msg.str()); }
    void fatal(const boost::format &msg) { category.fatal(msg.str()); }
    void alert(const boost::format &msg) { category.alert(msg.str()); }
    void crit(const boost::format &msg) { category.crit(msg.str()); }
    void error(const boost::format &msg) { category.error(msg.str()); }
    void warn(const boost::format &msg) { category.warn(msg.str()); }
    void notice(const boost::format &msg) { category.notice(msg.str()); }
    void info(const boost::format &msg) { category.info(msg.str()); }
    void debug(const boost::format &msg) { category.debug(msg.str()); }

    const int get_level_num(string level) { 
        return log4cpp::Priority::getPriorityValue(level);
    }
    const string &get_priority() { 
        return log4cpp::Priority::getPriorityName(category.getPriority());
    }
    void set_priority(const string &level) { 
        category.setPriority(log4cpp::Priority::getPriorityValue(level));
    }
};

#endif
