#ifndef TABS_H
#define TABS_H

#include <gtk/gtk.h>
#include <string>
#include <boost/optional.hpp>
#include <iterator>
#include <vector>

namespace s28 {

struct TerminalCtl {
};

/**
 * Metadata hold by every terminal widget
 */
class TerminalContext {
    static int id_counter;
public:
    TerminalContext(TerminalCtl *tctl, int z_axe) : tctl(tctl), z_axe(z_axe) {
        id_counter ++;
        // TODO: handle overflow
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

    void set_name(const std::string &s);

    TerminalCtl * tctl;
    int z_axe;
    boost::optional<std::string> name;
    int id;
    bool focus = false;
};

class Tabs;

/**
 * Tab widget wrap
 */
class Tab {
public:
    Tab() : tabs(nullptr), terminal(nullptr), order(-1) {}
    Tab(Tabs * tabs, GtkWidget *terminal) :
        tabs(tabs),
        terminal(terminal)
    {
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

    bool is(GtkWidget *t) {
        return t == terminal;
    }

    int index() const { return order; }

    TerminalContext * get_context();
    const TerminalContext * get_context() const { return const_cast<Tab *>(this)->get_context(); }

    int get_z_axe(void) { return get_context()->z_axe; }
    std::string get_name(bool *has_name = nullptr) const;

    int get_id() const;

    Tab next() const;
    Tab prev() const;

    void set_name(const std::string &s);
    int set_order(int n);

    void focus();

    bool has_focus() { return get_context()->focus; }
    void feed(const std::string &s);
    std::string dump();

    GtkWidget *raw() const { return terminal; }

    int notebook_order = -1;
    Tabs *tabs = nullptr;
    GtkWidget *terminal = nullptr;
    int order = -1;
};


/**
 * Notebook widget wrap
 */
class Tabs {
public:
    Tabs(GtkWidget *tabs, int z_axe);

    Tab at(int i);
    int index_of(GtkWidget *) const;
    int remove(int i);
    int remove(GtkWidget *);
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
        return const_iterator(Tab(const_cast<Tabs *>(this), nullptr));
    }

    GtkWidget * raw() const { return tabs; }

    Tab find(const std::string &name) const;

    std::string suggest_tab_name() const;

    std::vector<Tab> get_all_tabs(int z=-1);

    void reset_focus() {
            for (Tab t: ztabs)
            {
                    t.get_context()->focus = false;
            }
    }

    Tab get_focus()
    {
            for (Tab t: ztabs)
            {
                    if (t.get_context()->focus) {
                            return t;
                    }
            }
            if (!ztabs.empty()) return ztabs[0];
            return Tab(this, nullptr);
    }

    void sync();

    Tab last() {
            if (ztabs.empty()) return Tab(this, nullptr);
            return ztabs.back();

    }

    void set_z_axe(int z);

    bool empty_all_axes() { return total_tabs == 0; }

    int get_total_tabs() { return total_tabs; }

private:
    int total_tabs = 0;
    std::vector<Tab> ztabs;
    GtkWidget *tabs;
    int z_axe;
};
} // namespace s28
#endif
