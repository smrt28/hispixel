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

    if (s == "focus") {
        Action_t rv(Action_t::ACTION_FOCUS);
        rv.data = boost::lexical_cast<int>(word(p).str());
        return rv;
    }
    if (s == "focus_next") {
        return Action_t(Action_t::ACTION_FOCUS_NEXT);
    }
    if (s == "focus_prev") {
        return Action_t(Action_t::ACTION_FOCUS_PREV);
    }
    if (s == "opentab") {
        return Action_t(Action_t::ACTION_OPENTAB);
    }
    if (s == "toggle_tabbar") {
        return Action_t(Action_t::ACTION_TOGGLE_TABBAR);
    }

    RAISE(UNKNOWN_ACTION) << "probably unknown action: " << s;
    return TConfig_t::Action_t();
}

}

int TConfig_t::parse_config_line(const std::string &line) {
    parser::Parslet_t p(line);
    parser::ltrim(p);

    parser::Parslet_t pp = p;

    if (!p) return 0;
    if (p[0] == '#') return 0;

    std::string aword = parser::word(p).str();
    if (aword == "bindsym") {
        KeySym_t ks = parser::keysym(p);
        TConfig_t::Action_t action = parser::action(p);
        keybindings.push_back(KeyBinding_t(ks, action));
        return 1;
    }

    p = pp;
    auto res = parser::eq(p);
    try {
        auto it = kv.find(res.first);
        if (it == kv.end()) {
            RAISE(CFG_PARSE) << "unknown config key: " << res.first;
        }
        it->second->set(res.second);
        return 1;
    } catch(...) {
        RAISE(CFG_PARSE) << "invalid value for: " << res.first;
    }

    return 0;
}

bool TConfig_t::init(const std::string &file) {
    std::ifstream f(file);
    if (!f) return false;
    std::string line;
    int n = 0;
    while (std::getline(f, line)) {
        ++n;
        try {
            parse_config_line(line);
        } catch(...) {
            RAISE(CFG_PARSE) << "config parse error at line: " << n;
        }
    }
    return true;
}

void TConfig_t::init_defaults() {
    insert_default<std::string>("term_font", "Terminus");
    insert_default<int>("term_font_size", "12");
    insert_default<bool>("allow_bold", "true");
    insert_default<bool>("show_tabbar", "true");
    insert_default<GdkRGBA>("tabbar_bg_color","#303030");
    insert_default<bool>("tabbar_on_bottom", "false");
}

}
