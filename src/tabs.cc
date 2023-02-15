#include <ctype.h>
#include <stdlib.h>
#include <set>
#include <boost/lexical_cast.hpp>

#include <vte/vte.h>
#include <ctype.h>

#include "tabs.h"
#include "error.h"
#include "parslet.h"

namespace s28 {

static const char * CONTEXT28_ID = "context28";

void Tabs::sync()
{
        ztabs = get_all_tabs(z_axe);
}

Tabs::Tabs(GtkWidget *tabs, int z_axe) : tabs(tabs), z_axe(z_axe) {
        sync();
}

void Tabs::set_z_axe(int z) {
        z_axe = z;
        sync();
}


int Tabs::size() const {
        return int(ztabs.size());
}

Tab Tabs::at(int i) {
        if (i<0 || size_t(i) >= ztabs.size()) return Tab();
        return ztabs[i];
}

std::vector<Tab> Tabs::get_all_tabs(int z) {
        std::vector<Tab> rv;
        for (int j = 0;;++j) {
                GtkWidget * terminal = gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), j);
                if (!terminal) {
                    total_tabs = j;
                    break;
                }
                Tab t = Tab(this, terminal);
                t.order = int(rv.size());
                t.notebook_order = j;
                if (z==-1 || t.get_z_axe() == z) {
                        rv.push_back(t);
                }
        }
        return rv;
}


TerminalContext * Tab::get_context() {
    if (!terminal) return nullptr;
    TerminalContext *tc = (TerminalContext *)g_object_get_data(G_OBJECT(terminal), CONTEXT28_ID);
    if (!tc) RAISE(FATAL) << "missing terminal context";
    return tc;
}

int Tabs::current_index() const {
    Tab t;
    t = const_cast<Tabs *>(this)->get_focus();
    if (t.is_valid()) {
        return t.order;
    }
    return -1;
}

Tab Tab::next() const {
    return tabs->at(order + 1);
}

Tab Tab::prev() const {
    return tabs->at(order - 1);
}

std::string Tab::get_name() const {
    return std::to_string(order);
}

void Tab::swap(Tab t) {
    if (!*this || !t) return;
    set_order(t.notebook_order);
}

int Tab::get_id() const {
    auto context = get_context();
    if (!context) return -1;
    return context->get_id();
}

namespace {
class GOutputStreamGuard {
public:
    GOutputStreamGuard(GOutputStream * gss) : gss(gss) {}
    ~GOutputStreamGuard() {
        close();
    }

    void close() {
        if (!gss) return;
        GError *error = nullptr;
        g_output_stream_close(gss, nullptr, &error);
        gss = nullptr;
    }
    GOutputStream * gss = nullptr;
};
} // namespace


std::string Tab::dump() const {
    if (!is_valid()) RAISE(NOT_FOUND) << "invalid tab";

    GOutputStream * gss = g_memory_output_stream_new (NULL, 0, realloc, free);
    if (!gss) return std::string();

    GOutputStreamGuard guard(gss);

    if (!VTE_TERMINAL(terminal)) RAISE(NOT_FOUND) << "invalid tab; unitialized vte";
    GError *error = nullptr;
    vte_terminal_write_contents_sync(VTE_TERMINAL(terminal),
            gss, VTE_WRITE_DEFAULT, nullptr, &error);

    if (error) RAISE(FAILED) << "vte_terminal_write_contents_sync:";

    char *data = (char *)g_memory_output_stream_get_data(G_MEMORY_OUTPUT_STREAM(gss));
    size_t size = g_memory_output_stream_get_data_size(G_MEMORY_OUTPUT_STREAM(gss));

    while (size > 0 && ::isspace(data[size-1])) --size; // remove tailing space characters
    return std::string(data, size);
}


int Tabs::index_of(GtkWidget *w) const {
    for (auto t: ztabs) {
        if (t.is(w)) {
            return t.order;
        }
    }
    return -1;
}

int Tabs::remove(GtkWidget *w) {
    int i = gtk_notebook_page_num(GTK_NOTEBOOK(tabs), GTK_WIDGET(w));
    if (i < 0) return -1;
    gtk_notebook_remove_page(GTK_NOTEBOOK(tabs), i);
    sync();
    return i;
}

int Tab::set_order(int n) {
    if (!is_valid()) return -1;
    gtk_notebook_reorder_child(GTK_NOTEBOOK(tabs->raw()), terminal, n);
    return 0;
}

void Tab::focus() {
    if (!is_valid()) return;
    tabs->reset_focus();
    get_context()->focus = true;
    gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs->raw()), notebook_order);
}

Tab Tabs::find(const std::string &name) const {
    int n = stoi(name);

    if (n < 0 or n >= int(ztabs.size())) {
        return Tab();
    }

    return ztabs[n];
}

int Tab::remove() {
    if (!*this) return -1;
    int rv = tabs->remove(terminal);
    *this = Tab();
    return rv;
}

}
