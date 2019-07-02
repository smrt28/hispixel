#ifndef HISPIXELAPP_H
#define HISPIXELAPP_H

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "tconfig.h"
namespace s28 {

class HisPixelApp_t {
public:
    HisPixelApp_t(int argc, char **argv, char** envp) :
        argc(argc),
        argv(argv),
        envp(envp),
        label(0),
        tabs(0),
        window(0),
        box(0)
    {}

    ~HisPixelApp_t();

    void activate(GtkApplication* app);
    void open_tab();
    void child_exited(VteTerminal *t, gint status);
    gboolean key_press_event(GtkWidget *widget, GdkEvent *event);
    void page_removed(GtkNotebook *notebook, GtkWidget *child, guint page_num);

    std::string gtk_css();


    std::string tabbar_text();
    void update_tabbar();


    int argc;
    char **argv;
    char **envp;

    GtkApplication* app;
    GtkWidget *label;
    GtkWidget *tabs;
    GtkWidget *window;
    GtkWidget *box;
    GtkCssProvider *provider;

    bool tabbar_visible;

    s28::TConfig_t config;
};

} // namespace s28

#endif
