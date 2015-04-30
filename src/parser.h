#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include <ctype.h>
#include <string>

#include "error.h"

namespace s28 {
namespace parser {

class Parser_t {
public:

    Parser_t(const char *it, const char *eit) :
        it(it), eit(eit)
    {}

    Parser_t(const std::string &s) :
        it(s.c_str()), eit(s.c_str() + s.size())
    {}

    std::string str() {
        return std::string(it, eit);
    }

    void raise() {
        RAISE(CFG_PARSE);
    }
    const char * begin() { return it; }
    const char * end() { return eit; }

    char operator[](size_t i) {
        return at(i);
    }

    char at(size_t i) {
        if (it + i >= eit) raise();
        return it[i];
    }

    char operator++() {
        if (it == eit) raise();
        ++it;
        if (it == eit) raise();
        return *it;
    }

    char operator++(int) {
        if (it == eit) raise();
        return *(it++);
    }

    void operator+=(size_t i) {
        if (it + i > eit) raise();
        it += i;
    }

    operator bool() {
        return it != eit;
    }

    char operator*() {
        if (it == eit) raise();
        return *it;
    }

    void expect_char(char c) {
        if (at(0) != c) raise();
        ++it;
    }

    void skip() {
        if (it == eit) raise();
        ++it;
    }

    size_t size() { return eit - it; }

    const char *it;
    const char *eit;
};

void ltrim(Parser_t &p) {
    while(p && isspace(*p)) {
        p++;        
    }
}

inline std::string qu(Parser_t &p) {    
    p.expect_char('"');
    std::string rv;
    for (;;) {
        if (*p == '\\' && p[1] == '"') {
            rv += '"';
            p += 2;
            continue;
        }
        if (*p == '"') {
            p.skip();
            return rv;
        }
        rv += p++;
    }
}

inline std::string until_char_tr(Parser_t &p, char c) {
    std::string rv;
    ltrim(p);
    const char * it = p.begin();
    const char * eit = it;
    for (;;) {        
        if (*p == c) {
            break;
        }
        if (!isspace(*p)) eit = p.begin();
        p++;
    }
    return std::string(it, eit + 1);
}


inline std::string until_eof_tr(Parser_t &p) {
    std::string rv;
    ltrim(p);
    const char * it = p.begin();
    const char * eit = it;
    for (;;) {        
        if (!p) {
            break;
        }
        if (!isspace(*p)) eit = p.begin();
        p++;
    }
    return std::string(it, eit + 1);
}


Parser_t word(Parser_t &p) {
    ltrim(p);
    if (!isalnum(*p)) p.raise();
    Parser_t rv = p;
    while (p && !isspace(*p)) {
        p.skip();
    }
    rv.eit = p.it;
    return rv;
}

inline std::pair<std::string, std::string> eq(Parser_t &p) {
    std::pair<std::string, std::string> rv;
    ltrim(p);
    if (*p == '"') {
        rv.first = qu(p);
        ltrim(p);
    } else {
        rv.first = until_char_tr(p, '=');
    }
    p.expect_char('=');
    ltrim(p);
    if (*p == '"') {
        rv.second = qu(p);
        ltrim(p);
    } else {
        rv.second = until_eof_tr(p);
    }
    return rv;
}

} // nemaspace parser
} // namespace s28

#endif /* SRC_PARSER_H */
