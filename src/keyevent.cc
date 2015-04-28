#include <vector>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ctype.h>

#include "keyevent.h"

namespace s28 {

static void tolower(std::string &data) {
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
}

static guint MOD_MASK = GDK_MOD1_MASK /*| GDK_MOD2_MASK*/ | GDK_MOD3_MASK
    | GDK_MOD4_MASK | GDK_MOD5_MASK | GDK_CONTROL_MASK | GDK_SHIFT_MASK;

bool match_event(const std::string &descr, GdkEvent *event) {
    if (event->type != GDK_KEY_PRESS &&
        event->type != GDK_KEY_RELEASE) return false;


    std::vector<std::string> v;
    boost::split(v, descr, boost::is_any_of(" "));

    guint mask = 0;

    uint32_t key = 0;

    for (std::string s: v) {
        if (s.empty()) continue;
        if (s.size() == 1) {
            key = s[0];
            continue;
        }

        tolower(s);

        if (s[0] == 'm') {
            if (s == "mod1") { mask |= GDK_MOD1_MASK; continue; }
            if (s == "mod2") { mask |= GDK_MOD2_MASK; continue; }
            if (s == "mod3") { mask |= GDK_MOD3_MASK; continue; }
            if (s == "mod4") { mask |= GDK_MOD4_MASK; continue; }
            if (s == "mod5") { mask |= GDK_MOD5_MASK; continue; }
        }
        
        if (s == "ctrl") { mask |= GDK_CONTROL_MASK; continue; }
        if (s == "shift") { mask |= GDK_SHIFT_MASK; continue; }

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
        }

        if (s == "pause") { key = GDK_KEY_Pause; continue; }
        if (s == "escape") { key = GDK_KEY_Escape; continue; }
        if (s == "scrolllock") { key = GDK_KEY_Scroll_Lock; continue; }
        if (s == "delete") { key = GDK_KEY_Delete; continue; }
    }

    if (key == 0) return false;

    std::cout << gdk_keyval_name(event->key.keyval) << std::endl;

    std::cout << event->key.keyval << " " << (event->key.state & MOD_MASK) << std::endl;
    std::cout << "*" << key << " " << mask << std::endl;

    if (key == event->key.keyval && (event->key.state & MOD_MASK) == mask)
        return true;

    return false;
}

}

