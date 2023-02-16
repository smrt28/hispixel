#pragma once
#include <gtk/gtk.h>
#include <vte/vte.h>

#include "config.hxx"
#include "tabs.h"
#include "regevent.h"
#include "colormanager.h"
namespace s28 {

class HisPixelApp {
public:
    HisPixelApp(int argc, char **argv, char** envp) :
        argc(argc),
        argv(argv),
        envp(envp),
		z_manager(config)
    {}

    ~HisPixelApp();

    /**
     * GTK signal hanlders
     */
    void activate(GtkApplication* app);
    void child_exited(VteTerminal *t, gint status);
    gboolean key_press_event(GtkWidget *widget, GdkEvent *event);
    gboolean key_release_event(GtkWidget *widget, GdkEvent *event);

    void page_removed(GtkNotebook *notebook, GtkWidget *child, guint page_num);
    void selection_changed(VteTerminal *t);
    Tabs get_tabs(int z=-1) {
		if (z == -1) return Tabs(tabs, z_axe);
		return Tabs(tabs, z);
	}

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
        bool focus = true;
    };

    void open_tab(TabConfig tabconfig = TabConfig());
    // togle = true if tabbar visibility changed
    void update_tabbar(bool togle = false);

	ColorManger * get_z_manager() { return &z_manager; }


    typedef s28::Config_t::Action_t Action_t;
private:
    Action_t find_action(GdkEvent *event);

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
	ColorManger z_manager; // fixme: rename class!!
    int z_axe = 0;


    // ctrl-d must be released before it can close the app
    bool close_last_fuse_enabled = false;
};

} // namespace s28
