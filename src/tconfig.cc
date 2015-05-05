#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>

#include "tconfig.h"
#include "parser.h"
#include "keyevent.h"
#include "valuecast.h"

namespace s28 {

namespace parser {

KeySym_t keysym(parser::Parser_t &p) {
    ltrim(p);
    Parser_t rv = p;
    for (;;) {
        parser::word(p);
        rv.eit = p.it;
        ltrim(p);
        if (p[0] == '+') {
            p.skip();
            continue;
        }
        return parse_key_sym(rv.str());
    }
}

TConfig_t::Action_t action(parser::Parser_t &p) {
    std::string s = word(p).str();
    if (s == "focus") {
        TConfig_t::Action_t rv;
        rv.type = TConfig_t::Action_t::ACTION_FOCUS;
        rv.data = boost::lexical_cast<int>(word(p).str());
        return rv;
    }
    if (s == "opentab") {
        TConfig_t::Action_t rv;
        rv.type = TConfig_t::Action_t::ACTION_OPENTAB;
        return rv;
    }

    RAISE(UNKNOWN_ACTION) << "probably unknown action: " << s;
    return TConfig_t::Action_t();
}

}

int TConfig_t::parse_config_line(const std::string &line) {
    parser::Parser_t p(line);
    parser::ltrim(p);

    if (!p) return 0;
    if (p[0] == '#') return 0;

    std::string aword = parser::word(p).str();
    if (aword == "bindsym") {
        KeySym_t ks = parser::keysym(p);
        TConfig_t::Action_t action = parser::action(p);
        keybindings.push_back(KeyBinding_t(ks, action));
        return 1;
    }
    if (aword == "set") {
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
}


TConfig_t::Action_t TConfig_t::find_action(GdkEvent *event) const {
    for (const KeyBinding_t &kb: keybindings) {
        if (match_gtk_event(event, kb.keysym)) {
            return kb.action;
        }
    }
    return Action_t();
}

}
