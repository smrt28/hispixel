#include <stdint.h>
#include <string>
#include <boost/algorithm/string.hpp>

#include "colormanager.h"
#include "config.hxx"
#include "valuecast.h"
#include "utils.h"

namespace s28 {


std::string ColorManger::get_color(int index, std::string config_key, std::string _default) {
    std::vector<std::string> tab_colors =  config.get<std::vector<std::string>>(config_key);
    if (tab_colors.empty())
    {
        return _default;
    }
    return tab_colors[ index % int(tab_colors.size()) ];
}

std::string ColorManger::get_z_color_light(int z) {
    return get_color(z, "z_color_1", "ff44ff");
}

namespace {

void to_hex(gdouble n, char *rv) {
    uint32_t i = uint32_t(n * 255);
    if (i > 255) i = 255;
    const char * hex = "0123456789abcdef";
    rv[0] = hex[i>>4];
    rv[1] = hex[i&0xf];
}

std::string to_str(GdkRGBA *c) {
    char buf[7];
    to_hex(c->red, buf + 0);
    to_hex(c->green, buf + 2);
    to_hex(c->blue, buf + 4);
    buf[6] = 0;
    return buf;
}

}
std::string ColorManger::get_z_color_dark(int z) {
    std::string colorstr = std::string("#") + get_color(z, "z_color_1", "ff44ff");
    GdkRGBA color;
    if (!gdk_rgba_parse(&color, colorstr.c_str())) {
        RAISE(FAILED) << "invalid color in z_color_1";
    }
    int zgama = config.get<int>("z_gama");
    utils::apply_gama(color, zgama);

    std::string tmp = to_str(&color);

    return tmp;
}


std::string ColorManger::z_to_name(int z) {
    std::vector<std::string> zs = config.get<std::vector<std::string>>("z_names");
    if (z >= int(zs.size()) || z < 0) return "?";
    return zs[z];
    
}

int ColorManger::name_to_z(std::string s) {
    std::vector<std::string> zs = config.get<std::vector<std::string>>("z_names");
    for (size_t i = 0; i < zs.size(); ++i) {
        if (boost::algorithm::to_lower_copy(zs[i]) == boost::algorithm::to_lower_copy(s))
            return int(i);
    }
    return -1;
}

}
