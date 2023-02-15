#include "dbushandler.h"
#include "hispixelapp.h"
#include "parslet.h"

#include <iostream>

namespace s28 {

DbusHandler::DbusHandler(HisPixelApp &hispixel) :
    hispixel(hispixel)
{}


#if 0
std::string DbusHandler::focus(std::string s) {
    /*
    Tabs tt = hispixel.get_tabs();
    tt.find(s).focus();
    hispixel.update_tabbar();
    */
    return "0";
}


std::string DbusHandler::feed(std::string s) {
    /*
    Tabs tt = hispixel.get_tabs();
    parser::Parslet_t p(s);
    parser::ltrim(p);
    Tab t = tt.find(parser::word(p).str());
    if (!t) RAISE(NOT_FOUND) << "tab not found";
    p.expect_char(' ');
    t.feed(p.str());
    */
    return "0";
}


std::string DbusHandler::opentab(std::string s) {
    HisPixelApp::TabConfig tc;
    tc.focus = false;
    hispixel.open_tab(tc);
    return "0";
}

std::string DbusHandler::rename(std::string s) {
    /*
    parser::Parslet_t p(s);
    std::string old_name = parser::word(p).str(); // first word specs the tab
    parser::trim(p);

    std::string new_name;
    Tabs tt = hispixel.get_tabs();
    Tab t;

    t = tt.find(old_name);
    new_name = p.str();

    if (!t.is_valid()) {
        RAISE(NOT_FOUND) << "tab not found";
    }

    t.set_name(new_name);
    hispixel.update_tabbar();
    */
    return "0";
}
#endif
std::string DbusHandler::dump(std::string s) {
    parser::Parslet_t p(s);
    std::string z = parser::word(p).str();
    std::string x = parser::word(p).str();

    std::cout << "z=" << z << " x=" << x << std::endl;
    /*
    Tabs tt = hispixel.get_tabs();
    return tt.find(s).dump();
    */
    return "xx";
}

} // namespace s28
