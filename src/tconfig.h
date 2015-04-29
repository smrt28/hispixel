#ifndef SRC_TCONFIG_T
#define SRC_TCONFIG_T

#include <string>
#include <vector>
#include <map>

#include "keyevent.h"

namespace s28 {

class TConfig_t {
public:

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

	void init_defaults();

    std::string get_value(const std::string &key) {
        auto it = kv.find(key);
        if (it == kv.end()) return std::string();
        return it->second;
    }



private:
    std::map<std::string, std::string> kv;

};

}

#endif /* SRC_CONFIG_T */

