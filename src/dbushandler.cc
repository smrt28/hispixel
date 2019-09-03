#include "dbushandler.h"
#include "hispixelapp.h"


namespace s28 {
DbusHandler::DbusHandler(HisPixelApp_t &hispixel) :
    hispixel(hispixel)
{}

void DbusHandler::focus(std::string s) {
    Tabs tt = hispixel.get_tabs();
    tt.find(s).focus();
    hispixel.update_tabbar();
}
} // namespace s28
