#ifndef SRC_CONFIG_H
#define SRC_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <boost/ptr_container/ptr_map.hpp>

#include "anytypemap.h"
#include "error.h"

namespace s28 {

// represents a key on keyboard
class KeySym_t {
public:
    KeySym_t() : mask(0), key(0) {}
    guint mask; // meta keys bit mask
    uint32_t key; // key code
    std::string text;
};

class Config_t {
public:

    // key action class
    class Action_t {
    public:
        // the actions, usually invoked by a key press
        enum ActionType_t {
            ACTION_NONE,
            ACTION_FOCUS,
            ACTION_OPENTAB,
            ACTION_FOCUS_NEXT,
            ACTION_FOCUS_PREV,
            ACTION_CLOSE_LAST,
            ACTION_BE_FIRST,
            ACTION_TOGLE_TABBAR,
            ACTION_SET_WORKSPACE
        };

        Action_t() :
            type(ACTION_NONE)
        {}

        Action_t(ActionType_t type) :
            type(type)
        {}


        Action_t(ActionType_t type, int data) :
            type(type), data(data)
        {}

        ActionType_t type;
        int data = 0; // arbitrary data
    };


    class KeyBinding_t {
    public:
        KeyBinding_t(const KeySym_t &keysym, const Action_t &action) :
            keysym(keysym),
            action(action)
        {}
        KeySym_t keysym; // the key (or key combination)
        Action_t action; // what to the when pressed
    };

    template<typename Type_t>
    const Type_t & get(const std::string &key) const {
        return config_map.get<Type_t>(key);
    }

    /** 
     * Read and parse config file. Iterate all the file names and use the
     * first which opens for reading. If there is a syntax error in the file,
     * it throws.
     * @param files vector of file names
     * @return true if config file read successfully
     */
    bool init(const std::vector<std::string> &files);
    bool init(const std::string &file) {
            std::vector<std::string> v;
            v.push_back(file);
            return init(v);
    }

    typedef std::vector<KeyBinding_t> KeyBindings_t;

    /**
     * @return keybindings
     */
    const KeyBindings_t & get_keybindings() const { return keybindings; }

    // true if close_last keybinding set
    bool has_close_last = false;

private:
    void init_defaults();
    bool init_internal(const std::string &config_file);
    int parse_config_line(const std::string &line);

    AnyTypeMap_t config_map;
    KeyBindings_t keybindings;
};
} // namespace s28

#endif /* SRC_CONFIG_T */
