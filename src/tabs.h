#ifndef TABS_H
#define TABS_H

#include <gtk/gtk.h>
#include <string>
#include <boost/optional.hpp>
#include <iterator>

namespace s28 {
/**
 * Metadata hold by every terminal widget
 */
class TerminalContext {
    static int id_counter;
public:
    TerminalContext() {
        id_counter ++;
        // TODO: handle overflow
        id = id_counter;

        name = std::to_string(id);
    }

    int get_id() const { return id; }
    bool has_name() const {
        if (name) return true;
        return false;
    }

    std::string get_name() const {
        return *name;
    }

    void set_name(const std::string &s);
private:
    boost::optional<std::string> name;
    int id;
};

class Tabs;

/**
 * Tab widget wrap
 */
class Tab {
public:
    Tab() : tabs(nullptr), terminal(nullptr), order(-1) {}
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
        if (!tabs) return false;
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
    std::string get_name(bool *has_name = nullptr) const;
    int get_id() const;

    Tab next() const;
    Tab prev() const;

    int set_order(int n);

    void focus();

    GtkWidget *raw() const { return terminal; }

private:
    Tabs *tabs;
    GtkWidget *terminal;
    int order;
};


/**
 * Notebook widget wrap
 */
class Tabs {
public:
    Tabs(GtkWidget *tabs) : tabs(tabs) {}

    Tab at(int i);
    int index_of(GtkWidget *) const;
    void remove(int i);
    void remove(GtkWidget *);
    const Tab at(int i) const { return const_cast<Tabs *>(this)->at(i); }
    int current_index() const;
    Tab current() { return at(current_index()); }
    const Tab current() const { return at(current_index()); }
    int size() const;

    bool empty() const { return size() == 0; }

    template<typename TAB>
    class Iterator : public std::iterator<std::forward_iterator_tag, TAB> {
    public:
        Iterator(Tab t) : t(t) {}
        TAB operator*() { return t; }
        TAB operator->() { return t; }
        Iterator operator++() { t = t.next(); return *this; }
        Iterator operator++(int) { Tab rv = t; t = t.next(); return Iterator(rv); }
        bool operator==(Iterator it) const { return *it == t; }
        bool operator!=(Iterator it) const { return !(*this == it); }
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

    GtkWidget * raw() const { return tabs; }

    Tab find(const std::string &name) const;

    std::string suggest_tab_name() const;
public:
    GtkWidget *tabs;
};
} // namespace s28
#endif
