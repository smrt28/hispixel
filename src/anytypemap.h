#ifndef ANYTYPEMAP_H
#define ANYTYPEMAP_H

#include <string>
#include <vector>
#include <map>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/optional.hpp>
#include "error.h"
#include "valuecast.h"

namespace s28 {

class AnyTypeMap_t {
public:
    class Value_t {
    public:
        virtual ~Value_t() {}

        /**
         * Casts the BaseValue to particular Value
         */
        template<typename Type_t>
        const Type_t & cast() const {
            const ValueImpl_t<Type_t> *res = dynamic_cast<const ValueImpl_t<Type_t> *>(this);
            if (!res) RAISE(FATAL) << "requesting invalid config value type";
            return res->get();
        }

        virtual void set(const std::string &s) = 0;
    };

    /**
     * Return value for given key. Throws if the key doesn't exist or 
     * if it stores a different type.
     * @param key key
     * @return the value
     */
    template<typename Type_t>
    const Type_t & get(const std::string &key) const {
        try {
            return kv.at(key).cast<Type_t>();
        } catch (const s28::Error_t &) {
            throw;
        } catch (...) {
            RAISE(KEY_NOT_FOUND) << "key not found: " << key;
        }
        throw __PRETTY_FUNCTION__; // not reachable, avoids compiler warning
    }

    /**
     * Set value and type for the key
     * @param key key
     */
    template<typename Type_t>
    void set(std::string key, std::string val) {
        std::unique_ptr<Value_t> bv(new ValueImpl_t<Type_t>());
        bv->set(val);
        kv.insert(key, bv.release());
    }

    /**
     * Set type for the key
     * @param key key
     */
    template<typename Type_t>
    void set(std::string key) {
        kv.insert(key, new ValueImpl_t<Type_t>());
    }

    /**
     * Find value for a given key. Return null if the key doesn't exist
     * @param key key
     * @return pointer to value object, if found.
     */
    Value_t * find(std::string key) {
        auto it = kv.find(key);
        if (it == kv.end()) return nullptr;
        return it->second;
    }

private:
    // Value implementation for given typle
    template<typename Type_t>
    class ValueImpl_t : public Value_t {
    public:
        /**
         * Set value by string. Throws if the sting has wrong format.
         *
         * @param s the value to be parsed
         */
        void set(const std::string &s) {
            val = value_cast<Type_t>(s);
        }

        /**
         * @return the wrapped value
         */
        const Type_t & get() const {
            // ensure the value is set
            if (!val) RAISE(CFG_VAL_NOT_SET);
            return val.value();
        }
    private:
        boost::optional<Type_t> val;
    };

    boost::ptr_map<std::string, Value_t> kv;
};

} // namespace sh28

#endif // ANYTYPEMAP_H
