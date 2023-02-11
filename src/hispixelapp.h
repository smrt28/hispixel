#ifndef HISPIXELAPP_H
#define HISPIXELAPP_H

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "config.hxx"
#include "tabs.h"
#include "regevent.h"
namespace s28 {

class HisPixelApp {
public:
    HisPixelApp(int argc, char **argv, char** envp) :
        argc(argc),
        argv(argv),
        envp(envp)
    {}

    ~HisPixelApp();

    /**
     * GTK signal hanlders
     */
    void activate(GtkApplication* app);
    void child_exited(VteTerminal *t, gint status);
    gboolean key_press_event(GtkWidget *widget, GdkEvent *event);
    void page_removed(GtkNotebook *notebook, GtkWidget *child, guint page_num);
    void selection_changed(VteTerminal *t);
    Tabs get_tabs() { return Tabs(tabs, z_axe); }

    /**
     * Handle exception thrown in the GTK signal handler
     */
    void on_error(const std::exception *e);

    /**
     * Read and parse config file
     */
    void read_config(const char *cfg_file = nullptr);

    struct TabConfig {
        TabConfig() {}
        TabConfig(const std::string &name) : name(name) {}

        boost::optional<std::string> name;
        bool focus = true;
    };

    void open_tab(TabConfig tabconfig = TabConfig());
    // togle = true if tabbar visibility changed
    void update_tabbar(bool togle = false);

private:
    typedef RegEvents_t<HisPixelApp> SignalRegister_t;

    std::string gtk_css();
    std::string tabbar_text();


    // main arguments (not needed yet)
    int argc;
    char **argv;
    char **envp;

    // GTK resources
    GtkApplication* app = nullptr;
    GtkWidget *label = nullptr;
    GtkWidget *tabs = nullptr;
    GtkWidget *window = nullptr;
    GtkWidget *box = nullptr;
    PangoFontDescription *font_description = nullptr;
    GtkCssProvider *provider = nullptr;

    bool tabbar_visible = true;
    bool tabbar_need_update = true;

    // Config map
    s28::Config_t config;
    TerminalCtl tctl;
    int z_axe = 0;
};

} // namespace s28

#endif
