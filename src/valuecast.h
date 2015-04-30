#ifndef SRC_VALUECAST_H
#define SRC_VALUECAST_H

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
        }
    };

    template<>
    struct ValueCast_t<bool> {
        static bool cast(const std::string &s) {
            if (s == "true" || s == "TRUE" || s == "True"
                    || s == "1") return true;
            if (s == "false" || s == "FALSE" || s == "False"
                    || s == "0") return false;
            RAISE(VALUE_CAST);
        }
    };
} // namespace aux

template<typename Type_t>
Type_t value_cast(const std::string &s) {
    return aux::ValueCast_t<Type_t>(s);
}
} // namespace s28

#endif /* SRC_VALUECAST_H */
