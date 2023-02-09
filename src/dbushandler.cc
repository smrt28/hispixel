#include "dbushandler.h"
#include "hispixelapp.h"
#include "parslet.h"
#include <iostream>

namespace s28 {

DbusHandler::DbusHandler(HisPixelApp &hispixel) :
    hispixel(hispixel)
{}

std::string DbusHandler::dump(std::string s) {
    parser::Parslet_t p(s);
    std::string z = parser::word(p).str();
    std::string x = parser::word(p).str();

    ColorManger * zmanager = hispixel.get_z_manager();
    int z_axe = zmanager->name_to_z(z);
    if (z_axe == -1) return std::string();
    Tabs tt = hispixel.get_tabs(z_axe);
    Tab t = tt.find(x);
    if (!t) return std::string();

    return t.dump();
}

} // namespace s28
