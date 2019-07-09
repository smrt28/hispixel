#ifndef SRC_CONFIG_H
#define SRC_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/optional.hpp>

#include "valuecast.h"
#include "error.h"

namespace s28 {

// represents a key on keyboard
class KeySym_t {
public:
    KeySym_t() : mask(0), key(0) {}
    guint mask; // meta keys bit mask
    uint32_t key; // key code
};

class Config_t {
public:
    template<typename> class Value_t;
private:
    // class for values defined in config file
    class BaseValue_t {
    public:
        virtual ~BaseValue_t() {}

        template<typename Type_t>
        const Type_t & cast() const {
            const Value_t<Type_t> *res = dynamic_cast<const Value_t<Type_t> *>(this);
            if (!res) RAISE(FATAL) << "requesting invalid config value type";
            return res->get();
        }

        virtual void set(const std::string &s) = 0;
    };

public:

    // config value holder
    template<typename Type_t>
    class Value_t : public BaseValue_t {
    public:
        void set(const std::string &s) {
            val = value_cast<Type_t>(s);
        }

        const Type_t & get() const {
            // ensure the value is set
            if (!val) RAISE(CFG_VAL_NOT_SET);
            return val.value();
        }
    private:
        boost::optional<Type_t> val;
    };


    class Action_t {
    public:
        enum ActionType_t {
            ACTION_NONE,
            ACTION_FOCUS,
            ACTION_OPENTAB,
            ACTION_FOCUS_NEXT,
            ACTION_FOCUS_PREV,
            ACTION_CLOSE_LAST
        };

        Action_t() :
            type(ACTION_NONE),
            data(0)
        {}

        Action_t(ActionType_t type) :
            type(type),
            data(0)
        {}


        Action_t(ActionType_t type, int data) :
            type(type), data(data)
        {}

        ActionType_t type;
        int data; // arbitrary data
    };


    class KeyBinding_t {
    public:
        KeyBinding_t(const KeySym_t &keysym, const Action_t &action) :
            keysym(keysym),
            action(action)
        {}
        KeySym_t keysym;
        Action_t action;
    };

    template<typename Type_t>
    const Type_t & get(const std::string &key) const {
        try {
            return kv.at(key).cast<Type_t>();
        } catch(...) {
            RAISE(FATAL) << "unknow config key: " << key;
        }
        throw 1; // unreachable
    }


    bool init(const std::vector<std::string> &files);

    typedef std::vector<KeyBinding_t> KeyBindings_t;
    const KeyBindings_t & get_keybindings() const { return keybindings; }

    bool has_close_last = false;

private:
    void init_defaults();
    bool init(const std::string &file);
    int parse_config_line(const std::string &line);

    template<typename Type_t>
    void insert_default(std::string key, std::string value) {
        std::unique_ptr<BaseValue_t> bv(new Value_t<Type_t>());
        bv->set(value);
        kv.insert(key, bv.release());
    }

    boost::ptr_map<std::string, BaseValue_t> kv;
    KeyBindings_t keybindings;
};
} // namespace s28

#endif /* SRC_CONFIG_T */
