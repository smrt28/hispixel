#ifndef SRC_KEYEVENT_H
#define SRC_KEYEVENT_H

#include <gtk/gtk.h>

#include <string>

namespace s28 {

bool match_event(const std::string &descr, GdkEvent *event);

}

#endif /* SRC_KEYEVENT_H */
