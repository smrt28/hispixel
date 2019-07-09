#ifndef HISPIXELAPP_H
#define HISPIXELAPP_H

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "config.h"
namespace s28 {

class HisPixelApp_t {
public:
    HisPixelApp_t(int argc, char **argv, char** envp) :
        argc(argc),
        argv(argv),
        envp(envp)
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
    void read_config();
    void on_error(const std::exception *e);


private:

    int argc;
    char **argv;
    char **envp;

    GtkApplication* app = nullptr;
    GtkWidget *label = nullptr;
    GtkWidget *tabs = nullptr;
    GtkWidget *window = nullptr;
    GtkWidget *box = nullptr;
    PangoFontDescription *font_description = nullptr;
    GtkCssProvider *provider = nullptr;
    bool tabbar_visible = true;

    s28::Config_t config;
};

} // namespace s28

#endif
