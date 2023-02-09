#ifndef TABS_H
#define TABS_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <iostream>
namespace s28 {

/**
 * Metadata hold by every terminal widget
 */
class TerminalContext {
    static int id_counter;
public:
    TerminalContext(int z_axe) : z_axe(z_axe) {
        id_counter ++;
        // TODO: handle overflow
        id = id_counter;
    }

    int get_id() const { return id; }

    int z_axe;
    int id;
    bool focus = false;
};

class Tabs;

/**
 * Tab widget wrap
 */
class Tab {
	friend class Tabs;
private:
    Tab(Tabs * tabs, GtkWidget *terminal) :
        tabs(tabs),
        terminal(terminal)
    {}
public:
    Tab() {}

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

    bool is(GtkWidget *t) const {
        return t == terminal;
    }

	int remove();

    int index() const { return order; }

    TerminalContext * get_context();
    const TerminalContext * get_context() const { return const_cast<Tab *>(this)->get_context(); }

    int get_z_axe(void) { return get_context()->z_axe; }
    std::string get_name() const;
    int get_id() const;
    Tab next() const;
    Tab prev() const;

	void swap(Tab t);
    int set_order(int n);
    void focus();
    bool has_focus() const { return get_context()->focus; }
    std::string dump() const;
    GtkWidget *raw() const { return terminal; }

private:
    Tabs *tabs = nullptr;
    GtkWidget *terminal = nullptr;
    int order = -1;
    int notebook_order = -1;
};


/**
 * Notebook widget wrap
 */
class Tabs {
	public:
		Tabs(GtkWidget *tabs, int z_axe);

		Tab at(int i);
		int index_of(GtkWidget *) const;
		int remove(GtkWidget *);
		const Tab at(int i) const { return const_cast<Tabs *>(this)->at(i); }
		int current_index() const;
		Tab current() { return at(current_index()); }
		const Tab current() const { return at(current_index()); }
		int size() const;
		bool empty() const { return size() == 0; }

		typedef std::vector<Tab>::const_iterator const_iterator;

		const_iterator begin() const {
			return ztabs.begin();
		}

		const_iterator end() const {
			return ztabs.end();
		}

		GtkWidget * raw() const { return tabs; }

		Tab find(const std::string &name) const;
		std::vector<Tab> get_all_tabs(int z=-1);

		void reset_focus() {
			for (Tab t: ztabs)
			{
				t.get_context()->focus = false;
			}
		}

		Tab get_focus() const
		{
			for (Tab t: ztabs)
			{
				if (t.get_context()->focus) {
					return t;
				}
			}
			if (!ztabs.empty()) return ztabs[0];
			return Tab();
		}


		Tab last() const {
			if (ztabs.empty()) return Tab();
			return ztabs.back();
		}

		void sync();
		void set_z_axe(int z);
		bool empty_all_axes() const { return total_tabs == 0; }
		int get_total_tabs() const { return total_tabs; }

	private:
		int total_tabs = 0;
		std::vector<Tab> ztabs;
		GtkWidget *tabs;
		int z_axe;
};
} // namespace s28
#endif
