#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

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

    if (p && p[0] == '#') return 0;

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


void TConfig_t::init_defaults() {

    insert_default<std::string>("term_font", "Terminus 8");
    insert_default<bool>("allow_bold", "1");



    parse_config_line("bindsym alt+1 focus 1");
    parse_config_line("bindsym alt+2 focus 2");
    parse_config_line("bindsym alt+3 focus 3");
    parse_config_line("bindsym alt+4 focus 4");
    parse_config_line("bindsym alt+5 focus 5");
    parse_config_line("bindsym alt+6 focus 6");
    parse_config_line("bindsym alt+7 focus 7");
    parse_config_line("bindsym alt+8 focus 8");
    parse_config_line("bindsym alt+9 focus 9");
    parse_config_line("bindsym alt+10 focus 10");
    parse_config_line("bindsym alt+ctrl+z opentab");
    parse_config_line("set term_font = Terminus 8");
    parse_config_line("set allow_bold = true");
}


TConfig_t::Action_t TConfig_t::find_action(GdkEvent *event) {
    for (const KeyBinding_t &kb: keybindings) {
        if (match_gtk_event(event, kb.keysym)) {
            return kb.action;
        }
    }
    return Action_t();
}


}
