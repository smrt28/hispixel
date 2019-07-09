#include <vector>
#include <set>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <ctype.h>

#include "keyevent.h"
#include "error.h"

namespace s28 {

namespace {
void tolower(std::string &data) {
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
}
} // namespace

#define DEF_MASK(m, mm)   if (s == m) { mask |= mm; continue; }

KeySym_t parse_key_sym(const std::string &descr) {
    KeySym_t rv;
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

        if (s == "pause") { key = GDK_KEY_Pause; continue; }
        if (s == "escape") { key = GDK_KEY_Escape; continue; }
        if (s == "scrolllock") { key = GDK_KEY_Scroll_Lock; continue; }
        if (s == "delete") { key = GDK_KEY_Delete; continue; }

        // arrows
        if (s == "left") { key = GDK_KEY_Left; continue; }
        if (s == "right") { key = GDK_KEY_Right; continue; }
        if (s == "up") { key = GDK_KEY_Up; continue; }
        if (s == "down") { key = GDK_KEY_Down; continue; }

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

} // namespace s28
