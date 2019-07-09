#ifndef SRC_VALUECAST_H
#define SRC_VALUECAST_H

#include <gtk/gtk.h>
#include <ctype.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include "error.h"
namespace s28 {
namespace aux {

/** Config value cast implementations/specializations
 */

/**
 * General implementation uses boost::lexical_cast
 */
template<typename Type_t>
struct ValueCast_t {
    static Type_t cast(const std::string &s) {
        try {
            return boost::lexical_cast<Type_t>(s);
        } catch(...) {
            RAISE(VALUE_CAST);
        }
        throw __PRETTY_FUNCTION__; // not reachable, avoids compiler warning
    }
};

/**
 * Boolean could be defined 1/0 true/false yes/no
 */
template<>
struct ValueCast_t<bool> {
    static bool cast(const std::string &_s) {
        std::string s = _s;

        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        if (s == "true" || s == "1" || s == "yes" ||
                s == "y" || s == "t") return true;

        if (s == "false" || s == "0" || s == "no" ||
                s == "n" || s == "f") return false;

        RAISE(VALUE_CAST);
        throw __PRETTY_FUNCTION__; // not reachable, avoids compiler warning
    }
};

/**
 * casts color defined like "#303030" to GdkRGBA struct
 */
template<>
struct ValueCast_t<GdkRGBA> {
    static GdkRGBA cast(const std::string &s) {
        GdkRGBA rv;
        if (!gdk_rgba_parse(&rv, s.c_str())) {
            RAISE(VALUE_CAST);
        }
        return rv;
    }
};

} // namespace aux

template<typename Type_t>
Type_t value_cast(const std::string &s) {
    return aux::ValueCast_t<Type_t>::cast(s);
}
} // namespace s28

#endif /* SRC_VALUECAST_H */
