#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <set>
#include <iostream>


#include "config.hxx"
#include "parslet.h"
#include "valuecast.h"

namespace s28 {


namespace {

#define DEF_MASK(m, mm)   if (s == m) { mask |= mm; continue; }

void tolower(std::string &data) {
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
}

/**
 * parse key-sym description like "ctrl+alt+1"
 * @return KeySym_t structure
 */
KeySym_t parse_key_sym(const std::string &descr) {
    KeySym_t rv;
    rv.text = descr;
    std::vector<std::string> v;

    boost::split(v, descr, boost::is_any_of("+"));

    guint mask = 0;
    uint32_t key = 0;

    std::set<std::string> check;

    for (std::string s: v) {
        boost::trim(s);
        if (s.empty()) {
            RAISE(PARSER) << "invalid key-sym spec";
        }

        // handle single char
        if (s.size() == 1) {
            if (key != 0) {
                RAISE(PARSER) << "invalid key-sym spec";
            }
            key = s[0];
            continue;
        }

        tolower(s);

        // hadle keyword entered multiple times, like "alt+alt"
        if (check.count(s)) {
            RAISE(PARSER) << "invalid key-sym spec";
        }

        check.insert(s);

        // handle win-key etc...
        if (s[0] == 'm') {
            DEF_MASK("mod1", GDK_MOD1_MASK);
            DEF_MASK("mod2", GDK_MOD2_MASK);
            DEF_MASK("mod3", GDK_MOD3_MASK);
            DEF_MASK("mod4", GDK_MOD4_MASK);
            DEF_MASK("mod5", GDK_MOD5_MASK);
        }

        DEF_MASK("alt", GDK_MOD1_MASK);
        DEF_MASK("ctrl", GDK_CONTROL_MASK);
        DEF_MASK("shift", GDK_SHIFT_MASK);

        // f* keys
        if (s[0] == 'f') {
            if (s == "f1") { key = GDK_KEY_F1; continue; }
            if (s == "f2") { key = GDK_KEY_F2; continue; }
            if (s == "f3") { key = GDK_KEY_F3; continue; }
            if (s == "f4") { key = GDK_KEY_F4; continue; }
            if (s == "f5") { key = GDK_KEY_F5; continue; }
            if (s == "f6") { key = GDK_KEY_F6; continue; }
            if (s == "f7") { key = GDK_KEY_F7; continue; }
            if (s == "f8") { key = GDK_KEY_F8; continue; }
            if (s == "f9") { key = GDK_KEY_F9; continue; }
            if (s == "f10") { key = GDK_KEY_F10; continue; }
            if (s == "f11") { key = GDK_KEY_F11; continue; }
            if (s == "f12") { key = GDK_KEY_F12; continue; }
        }

        // arrows
        if (s == "left") { key = GDK_KEY_Left; continue; }
        if (s == "right") { key = GDK_KEY_Right; continue; }
        if (s == "up") { key = GDK_KEY_Up; continue; }
        if (s == "down") { key = GDK_KEY_Down; continue; }
        if (s == "esc") { key = GDK_KEY_Escape; continue; }

        // other spec. keys
        if (s == "pause") { key = GDK_KEY_Pause; continue; }
        if (s == "escape") { key = GDK_KEY_Escape; continue; }
        if (s == "scrolllock") { key = GDK_KEY_Scroll_Lock; continue; }
        if (s == "delete") { key = GDK_KEY_Delete; continue; }
        if (s == "home") { key = GDK_KEY_Home; continue; }
        if (s == "end") { key = GDK_KEY_End; continue; }
        if (s == "pageup") { key = GDK_KEY_Page_Up; continue; }
        if (s == "pagedown") { key = GDK_KEY_Page_Down; continue; }

        RAISE(PARSER) << "unknown key-sym keyword: \"" << s << "\"";
    }
    rv.key = key;
    rv.mask = mask;

    return rv;
}


/**
 * parse "alt+ctrl+z" key description
 * @param p parslet
 */
KeySym_t keysym(parser::Parslet_t &p) {
    ltrim(p);
    parser::Parslet_t key_descr = p;
    for (;;) {
        parser::word(p);
        key_descr = parser::Parslet_t(key_descr.begin(), p.begin());
        ltrim(p);
        if (p[0] == '+') {
            p.skip();
            continue;
        }
        KeySym_t res = parse_key_sym(key_descr.str());

//        std::cout << res.text << " mask=" << res.mask << " key="
//            << res.key << std::endl;
        return res;
    }
}


/**
 * Convert config file key-binding keyword to the action
 * @return action
 */
Config_t::Action_t string_to_action(parser::Parslet_t &p) {
    typedef Config_t::Action_t Action_t;
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
    if (s == "togle_tabbar") {
        return Action_t(Action_t::ACTION_TOGLE_TABBAR);
    }
    if (s == "swap_1") { return Action_t(Action_t::ACTION_BE_FIRST, 0); }
    if (s == "swap_2") { return Action_t(Action_t::ACTION_BE_FIRST, 1); }
    if (s == "swap_3") { return Action_t(Action_t::ACTION_BE_FIRST, 2); }
    if (s == "swap_4") { return Action_t(Action_t::ACTION_BE_FIRST, 3); }
    if (s == "swap_5") { return Action_t(Action_t::ACTION_BE_FIRST, 4); }
    if (s == "swap_6") { return Action_t(Action_t::ACTION_BE_FIRST, 5); }
    if (s == "swap_7") { return Action_t(Action_t::ACTION_BE_FIRST, 6); }
    if (s == "swap_8") { return Action_t(Action_t::ACTION_BE_FIRST, 7); }
    if (s == "swap_9") { return Action_t(Action_t::ACTION_BE_FIRST, 8); }
    if (s == "swap_10") { return Action_t(Action_t::ACTION_BE_FIRST, 9); }
    if (s == "swap_11") { return Action_t(Action_t::ACTION_BE_FIRST, 10); }
    if (s == "swap_12") { return Action_t(Action_t::ACTION_BE_FIRST, 11); }

    RAISE(UNKNOWN_ACTION) << "unknown config key: " << s;
    return Config_t::Action_t(); // not reachable (avoids compiler warning)
}


} // namespce

int Config_t::parse_config_line(const std::string &line) {
    parser::Parslet_t p(line);
    parser::ltrim(p);

    parser::Parslet_t pp = p;

    // handle empty lines and comments
    if (!p) return 0;
    if (p[0] == '#') return 0;

    // handle bindsym first
    std::string aword = parser::word(p).str();
    if (aword == "bindsym") {
        KeySym_t ks = keysym(p);



        Config_t::Action_t action = string_to_action(p);
        if (action.type == Action_t::ACTION_CLOSE_LAST)
            has_close_last = true;
        keybindings.push_back(KeyBinding_t(ks, action));
        return 1;
    }

    p = pp;
    // parse "key = value" style config lines
    auto res = parser::eq(p);

    auto val = config_map.find(res.first);

    // known keys are defined in init_defaults()
    if (!val) {
        RAISE(CONFIG) << "unknown key: " << res.first;
    }

    try {
        // set() checks the value format by ValueCast_t::cast
        val->set(res.second);
        return 1;
    } catch(...) {
        RAISE(CONFIG) << "invalid value for key: " << res.first;
    }

    return 0;
}

bool Config_t::init(const std::string &file) {
    static const std::string errmsg_prefix = "config file error at line: ";
    std::ifstream f(file);
    if (!f) return false; // return false if can't open config file
    std::string line;
    int n = 0;
    // iterate config file lines
    while (std::getline(f, line)) {
        ++n; // line counter
        try {
            parse_config_line(line);
        } catch(const std::exception &e) {
            RAISE(CONFIG) << errmsg_prefix << n << "; [" << e.what() << "]";
        } catch(...) {
            RAISE(CONFIG) << errmsg_prefix << n;
        }
    }
    if (n == 0) return false;
    return true;
}

bool Config_t::init(const std::vector<std::string> &files) {
    // initialize default values
    init_defaults();

    // iterate all the file names and use the first which opens for reading
    for (std::string cfile: files) {
        if (init(cfile)) return true;
    }
    return false;
}

namespace {

struct ConfigDumpator_t {
        template<typename Dummy>
        void set(std::string key, std::string val) {
                std::cout << key << " = \"" << val << "\"" << std::endl;
        }

        template<typename Dummy>
        void set(std::string key, std::string alias, std::string val) {
                std::cout << alias << " = \"" << val << "\"" << " # " << key << std::endl;
        }

        void comment(const std::string &s) {
                if (s.empty()) {
                        std::cout << std::endl;
                        return;
                }
                std::cout << "# " << s << std::endl;
        }
};

template<typename Map>
void setup_config_defualuts(Map &config_map) {
        // hardcoded default config values
        //
        // On UBUNTU, the Mono font could be even better....
        config_map.template set<std::string>("term_font", "Terminus");
        config_map.template set<int>("term_font_size", "12");
        config_map.template set<bool>("allow_bold", "true");
        config_map.template set<bool>("show_tabbar", "true");
        config_map.template set<GdkRGBA>("tabbar_bg_color","#303030");
        config_map.template set<bool>("tabbar_on_bottom", "false");
        config_map.template set<uint32_t>("scrollback_lines", "50000");
        config_map.template set<std::string>("command", "/bin/bash");
        config_map.template set<bool>("audible_bell", "false");
        config_map.template set<uint32_t>("startup_tabs", "1");
        config_map.template set<bool>("auto_open_tabs", "false");
        config_map.template set<uint32_t>("window_width", "400");
        config_map.template set<uint32_t>("window_height", "300");

        config_map.comment("");
        config_map.comment("Terminal font colors");

        config_map.template set<GdkRGBA>("color_bg", "#000000");
        config_map.template set<GdkRGBA>("color_fg", "#e5e5e5");

        config_map.comment(""); config_map.comment("Black");
        config_map.template set<GdkRGBA>("color_0", "color_black", "#000000");
        config_map.template set<GdkRGBA>("color_8", "color_black_light", "#4d4d4d");

        config_map.comment(""); config_map.comment("Red");
        config_map.template set<GdkRGBA>("color_1", "color_red", "#B22222");
        config_map.template set<GdkRGBA>("color_9", "color_red_light", "#ED2939");

        config_map.comment(""); config_map.comment("Green");
        config_map.template set<GdkRGBA>("color_2", "color_green", "#00a000");
        config_map.template set<GdkRGBA>("color_10", "color_green_light", "#32cd32");

        config_map.comment(""); config_map.comment("Yellow");
        config_map.template set<GdkRGBA>("color_3", "color_yellow", "#cdcd00");
        config_map.template set<GdkRGBA>("color_11", "color_yellow_light", "#ffff00");

        config_map.comment(""); config_map.comment("Blue");
        config_map.template set<GdkRGBA>("color_4", "color_blue", "#2346DF");
        config_map.template set<GdkRGBA>("color_12", "color_blue_light", "#2b65ec");

        config_map.comment(""); config_map.comment("Magenta");
        config_map.template set<GdkRGBA>("color_5", "color_magenta", "#AA00AA");
        config_map.template set<GdkRGBA>("color_13", "color_magenta_light", "#C154C1");

        config_map.comment(""); config_map.comment("Cyan");
        config_map.template set<GdkRGBA>("color_6", "color_cyan", "#58C6ED");
        config_map.template set<GdkRGBA>("color_14", "color_cyan_light", "#00DFFF");

        config_map.comment(""); config_map.comment("White");
        config_map.template set<GdkRGBA>("color_7", "color_white", "#e5e5e5");
        config_map.template set<GdkRGBA>("color_15", "color_white_light", "#ffffff");
}

} // namespace

void dump_default_config() {
        ConfigDumpator_t cd;
        setup_config_defualuts(cd);
}

void Config_t::init_defaults() {
        setup_config_defualuts(config_map);
}

} // namespace s28
