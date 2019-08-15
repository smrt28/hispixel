#ifndef TABS_H
#define TABS_H

#include <gtk/gtk.h>
#include <string>
#include <boost/optional.hpp>

namespace s28 {
class TerminalContext {
    static int id_counter;
public:
    TerminalContext() {
        id_counter ++;
        id = id_counter;
    }

    int get_id() const { return id; }
    bool has_name() const {
        if (name) return true;
        return false;
    }

    std::string get_name() const {
        return *name;
    }

    void set_name(const std::string &s) {
        name = s;
    }
private:
    boost::optional<std::string> name;
    int id;
};

class Tabs;

class Tab {
public:
    Tab(Tabs * tabs, GtkWidget *terminal, int order_arg) :
        tabs(tabs),
        terminal(terminal)
    {
        if (!terminal) {
            order = -1;
        } else {
            order = order_arg;
        }
    }

    operator bool() const {
        return is_valid();
    }

    bool is_valid() const {
        if (!terminal) return false;
        if (order < 0) return false;
        return true;
    }

    bool operator==(const Tab &t) const {
        if (!t.is_valid() && !is_valid()) return true;
        if (t.index() == index() && t.tabs == tabs) return true;
        return false;
    }

    int index() const { return order; }

    TerminalContext * get_context();
    const TerminalContext * get_context() const { return const_cast<Tab *>(this)->get_context(); }

    std::string get_name() const;


    Tab next() const;

private:
    Tabs *tabs;
    GtkWidget *terminal;
    int order;
};

class Tabs {
public:
    Tabs(GtkWidget *tabs) : tabs(tabs) {}
    Tab at(int i);
    const Tab at(int i) const {
        return const_cast<Tabs *>(this)->at(i);
    }
    Tab current();
    int size();

    template<typename TAB>
    class Iterator {
        public:
        Iterator(Tab t) : t(t) {}

        TAB operator*() { return t; }
        TAB operator++() { t = t.next(); return t; }
        TAB operator->() { return t; }

        bool operator==(Iterator it) const {
            return *it == t;
        }

        bool operator!=(Iterator it) const {
            return !(*this == it);
        }
    private:
        Tab t;
    };

    typedef Iterator<const Tab> const_iterator;

    const_iterator begin() const {
        return const_iterator(at(0));
    }

    const_iterator end() const {
        return const_iterator(Tab(const_cast<Tabs *>(this), nullptr, -1));
    }

public:
    GtkWidget *tabs;
};
} // namespace s28
#endif
