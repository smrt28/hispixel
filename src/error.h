#ifndef S28_ERROR_H
#define S28_ERROR_H

#include <stdint.h>

#include <string>
#include <exception>
#include <sstream>

#include "errorcodes.h"

namespace s28 {

class Error_t : public std::exception
{
public:
    Error_t(const std::string &msg) :
        msg(msg)
    {}

    virtual ~Error_t() throw() {}

    const char* what() const throw() {
        return msg.c_str();
    }

    virtual int code() const { return 0; }
private:
    std::string msg;
};

template<int CODE>
class HisError_t : public s28::Error_t {
public:
    HisError_t(const std::string &msg) : Error_t(msg) {}

    int code() const override { return CODE; }
};


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

template<int CODE>
class LErr_t {
public:
    void operator=(const RErr_t &re) {
        throw HisError_t<CODE>(re.str());
    }
};

} // namespace detail

#define RAISE(code)\
    s28::detail::LErr_t<::s28::errcode::code>()=s28::detail::RErr_t()

}

#endif
