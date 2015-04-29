#ifndef S28_ERROR_H
#define S28_ERROR_H

#include <stdint.h>

#include <string>
#include <exception>
#include <sstream>

#include "errorcodes.h"

namespace s28 {


class Error_t
{
public:
    Error_t(int c, const std::string &msg) :
        msg(msg),
        _code(c)
    {}

    virtual ~Error_t() throw() {}
    
    int code() const {
        return _code;
    }

    const char* what() const throw() {
        return msg.c_str();
    }
private:
    std::string msg;
    int _code;
};

template<int CAT>
class GeneralError_t : public Error_t {
public:
    GeneralError_t(int c, const std::string &msg) :
        Error_t(c, msg)
    {}
};


template<typename Code_t>
void raise(const std::string &msg = std::string()) {
    throw GeneralError_t<Code_t::CATEGORY>(Code_t::CODE, msg);
}


namespace detail {

class RErr_t {
public:
    RErr_t() {
        oss.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    }

    template<typename T_t>
    RErr_t & operator <<(T_t val) {
        oss << val;
        return *this;
    }

    std::string str() const {
        return oss.str();
    }
private:
    std::ostringstream oss;
};

template<typename EC_t>
class LErr_t {
public:
    void operator=(const RErr_t &re) {
        s28::raise<EC_t>(re.str());
    }
};

} // namespace detail

#define RAISE(code)\
    s28::detail::LErr_t<s28::errcode::code>()=s28::detail::RErr_t()

}

#endif
