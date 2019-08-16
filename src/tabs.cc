
#include <stdlib.h>
#include <set>
#include "tabs.h"

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
        return std::to_string(order + 1);;
    }
    if (has_name) *has_name = true;
    return tc->get_name();
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
    for (auto t: *this) {
        if (t.get_name() == name) return t;
    }

    return Tab(const_cast<Tabs *>(this), nullptr, -1);
}

}
