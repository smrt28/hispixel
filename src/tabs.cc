#include <ctype.h>
#include <stdlib.h>
#include <set>
#include <boost/lexical_cast.hpp>

#include "tabs.h"
#include "error.h"
#include "parslet.h"

namespace s28 {

static const char * CONTEXT28_ID = "context28";

int Tabs::size() const {
    return gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));
}

Tab Tabs::at(int i) {
    GtkWidget * terminal = gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), i);
    return Tab(this, terminal, i);
}

TerminalContext * Tab::get_context() {
    if (!terminal) return nullptr;
    TerminalContext *tc = (TerminalContext *)g_object_get_data(G_OBJECT(terminal), CONTEXT28_ID);
    return tc;
}

int Tabs::current_index() const {
    return gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));
}

Tab Tab::next() const {
    return tabs->at(order + 1);
}

Tab Tab::prev() const {
    return tabs->at(order - 1);
}

std::string Tab::get_name(bool *has_name) const {
    const TerminalContext * tc = get_context();
    if (!tc || !tc->has_name()) {
        if (has_name) *has_name = false;
        if (tc) {
            return std::to_string(tc->get_id());
        }

        return "?";
    }
    if (has_name) *has_name = true;
    return tc->get_name();
}

std::string Tab::get_name_hr(bool *has_name_arg) const {
    bool has_name;
    std::string rv = get_name(&has_name);
    if (has_name_arg) *has_name_arg = has_name;
    if (!has_name) {
        return std::string("*") + rv;
    }
    return rv;
}


int Tab::get_id() const {
    const TerminalContext * tc = get_context();
    if (!tc) return -1;
    return tc->get_id();
}

void Tabs::remove(int i) {
    gtk_notebook_remove_page(GTK_NOTEBOOK(tabs), i);
}

int Tabs::index_of(GtkWidget *w) const {
    return gtk_notebook_page_num(GTK_NOTEBOOK(tabs), GTK_WIDGET(w));
}

void Tabs::remove(GtkWidget *w) {
    int i = index_of(w);
    if (i < 0) return;
    remove(i);
}

int Tab::set_order(int n) {
    if (!is_valid()) return -1;
    gtk_notebook_reorder_child(GTK_NOTEBOOK(tabs->raw()), terminal, n);
    return 0;
}

void Tab::focus() {
    if (!is_valid()) return;
    gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs->raw()), order);
}


Tab Tabs::find(const std::string &name) const {
    try {
        if (name.empty()) return Tab(const_cast<Tabs *>(this), nullptr, -1);
        if (name == "{}") return current();

        parser::Parslet_t p(name);

        if (p.first() == '{' && p.last() == '}') {
            p.next(); p.shift();
            int id = boost::lexical_cast<int>(p.str());
            for (auto t: *this) {
                if (t.get_id() == id)
                    return t;
            }
        } else {
            for (auto t: *this) {
                if (t.get_name() == name)
                    return t;
            }
        }
    } catch(const std::exception &e) {
        // ...
    }

    return Tab(const_cast<Tabs *>(this), nullptr, -1);
}

void TerminalContext::set_name(const std::string &s) {
    if (s.empty()) RAISE(COMMAND_ARG) << "empty";
    if (s.size() > 20) RAISE(COMMAND_ARG) << "too long";
    for (char c: s) {
        if (c == '{' || c == '}' || c == '*') RAISE(COMMAND_ARG) << "invalid character";
        if (isspace(c)) RAISE(COMMAND_ARG) << "space character";
        if (!isgraph(c)) RAISE(COMMAND_ARG) << "not printable";
    }
    name = s;
}

}
