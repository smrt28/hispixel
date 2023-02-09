#pragma once

#include "config.hxx"

namespace s28 {

    class ColorManger {
    public:
        ColorManger(const Config_t &config) : config(config) {}

        std::string get_z_color_light(int z);
        std::string get_z_color_dark(int z);

        std::string z_to_name(int z);
        int name_to_z(std::string s);

    private:
        std::string get_color(int index, std::string config_key, std::string _default);
        const Config_t &config;
    };
}
