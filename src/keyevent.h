#ifndef SRC_KEYEVENT_H
#define SRC_KEYEVENT_H

#include <gtk/gtk.h>

#include <string>

namespace s28 {

class KeySym_t {
public:
    KeySym_t() : mask(0), key(0) {}
    guint mask;
    uint32_t key;
};


bool match_gtk_event(GdkEvent *event, const KeySym_t &ks);

KeySym_t parse_key_sym(const std::string &descr);

}

#endif /* SRC_KEYEVENT_H */
