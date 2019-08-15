#ifndef ENVFACTORY_H
#define ENVFACTORY_H

#include <vector>
#include <string>

namespace s28 {

class ArgsFactory {
public:
    ~ArgsFactory() {
        if (!env) reset();
    }

    void add(std::string key, std::string val) {
        std::ostringstream oss;
        oss << key << "=" << val;
        v.push_back(oss.str());
    }

    void add(std::string s) {
        v.push_back(s);
    }

    char **build() {
        if (env) reset();
        len = v.size();
        env = (char **)malloc(sizeof(char * [len + 1]));
        if (!env) return nullptr;
        memset(env, 0, sizeof(char * [len + 1]));
        for (size_t i = 0; i < len; ++i) {
            if (!(env[i] = strdup(v[i].c_str())))
                    return nullptr;
        }
        return env;
    }

private:
    void reset() {
       if (!env) return;
       for (size_t i = 0; i < len; ++i) {
           free(env[i]);
       }
       free(env);
       len = 0;
       env = nullptr;
    }

    std::vector<std::string> v;

    char **env = nullptr;
    size_t len = 0;
};

} // namespace s28

#endif
