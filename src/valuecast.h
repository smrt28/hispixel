#ifndef SRC_VALUECAST_H
#define SRC_VALUECAST_H

#include <gtk/gtk.h>
#include <ctype.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include "error.h"
namespace s28 {
namespace aux {
template<typename Type_t>
struct ValueCast_t {
    static Type_t cast(const std::string &s) {
        try {
            return boost::lexical_cast<Type_t>(s);
        } catch(...) {
            RAISE(VALUE_CAST);
        }
        throw 1; // not reachable
    }
};

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
        throw 1; // not reachable
    }
};

template<>
struct ValueCast_t<GdkRGBA> {
    static GdkRGBA cast(const std::string &_s) {
        GdkRGBA rv;
        if (!gdk_rgba_parse(&rv, _s.c_str())) {
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
