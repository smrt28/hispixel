#include "gtest/gtest.h"
#include "parslet.h"
#include "valuecast.h"
#include "anytypemap.h"


// anytypemap.h
TEST(AnyTypeMap, map) {
    s28::AnyTypeMap_t m;
    m.set<int>("a", "10"); // insert 10
    EXPECT_EQ(m.get<int>("a"), 10); // expect 10
    auto v = m.find("a");
    EXPECT_NE(v, nullptr); // expect value of 10
    v->set("11"); // restet to 11

    EXPECT_THROW(v->set("a11"), s28::Error_t); // "a11" is not an integer
    EXPECT_EQ(m.get<int>("a"), 11); // 11 was set previously
    EXPECT_THROW(m.get<std::string>("a"), s28::Error_t); // not a string
    m.set<std::string>("a", "v"); // reset to "v"
    EXPECT_EQ(m.get<std::string>("a"), "v"); // now it's a string
    EXPECT_THROW(m.get<int>("a"), s28::Error_t); // but not int
}


// valuecast.h
TEST(Value, cast) {
    // juset check expected results for several cases...
    EXPECT_TRUE(s28::value_cast<bool>("1"));
    EXPECT_TRUE(s28::value_cast<bool>("true"));
    EXPECT_TRUE(s28::value_cast<bool>("yes"));
    EXPECT_FALSE(s28::value_cast<bool>("0"));
    EXPECT_FALSE(s28::value_cast<bool>("false"));
    EXPECT_FALSE(s28::value_cast<bool>("no"));
    EXPECT_THROW(s28::value_cast<bool>("2"), s28::Error_t);
    EXPECT_THROW(s28::value_cast<bool>(""), s28::Error_t);
    EXPECT_THROW(s28::value_cast<bool>("ajflas"), s28::Error_t);
    EXPECT_EQ(s28::value_cast<int>("10"), 10);
    EXPECT_EQ(s28::value_cast<std::string>("10"), "10");
}


// parslet.h
TEST(Parsing, trim) {
    // trim those strings...
    std::string s  = "    one, two, three   ";
    std::string ss =     "one, two, three";
    std::string s2 =     "one, two, three   ";

    // ...from right and left and check results
    s28::parser::Parslet_t p(s);
    EXPECT_EQ(p.str(), s);
    s28::parser::ltrim(p);
    EXPECT_EQ(p.str(), s2);
    s28::parser::rtrim(p);
    EXPECT_EQ(p.str(), ss);
    EXPECT_EQ(p.last(), 'e');

    p.reset();

    // echeck empty()
    EXPECT_TRUE(p.empty());
    EXPECT_FALSE(p);
    EXPECT_EQ(p.size(), 0U);
    EXPECT_EQ(p.str(), "");

    // trimmming empty string...
    s28::parser::ltrim(p);
    s28::parser::rtrim(p);
    EXPECT_EQ(p.str(), "");

    // eof = -1
    EXPECT_EQ(p[0], -1);
    EXPECT_EQ(p[1], -1);
    EXPECT_EQ(p[-1], -1);

    std::string q1 = "  \" 123\\\"   \"45  ";
    std::string res = " 123\"   ";
    p.reset(q1);

    // how trim works with spec. characters
    s28::parser::trim(p);

    // unqote the string
    EXPECT_EQ(res, s28::parser::qu(p));
    EXPECT_EQ(p.str(), "45");
}

// parslet.h
TEST(Parsing, refs) {
    std::string ss = "one, two, three";
    s28::parser::Parslet_t p(ss);
    EXPECT_EQ(p.size(), ss.size());

    // calculate a checksum of the string using the parslet
    int cnt = 0;
    for (int i = -10; i < 20; ++i) {
        int a = p[i];
        int b = a + 1;

        try {
            b = p.at(i);
        } catch(const s28::Error_t &e) {
            b = -1;
        }

        EXPECT_EQ(a, b);
        cnt += a;
    }

    // check the checksum
    EXPECT_EQ(cnt, 2309);

    // iterating parslet and check it's equal to orig. string
    for (size_t i = 0; i < ss.size(); ++i) {
        EXPECT_EQ(ss[i], (char)p.next());
    }

    p.reset(ss);

    // test operators
    EXPECT_EQ(p++, ss[0]);
    EXPECT_EQ(*p, ss[1]);
    EXPECT_EQ(++p, ss[2]);
    EXPECT_EQ(*p, ss[2]);

    p.reset();
    EXPECT_THROW(*p, s28::Error_t);


    // should not throw
    p++; p+=1;
}


// parslet.h
TEST(Parsing, split) {
    std::string ss = "one, two, three";

    // empty parslet defined by empty string
    std::string empty;
    s28::parser::Parslet_t p(empty);

    // at(1) must throw on empty string
    EXPECT_THROW(p.at(1), std::exception);

    p.reset(ss);

    // expect chars from original string
    for (size_t i = 0; i < ss.size(); ++i) {
        p.expect_char(ss[i]);
    }
    EXPECT_TRUE(p.empty());

    std::string q1 = " one = a b c d  ";
    p.reset(q1);

    // split by "="
    s28::parser::Parslet_t key = s28::parser::split(p, '=');

    // check left/right side of the equation
    EXPECT_EQ(key.str(), " one ");
    EXPECT_EQ(p.str(), " a b c d  ");

    p.reset(q1);

    // there is no '-' in q1...
    EXPECT_THROW(s28::parser::split(p, '-'), s28::Error_t);
    EXPECT_EQ(p.str(), q1);
}

// parslet.h
TEST(Parsing, word) {
    std::string s = "   123   abc \n\t\r e-a x";
    s28::parser::Parslet_t p(s);

    // iterate words...
    EXPECT_EQ(s28::parser::word(p).str(), "123");
    EXPECT_EQ(s28::parser::word(p).str(), "abc");
    EXPECT_EQ(s28::parser::word(p).str(), "e-a");
    EXPECT_EQ(s28::parser::word(p).str(), "x");

    // must be empty now
    EXPECT_TRUE(p.empty());

    // there is no word remaining
    EXPECT_THROW(s28::parser::word(p), s28::Error_t);

    s = "   123   ";
    p.reset(s);
    EXPECT_EQ(s28::parser::word(p).str(), "123");
    EXPECT_EQ(p.size(), 3U); // 3 spaces remains
}

// parslet.h
TEST(Parsing, noname) {
    using namespace s28::parser;
    std::string s;
    s = " a key = the value ";

    Parslet_t p(s);

    // parse equation
    auto kv = s28::parser::eq(p);

    // chek left/right
    EXPECT_EQ(kv.first, "a key");
    EXPECT_EQ(kv.second, "the value");

    s = " \" a key \" = \"the value\" X ";
    p.reset(s);

    // parse equation with quotes
    kv = s28::parser::eq(p);
    EXPECT_EQ(kv.first, " a key ");
    EXPECT_EQ(kv.second, "the value");
    EXPECT_EQ(p.str(), " X ");

    s = "";
    p.reset(s);

    // throw on empty string
    EXPECT_THROW(s28::parser::eq(p), s28::Error_t);

    // missing tailing "-character
    s = "   \"   - =  x  ";
    p.reset(s);
    EXPECT_THROW(s28::parser::eq(p), s28::Error_t);

    // equation with empty sides
    s = "=";
    p.reset(s);
    kv = s28::parser::eq(p);
    EXPECT_EQ(kv.first, "");
    EXPECT_EQ(kv.second, "");

    // reverse indexing
    s = "123";
    p.reset(s);
    EXPECT_EQ(p[-1], '3');
    EXPECT_EQ(p[-2], '2');
    EXPECT_EQ(p[-3], '1');

    // expect char check
    p.expect_char('1');
    p.expect_char('2');
    p.expect_char('3');

    // check EOF
    EXPECT_EQ(p[-1], -1);
    EXPECT_FALSE(p);

    // remove head and tail character from "123"
    p.reset(s);
    p.shift();
    p.next();

    // "2" remains
    EXPECT_EQ(p.str(), "2");

    // echeck no-ASCI characters handling
    char buf[] = { -4, -1, -56, 0 };
    s = buf;
    p.reset(s);

    EXPECT_TRUE(p[1] > 0);
    EXPECT_EQ(p[11], -1);

    // must trim properly...
    trim(p);
    EXPECT_EQ(p.size(), 3U);

    // \ character in ""
    s = "\"12\\345\\";
    p.reset(s);

    EXPECT_THROW(qu(p), s28::Error_t);

    // escaped tail `"`
    s = "\"12\\345\\\\\"";
    p.reset(s);
    EXPECT_EQ(qu(p), "12345\\");

    EXPECT_THROW(qu(p), s28::Error_t);

    s = buf;
    int i = 0;
    p.reset(s);
    // coun characters
    while (p.next()!=-1) ++i;
    EXPECT_EQ(i, 3);

    s = " aaa ";
    s[2] = -1; // pretending EOF
    p.reset(s);
    s28::parser::trim(p);
    std::string s2 = "aaa"; s2[1] = -1;
    EXPECT_EQ(s2, p.str());

    // check expect_eof()
    p.reset(s2);
    p.expect_char('a');
    p.expect_char(-1);
    p.expect_char('a');
    p.expect_eof();

    // p.expect_char(-1) doesn't expect EOF
    EXPECT_THROW(p.expect_char(-1), s28::Error_t);
    s = " ";

    // try expect on every possible character
    for (int i = 0; i< 520; ++i) {
        s[0] = (char)i;
        p.reset(s);
        EXPECT_EQ(s[0], *p);
    }

    // again, -1 is not EOF!
    s = "      ";
    s[3] = -1;
    p.reset(s);
    s28::parser::trim(p);

    // -1 is a character
    EXPECT_EQ(p.size(), size_t(1));
    EXPECT_EQ(*p, -1);
}
