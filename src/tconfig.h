#ifndef SRC_TCONFIG_T
#define SRC_TCONFIG_T

#include <string>
#include <vector>
#include <map>
#include <boost/ptr_container/ptr_map.hpp>

#include "keyevent.h"
#include "valuecast.h"
#include "error.h"

namespace s28 {

class TConfig_t {
public:
    template<typename> class Value_t;
private:
    class BaseValue_t {
    public:
        virtual ~BaseValue_t() {}


        template<typename Type_t>
        Type_t & cast() {
            Value_t<Type_t> *res =  dynamic_cast<Value_t<Type_t> *>(this);
            if (!res) {
                RAISE(VALUE_DEFINITION);
            }
            return res->val;
        }

        template<typename Type_t>
        const Type_t & cast() const {
            Value_t<Type_t> *res =  dynamic_cast<Value_t<Type_t> *>(this);
            if (!res) {
                RAISE(VALUE_DEFINITION);
            }
            return res->val;
        }

        virtual void set(const std::string &s) = 0;
    };
public:
    template<typename Type_t>
    class Value_t : public BaseValue_t {
    public:
        Value_t(const std::string &s) :
            val(value_cast<Type_t>(s))
        {}

        void set(const std::string &s) {
            val = value_cast<Type_t>(s);
        }
        Type_t val;
    };


	class Action_t {
	public:
        Action_t() :
            type(ACTION_NONE),
            data(0)
        {}
		enum ActionType_t {
            ACTION_NONE,
			ACTION_FOCUS,
            ACTION_OPENTAB

		};

		Action_t(ActionType_t type, int data) :
			type(type), data(data)
		{}

		ActionType_t type;
		int data;
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

	int parse_config_line(const std::string &line);


    Action_t find_action(GdkEvent *event);

	std::vector<KeyBinding_t> keybindings;

    template<typename Type_t>
    Type_t get(const std::string &key) {
        try {
            return kv.at(key).cast<Type_t>();
        } catch(...) {
            RAISE(FATAL) << "unknow config key: " << key;
        }
        throw 1; // unreachable
    }

	void init_defaults();

private:
    template<typename Type_t>
    void insert_default(std::string key, std::string value) {
        std::auto_ptr<BaseValue_t> bv(new Value_t<Type_t>(value));
        kv.insert(key, bv);
    }

    boost::ptr_map<std::string, BaseValue_t> kv;

};

}

#endif /* SRC_CONFIG_T */

