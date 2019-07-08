#ifndef S28_ERROR_H
#define S28_ERROR_H

#include <stdint.h>

#include <string>
#include <exception>
#include <sstream>

#include "errorcodes.h"

namespace s28 {

class ErrorBase_t
{
public:
    ErrorBase_t(const std::string &msg) :
        msg(msg)
    {}

    virtual ~ErrorBase_t() throw() {}

    const char* what() const throw() {
        return msg.c_str();
    }
private:
    std::string msg;
};


class Error_t : public ErrorBase_t {
public:
    Error_t(int code, const std::string &msg) :
        ErrorBase_t(msg),
        code_(code)
    {}

    int code() const { return code_; }

private:
    int code_;
};


template<typename Code_t>
void raise(const std::string &msg = std::string()) {
    throw Error_t(Code_t::CODE, msg);
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

class LErr_t {
public:
    LErr_t(int code) : code(code) {}

    void operator=(const RErr_t &re) {
        throw Error_t(code, re.str());
    }

    int code;
};

} // namespace detail

#define RAISE(code)\
    s28::detail::LErr_t(::s28::errcode::code)=s28::detail::RErr_t()

}

#endif
