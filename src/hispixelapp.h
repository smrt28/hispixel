#ifndef HISPIXELAPP_H
#define HISPIXELAPP_H

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "config.hxx"
namespace s28 {

class HisPixelApp_t {
public:
    HisPixelApp_t(int argc, char **argv, char** envp) :
        argc(argc),
        argv(argv),
        envp(envp)
    {}

    ~HisPixelApp_t();

    /**
     * GTK signal hanlders
     */
    void activate(GtkApplication* app);
    void child_exited(VteTerminal *t, gint status);
    gboolean key_press_event(GtkWidget *widget, GdkEvent *event);
    void page_removed(GtkNotebook *notebook, GtkWidget *child, guint page_num);
    void selection_changed(VteTerminal *t);


    /**
     * Handle exception thrown in the GTK signal handler
     */
    void on_error(const std::exception *e);

    /**
     * Read and parse config file
     */
    void read_config();


private:
    typedef RegEvents_t<HisPixelApp_t> SignalRegister_t;

    void update_tabbar();
    std::string gtk_css();
    std::string tabbar_text();
    void open_tab();

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

    // Config map
    s28::Config_t config;
};

} // namespace s28

#endif
