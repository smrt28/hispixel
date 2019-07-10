#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include <ctype.h>
#include <string>

#include "error.h"

namespace s28 {
namespace parser {

// parser low level error codes (reasons)

static const int UNDEFINED = 1;
static const int OVERFLOW  = 2;
static const int RANGE     = 3;
static const int EXPECT    = 4;

template<int REASON>
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

    /**
     * Constructor
     * @param it the string beginning (iterator)
     * @param eit the string end (end iterator)
     */
    Parslet_t(const char *it, const char *eit) :
        it(it), eit(eit)
    {}

    /**
     * Constructor
     * @param s the string
     */
    Parslet_t(const std::string &s) :
        it(s.c_str()), eit(s.c_str() + s.size())
    {}

    /**
     * Construcor
     * @param s string
     */
    void reset(const std::string &s) {
        it = s.c_str();
        eit = it + s.size();
    }

    /**
     * Make parslet empty
     */
    void reset() {
        eit = it = nullptr;
    }

    /**
     * @return content of parslet as a string
     */
    std::string str() const {
        if (it == nullptr || it >= eit) return std::string();
        return std::string(it, eit);
    }

    /**
     * @return parslet begin
     */
    const char * begin() const { return it; }

    /**
     * @return parslet end
     */
    const char * end() const { return eit; }

    /**
     * @param i index within the parslet
     * @return character at index i. If the index is out of the parslet range, return eof
     */
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

    /**
     * @param i index within the parslet
     * @return same as operator[], just throws in case of index out of the parslet range
     */
    char at(size_t i) const {
        int rv = (*this)[i];
        if (rv == eof)
            throw Error_t<RANGE>();
        return (char)rv;
    }

    /**
     * Shifts the parslet beggining by 1 character
     * @return first character of the parslet before the shift
     */
    int next() {
        if (it >= eit) return eof;
        int rv = *it;
        ++it;
        return (unsigned char)rv;
    }

    /**
     * Shifts the parslet beggining by 1 character
     * @return first character of the parslet after the shift
     */
    int operator++() {
        next();
        return (*this)[0];
    }

    /**
     * Shifts the parslet beggining by 1 character
     * @return first character of the parslet
     */
    int operator++(int) {
        return next();
    }


    /**
     * Shifts the parslet beggining by 1 character.
     * @return first character of the parslet before the shift
     */
    int operator+=(size_t i) {
        if (it + i > eit) {
            it = eit;
            return eof;
        }
        int rv = (unsigned char)*it;
        it += i;
        return rv;
    }

    /**
     * @return false if the parslet is empty
     */
    operator bool() const {
        return it < eit;
    }

    /**
     * @return current character. Throws if the parslet is empty.
     */
    int operator*() const {
        return at(0);
    }

    /**
     * throws if the parslet is not empty
     */
    void expect_eof() {
        if (!empty())
            throw Error_t<EXPECT>();
    }

    /**
     * Throws if the character c is not at the beginning of the parslet. Calls next().
     * @param c expected character
     */
    void expect_char(char c) {
        int k = (*this)[0];
        if (k == -1)
            throw Error_t<OVERFLOW>();

        if ((char)k != c)
            throw Error_t<EXPECT>();
        next();
    }

    /**
     * Throws if the parslet is empty. Calls next().
     */
    void skip() {
        if (it >= eit)
            throw Error_t<OVERFLOW>();
        ++it;
    }

    /**
     * @return the last parslet character
     */
    int last() const {
        if (it >= eit) return eof;
        return eit[-1];
    }

    /**
     * Shifts the last parslet character
     * @return false if the parslet is empty
     */
    bool shift() {
        if (it < eit) {
            --eit;
            return true;
        }
        return false;
    }

    /**
     * @return parslet size
     */
    size_t size() const {
        if (it > eit)
            throw Error_t<OVERFLOW>();
        return eit - it;
    }

    /**
     * @return ture if parslet is empty
     */
    bool empty() const { return it == eit; }

private:
    const char *it;
    const char *eit;


};


/**
 * Trim parslet spacess from left
 * @param p parslet
 */
inline void ltrim(Parslet_t &p) {
    while(p && isspace(*p)) {
        p++;
    }
}

/**
 * Trim parslet spacess from right
 * @param p parslet
 */
inline void rtrim(Parslet_t &p) {
    while(isspace(p.last())) {
        p.shift();
    }
}

/**
 * Trim parslet spacess from left and right
 * @param p parslet
 */
inline void trim(Parslet_t &p) {
    ltrim(p);
    rtrim(p);
}

/**
 * Parse a quoted string. The quote within the string could be escaped by '\' character.
 * @param p the string enclosed in quotes
 * @return the string
 */
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

/**
 * Splits the string by the firs occurence of c character.
 * @param p input parslet. It would contain the tail part after the split.
 * @param c the character
 * @return the first part
 */
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


/**
 * Parse "key=value" equation style string format.
 * @param p parslet
 * @return key/value pair of strings
 */
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

/**
 * Read first word from the parslet. Shift the parslet beginning after the
 * end of the next word. Throws, if there is no remaining word in the parslet.
 * @param p the parslet
 * @return the word
 */
inline Parslet_t word(Parslet_t &p) {
    ltrim(p);
    if (!isalnum(*p)) {
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
