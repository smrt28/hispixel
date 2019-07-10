#ifndef S28_ERRORCOES_H
#define S28_ERRORCOES_H

/**
 * Codes used to define new exceptions:
 *
 * throw HisError_t<FATAL>();
 *
 */

namespace s28 {

namespace errcode {
    static const int FAILED = 300;
    static const int FATAL = 500;
    static const int UNKNOWN_ACTION = 801;
    static const int VALUE_CAST = 802;
    static const int NOT_FOUND = 803;
    static const int CONFIG = 804;
    static const int PARSER = 805;
}

}
#endif
