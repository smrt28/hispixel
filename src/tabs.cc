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
    if (!tc) RAISE(FATAL) << "missing terminal context";
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
    if (has_name) *has_name = true;
    return tc->get_name();
}

int Tab::get_id() const {
    return get_context()->get_id();
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


std::string Tab::dump() {
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

void Tab::feed(const std::string &s) {
    if (!is_valid()) return;
    vte_terminal_feed_child(VTE_TERMINAL(terminal), s.c_str(), s.size());
    vte_terminal_feed_child(VTE_TERMINAL(terminal), "\n", 1);
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

}
