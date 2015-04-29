#ifndef SRC_TCONFIG_T
#define SRC_TCONFIG_T

#include <string>
#include <vector>

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
			ACTION_FOCUS
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

private:

};

}

#endif /* SRC_CONFIG_T */

