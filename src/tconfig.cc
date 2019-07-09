#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iostream>
#include "tconfig.h"
#include "parslet.h"
#include "keyevent.h"
#include "valuecast.h"

namespace s28 {

namespace parser {

// parse "alt+ctrl+z" key description
KeySym_t keysym(parser::Parslet_t &p) {
    ltrim(p);
    Parslet_t rv = p;
    for (;;) {
        parser::word(p);
        rv = Parslet_t(rv.begin(), p.begin());
        ltrim(p);
        if (p[0] == '+') {
            p.skip();
            continue;
        }
        return parse_key_sym(rv.str());
    }
}

TConfig_t::Action_t action(parser::Parslet_t &p) {
    typedef TConfig_t::Action_t Action_t;
    std::string s = word(p).str();

    // handle actions for keybindings
    if (s == "focus") { // focus tab
        Action_t rv(Action_t::ACTION_FOCUS);
        // stor tab nuber in data
        rv.data = boost::lexical_cast<int>(word(p).str());
        return rv;
    }
    if (s == "focus_next") { // focus next tab
        return Action_t(Action_t::ACTION_FOCUS_NEXT);
    }
    if (s == "focus_prev") { // focus prev tab
        return Action_t(Action_t::ACTION_FOCUS_PREV);
    }
    if (s == "opentab") { // open new tab
        return Action_t(Action_t::ACTION_OPENTAB);
    }
    if (s == "close_last") { // exits the app when there is no tab open
        return Action_t(Action_t::ACTION_CLOSE_LAST);
    }

    RAISE(UNKNOWN_ACTION) << "unknown config key: " << s;
    return TConfig_t::Action_t(); // not reachable (avoids compiler warning)
}

} // namespace parser

int TConfig_t::parse_config_line(const std::string &line) {
    parser::Parslet_t p(line);
    parser::ltrim(p);

    parser::Parslet_t pp = p;

    // handle empty lines and comments
    if (!p) return 0;
    if (p[0] == '#') return 0;

    // handle bindsym first
    std::string aword = parser::word(p).str();
    if (aword == "bindsym") {
        KeySym_t ks = parser::keysym(p);
        TConfig_t::Action_t action = parser::action(p);
        if (action.type == Action_t::ACTION_CLOSE_LAST)
            has_close_last = true;
        keybindings.push_back(KeyBinding_t(ks, action));
        return 1;
    }

    p = pp;
    // parse "key = value" style config lines
    auto res = parser::eq(p);

    auto it = kv.find(res.first);

    // known keys are defined in init_defaults()
    if (it == kv.end()) {
        RAISE(CONFIG_SYNTAX) << "unknown key: " << res.first;
    }

    try {
        // set() checks the value format by ValueCast_t::cast
        it->second->set(res.second);
        return 1;
    } catch(...) {
        RAISE(CONFIG_SYNTAX) << "invalid value for key: " << res.first;
    }

    return 0;
}

bool TConfig_t::init(const std::string &file) {
    static const std::string errmsg_prefix = "config file error at line: ";
    std::ifstream f(file);
    if (!f) return false; // return false if can't open config file
    std::string line;
    int n = 1;
    // iterate config file lines
    while (std::getline(f, line)) {
        try {
            parse_config_line(line);
        } catch(const std::exception &e) {
            RAISE(CONFIG_SYNTAX) << errmsg_prefix << n << "; [" << e.what() << "]";
        } catch(...) {
            RAISE(CONFIG_SYNTAX) << errmsg_prefix << n;
        }
        ++n; // line counter
    }
    return true;
}

bool TConfig_t::init(const std::vector<std::string> &files) {
    init_defaults();

    // iterate all the file names and use the first which opens for reading
    for (std::string cfile: files) {
        if (init(cfile)) return true;
    }
    return false;
}

void TConfig_t::init_defaults() {
    // hardcoded default config values
    insert_default<std::string>("term_font", "Terminus");
    insert_default<int>("term_font_size", "12");
    insert_default<bool>("allow_bold", "true");
    insert_default<bool>("show_tabbar", "true");
    insert_default<GdkRGBA>("tabbar_bg_color","#303030");
    insert_default<bool>("tabbar_on_bottom", "false");
}

} // namespace s28
