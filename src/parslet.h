#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include <ctype.h>
#include <string>

#include "error.h"

namespace s28 {
namespace parser {

enum Reason_t {
    UNDEFINED, OVERFLOW, RANGE, EXPECT
};

template<parser::Reason_t REASON>
class Error_t : public s28::HisError_t<errcode::PARSER> {
    public:
        Error_t(const std::string &msg = "parslet") :
            s28::HisError_t<errcode::PARSER>(msg) {}
};

// Parslet_t is a parsing primitive. It holds pointers to the beginning and to
// the end of the parsed string.
// 
// All the methods could be called with any argument without causing a memory
// violation. They either return a value or throw an exception.

class Parslet_t {
public:
    static const int eof = -1;

    /**
     * Constructor, empty parslet by default
     */
    Parslet_t() : it(nullptr), eit(nullptr) {}

    Parslet_t(const char *it, const char *eit) :
        it(it), eit(eit)
    {}

    Parslet_t(const std::string &s) :
        it(s.c_str()), eit(s.c_str() + s.size())
    {}

    void reset(const std::string &s) {
        it = s.c_str();
        eit = it + s.size();
    }

    void reset() {
        eit = it = nullptr;
    }

    std::string str() const {
        if (it == nullptr || it >= eit) return std::string();
        return std::string(it, eit);
    }

    const char * begin() const { return it; }

    const char * end() const { return eit; }

    int operator[](ssize_t i) const {
        const char *c;
        if (i >= 0) {
            c = it + i;
        } else {
            c = eit + i;
        }
        if (c >= eit || c < it) return eof;
        return (unsigned char)*c;
    }

    char at(size_t i) const {
        int rv = (*this)[i];
        if (rv == eof)
            throw Error_t<RANGE>();
        return (char)rv;
    }

    int next() {
        if (it >= eit) return eof;
        int rv = *it;
        ++it;
        return (unsigned char)rv;
    }

    int operator++() {
        next();
        return (*this)[0];
    }

    int operator++(int) {
        return next();
    }

    int operator+=(size_t i) {
        if (it + i > eit) {
            it = eit;
            return eof;
        }
        int rv = (unsigned char)*it;
        it += i;
        return rv;
    }

    operator bool() const {
        return it < eit;
    }

    int operator*() const {
        return at(0);
    }

    void expect_eof() {
        if (!empty())
            throw Error_t<EXPECT>();
    }

    void expect_char(char c) {
        int k = (*this)[0];
        if (k == -1)
            throw Error_t<OVERFLOW>();

        if ((char)k != c)
            throw Error_t<EXPECT>();
        next();
    }

    void skip() {
        if (it >= eit)
            throw Error_t<OVERFLOW>();
        ++it;
    }

    int last() const {
        if (it >= eit) return eof;
        return eit[-1];
    }

    int first() const {
        if (eit > it) return *it;
        return eof;
    }

    bool shift() {
        if (it < eit) {
            --eit;
            return true;
        }
        return false;
    }

    size_t size() const {
        if (it > eit)
            throw Error_t<OVERFLOW>();
        return eit - it;
    }

    bool empty() const { return it == eit; }

private:
    const char *it;
    const char *eit;


};

inline void ltrim(Parslet_t &p) {
    while(p && isspace(*p)) {
        p++;
    }
}

inline void rtrim(Parslet_t &p) {
    while(isspace(p.last())) {
        p.shift();
    }
}

inline void trim(Parslet_t &p) {
    ltrim(p);
    rtrim(p);
}

inline std::string qu(Parslet_t &p) {
    p.expect_char('"');
    std::string rv;
    for (;;) {
        if (*p == '"') {
            p.skip();
            return rv;
        }

        if (*p == '\\') {
            rv += p.at(1);
            p += 2;
            continue;
        }

        rv += (char)(p++);
    }
}

inline Parslet_t split(Parslet_t &p, char c) {
    Parslet_t orig = p;
    const char *it = p.begin();
    for (;;) {
        int x = p.next();
        if (x == Parslet_t::eof) {
            p = orig;
            throw Error_t<EXPECT>();
        }
        if ((unsigned char)x == (unsigned char)c) {
            return Parslet_t(it, p.begin() - 1);
        }
    }
}


inline std::pair<std::string, std::string> eq(Parslet_t &p) {
    std::pair<std::string, std::string> rv;
    ltrim(p);
    if (*p == '"') {
        rv.first = qu(p);
        ltrim(p);
        p.expect_char('=');
    } else {
        Parslet_t l = split(p, '=');
        rtrim(l);
        rv.first = l.str();
    }
    ltrim(p);

    if (p[0] == '"') {
        rv.second = qu(p);
    } else {
        trim(p);
        rv.second = p.str();
    }
    return rv;
}

inline Parslet_t word(Parslet_t &p) {
    ltrim(p);
    if (!isgraph(*p)) {
        throw Error_t<EXPECT>();
    }
    Parslet_t rv = p;
    while (p && !isspace(*p)) {
        p.skip();
    }
    return Parslet_t(rv.begin(), p.begin());
}


} // nemaspace parser
} // namespace s28

#endif /* SRC_PARSER_H */
