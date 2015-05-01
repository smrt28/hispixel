#ifndef S28_ERRORCOES_H
#define S28_ERRORCOES_H

namespace s28 {

template<int category, int code>
class ErrorCode_t {
public:
    static const int CODE = code;
    static const int CATEGORY = category;
};


namespace errc {
    static const int MEMORY = 1;
    static const int IO = 2;
    static const int INTERNAL = 3;
    static const int CONFIG_PARSE = 4;
}

namespace errcode {

    // IO category
    typedef ErrorCode_t<errc::IO, 200> READ;
    typedef ErrorCode_t<errc::IO, 201> WRITE;
    typedef ErrorCode_t<errc::IO, 202> STAT;
    typedef ErrorCode_t<errc::IO, 203> SEEK;
    typedef ErrorCode_t<errc::IO, 204> OPEN;

    typedef ErrorCode_t<errc::INTERNAL, 401> IMPOSSIBLE;
    typedef ErrorCode_t<errc::INTERNAL, 402> ARGS;
    typedef ErrorCode_t<errc::INTERNAL, 500> FATAL;


    typedef ErrorCode_t<errc::CONFIG_PARSE, 800> CFG_PARSE;
    typedef ErrorCode_t<errc::CONFIG_PARSE, 801> UNKNOWN_ACTION;
    typedef ErrorCode_t<errc::CONFIG_PARSE, 802> VALUE_CAST;
    typedef ErrorCode_t<errc::CONFIG_PARSE, 803> VALUE_DEFINITION;
    typedef ErrorCode_t<errc::CONFIG_PARSE, 804> CFG_VAL_NOT_SET;
}
}

#endif
