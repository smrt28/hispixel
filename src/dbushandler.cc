#include "dbushandler.h"
#include "hispixelapp.h"
#include "parslet.h"

namespace s28 {
DbusHandler::DbusHandler(HisPixelApp &hispixel) :
    hispixel(hispixel)
{}

void DbusHandler::focus(std::string s) {
    Tabs tt = hispixel.get_tabs();
    tt.find(s).focus();
    hispixel.update_tabbar();
}


void DbusHandler::feed(std::string s) {
    Tabs tt = hispixel.get_tabs();
    parser::Parslet_t p(s);
    parser::ltrim(p);
    Tab t = tt.find(parser::word(p).str());
    if (!t) RAISE(NOT_FOUND) << "tab not found";
    p.expect_char(' ');
    t.feed(p.str());
}


void DbusHandler::opentab(std::string s) {
    HisPixelApp::TabConfig tc;
    tc.name = s;
    tc.focus = false;
    hispixel.open_tab(tc);
}

void DbusHandler::rename(std::string s) {
    parser::Parslet_t p(s);
    std::string old_name = parser::word(p).str(); // first word specs the tab
    parser::trim(p);

    std::string new_name;
    Tabs tt = hispixel.get_tabs();
    Tab t;

    t = tt.find(old_name);
    new_name = p.str();

    if (!t.is_valid()) return;

    t.set_name(new_name);
    hispixel.update_tabbar();
}

} // namespace s28
