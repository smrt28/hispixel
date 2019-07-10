#ifndef ANYTYPEMAP_H
#define ANYTYPEMAP_H

#include <string>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/optional.hpp>

#include "error.h"
#include "valuecast.h"

namespace s28 {

/**
 * AnyTypeMap holds the key-value map string->any. The value is always
 * set by a string where the string is value_cast'ed to the particular type - see valuecast.h.
 *
 * m.set<int>("a", 1);
 *
 * m.get<int>("a"); // return 1
 * m.get<std::string>("a"); // throws since there "a" contains integer type
 * m.set<int>("a", "x"); // throws, x is supposed to be a decmal number
 * 
 * // You dont neet'd to know the type if the type has been already set:
 * AnyTypeMap_t::Value_t *v = m.find("a");
 * a->set("10");
 *
 * m.find("b") // return nullptr since there was no value set for the key "b"
 */

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
            if (!res) RAISE(FATAL) << "invalid value type";
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
            RAISE(NOT_FOUND) << "key not found: " << key;
        }
        throw __PRETTY_FUNCTION__; // not reachable, avoids compiler warning
    }

    /**
     * Set value and type for the key
     * @param key key
     */
    template<typename Type_t>
    void set(std::string key, std::string val) {
        kv.erase(key);
        std::unique_ptr<ValueImpl_t<Type_t> > bv(new ValueImpl_t<Type_t>());
        bv->set(val);
        kv.insert(key, bv.release());
    }

    /**
     * Set type for the key
     * @param key key
     */
    template<typename Type_t>
    void set(std::string key) {
        kv.erase(key);
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
    // Value implementation for given type
    template<typename Type_t>
    class ValueImpl_t : public Value_t {
    public:
        /**
         * Set value by string. Throws if the sting has wrong format.
         * @param s the value to be parsed
         */
        void set(const std::string &s) override {
            val = value_cast<Type_t>(s);
        }

        /**
         * @return the wrapped value
         */
        const Type_t & get() const {
            // ensure the value is set
            if (!val) RAISE(NOT_FOUND);
            return val.value();
        }
    private:
        boost::optional<Type_t> val; // the value
    };

    boost::ptr_map<std::string, Value_t> kv;
};

} // namespace sh28

#endif // ANYTYPEMAP_H
