#include "tabs.h"
namespace s28 {

static const char * CONTEXT28_ID = "context28";

int Tabs::size() {
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

Tab Tabs::current() {
    gint c = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));
    return at(c);
}

Tab Tab::next() {
    return tabs->at(order + 1);
}

std::string Tab::get_name() const {
    const TerminalContext * tc = get_context();
    if (!tc || !tc->has_name()) return std::to_string(order + 1);;
    return tc->get_name();
}

}
