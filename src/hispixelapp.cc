#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <iostream>

#include "regevent.h"
#include "config.h"
#include "error.h"
#include "hispixelapp.h"

namespace s28 {

namespace {

// recognize the user home directory
std::string homedir() {
    const char *h = nullptr;
    if ((h = getenv("HOME")) == NULL) {
        struct passwd *pwd = getpwuid(getuid());
        if (pwd) h = pwd->pw_dir;
    }
    if (!h) RAISE(FATAL) << "unknown users home directory";
    return h;
}


// returns list of possible hispixel.conf file placements
std::vector<std::string> get_config_files() {
    static const char * HISPIXE_CFG = ".hispixel";
    static const char * HISPIXE_CFG2 = "hispixel.conf";
    std::string h = homedir();

    std::vector<std::string> rv;
    rv.push_back(h + "/" + HISPIXE_CFG); // ~ directory
    rv.push_back(h + "/" + HISPIXE_CFG2);
    rv.push_back(h + "/.config/" + HISPIXE_CFG); // ~/.config directory
    rv.push_back(h + "/.config/" + HISPIXE_CFG2);
    rv.push_back(std::string("/etc/") + HISPIXE_CFG); // /etc directory
    rv.push_back(std::string("/etc/") + HISPIXE_CFG2);
    return rv;
}


// matck GTK event and KeySym_t
bool match_gtk_ks_event(GdkEvent *event, const KeySym_t &ks) {
    if (event->type != GDK_KEY_PRESS &&
        event->type != GDK_KEY_RELEASE) return false;

    if (ks.key == 0) return false;
    if (ks.key == event->key.keyval && (event->key.state & ks.mask) == ks.mask)
        return true;

    return false;
}

} // namespace

gboolean HisPixelApp_t::key_press_event(GtkWidget *, GdkEvent *event)
{
    typedef s28::Config_t::Action_t Action_t;
    Action_t ac;

    const s28::Config_t::KeyBindings_t &keybindings = config.get_keybindings();

    // search key bindings
    for (auto binding: keybindings) {
        if (match_gtk_ks_event(event, binding.keysym)) {
            ac = binding.action;
            break;
        }
    }

    switch (ac.type) {
        case Action_t::ACTION_OPENTAB:
            open_tab();
            return TRUE;
        case Action_t::ACTION_FOCUS:
            // switching tabs...
            gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), ac.data - 1);
            update_tabbar();
            return TRUE;
        case Action_t::ACTION_FOCUS_NEXT: {
            // get current tab index + 1
            gint n = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)) + 1;

            // get total number of tabs
            gint total = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));

            // switch from the last tab to the first
            if (n >= total) n = 0;
            gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), n);

            // update tabbar
            update_tabbar();
            return TRUE;
            }
        case Action_t::ACTION_FOCUS_PREV: {
            // get current tab
            gint n = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));

            // get total number of tabs
            gint total = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));

            if (n == 0) {
                // jump form the first to the last
                n = total - 1;
            } else {
                // jump to the next tab
                n -= 1;
            }

            // switch tabs
            gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), n);

            // and update tabbar
            update_tabbar();
            return TRUE;
            }
            break;
        case Action_t::ACTION_CLOSE_LAST: {
            // If there is a key binding for "close_last" defined, HisPixel doesn't exit
            // with the last tab, but it waits for pressing the "close_last" key to confirm exit.
            gint n = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)) + 1;
            if (n == 0) g_application_quit(G_APPLICATION(app));
            return FALSE;
            }
        case Action_t::ACTION_NONE:
            return FALSE;
        default:
            break;
    }
    return FALSE;
}


void HisPixelApp_t::on_error(const std::exception *e) {
    if (e) {
        std::cerr << "HisPixelApp error: " << e->what() << std::endl;
    } else {
        std::cerr << "HisPixelApp general error"  << std::endl;
    }

    // Is it a good idea to force exit here? Probably not...
    // g_application_quit(G_APPLICATION(app)); 
}

void HisPixelApp_t::page_removed(GtkNotebook * /*notebook*/,
        GtkWidget * /*child*/, guint /*page_num*/)
{
    update_tabbar();
}

void HisPixelApp_t::child_exited(VteTerminal *t, gint /*status*/) {
    gint n = gtk_notebook_page_num(GTK_NOTEBOOK(tabs), GTK_WIDGET(t));
    if (n < 0) return;
    gtk_notebook_remove_page(GTK_NOTEBOOK(tabs), n);
    if (!config.has_close_last && gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs)) == 0) {
        g_application_quit(G_APPLICATION(app));
    }
}


std::string HisPixelApp_t::tabbar_text() {
    // get number of tabs
    gint n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));
    if (n <= 0) return std::string();

    // selected tab number
    gint c = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));

    std::ostringstream oss;
    for (int i = 0; i < n; i++) {
        if (i == c) {
            oss << "<span foreground=\"#ffffff\"";
            oss << " font_weight=\"bold\">"; // the selected tab is bold
            oss << "[" << i << "]"; // tab number
            oss << "</span>";
        } else {
            oss << "[" << i << "]";
        }
    }
    return oss.str();
}

void HisPixelApp_t::update_tabbar() {
    // tabbar is not visible, update is not needed
    if (!tabbar_visible) return;

    // get the tab-bar content
    std::string s = tabbar_text();

    // and show it
    gtk_label_set_markup(GTK_LABEL(label), s.c_str());
}



void HisPixelApp_t::open_tab() {

    GtkWidget * terminal = vte_terminal_new();

    if (!terminal) {
        RAISE(FAILED) << "vte_terminal_new failed";
    }

    if (config.get<bool>("allow_bold")) {
        vte_terminal_set_allow_bold(VTE_TERMINAL(terminal), TRUE);
    } else {
        vte_terminal_set_allow_bold(VTE_TERMINAL(terminal), FALSE);
    }

    vte_terminal_set_font (VTE_TERMINAL(terminal), font_description);

    RegEvents_t<HisPixelApp_t> evts(this);

    // when the terminal exits it has to be removed from tab-bar, so
    // we need to register for the child-exited signal
    evts.reg_child_exited(terminal);

    // Not sure if this is needed. Maybe const_cast would be sufficiend here...
    char *argv[2];
    argv[0] = strdup("/bin/bash"); // TODO: get shell be getpwuid()
    argv[1] = 0;

    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT, /* VtePtyFlags pty_flags */
        NULL, /* const char *working_directory */
        argv, /* char **argv */
        NULL, /* char **envv */
        G_SPAWN_SEARCH_PATH,    /* GSpawnFlags spawn_flags */
        NULL, /* GSpawnChildSetupFunc child_setup */
        NULL, /* gpointer child_setup_data */
        NULL, /* GDestroyNotify child_setup_data_destroy */
        -1, /* int timeout */
        NULL, /* GCancellable *cancellable, */
        NULL, /* GPid *child_pid */
        NULL  /* GError **error */
        );

    // vte_terminal_spawn_async doesn't thow, so this is safe
    ::free(argv[0]);

    int sel = gtk_notebook_append_page(GTK_NOTEBOOK(tabs), terminal, 0);
    gtk_widget_show(terminal);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), sel);
    gtk_notebook_next_page (GTK_NOTEBOOK(tabs));
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tabs), 0);
    update_tabbar();

    gtk_widget_grab_focus(terminal);
}


std::string HisPixelApp_t::gtk_css() {
    // create the CSS style according to the hispixel.conf
    std::ostringstream oss;
    GdkRGBA clor = config.get<GdkRGBA>("tabbar_bg_color");
    oss << "* { "
        <<       "background-color: " << gdk_rgba_to_string (&clor)   << ";"
        <<       "border-color: black;"
        <<" }";

    return oss.str();
}


void HisPixelApp_t::activate(GtkApplication* theApp) {
    app = theApp;


    window = gtk_application_window_new (app);
    if (!window) RAISE(FATAL) << "gtk_application_window_new failed";

    // event weapper
    RegEvents_t<HisPixelApp_t> evts(this);

    // register window key_press signal callbacks
    evts.reg_key_press_event(window);

    // GTK3 uses CSS styling. Create the CSS for the app.
    // create CSS provider, assign the CSS to the screen
    GdkDisplay *display = gdk_display_get_default();
    if (!display) RAISE(FATAL) << "gdk_display_get_default failed";

    GdkScreen *screen = gdk_display_get_default_screen(display);
    if (!screen) RAISE(FATAL) << "gdk_display_get_default_screen failed";

    std::string css = gtk_css();

    provider = gtk_css_provider_new();
    if (!provider) RAISE(FATAL) << "gtk_css_provider_new failed";

    GError *error = nullptr;
    gtk_css_provider_load_from_data(provider, css.c_str(), css.size(), &error);

    if (error) RAISE(FATAL) << "invalid GTK CSS";

    gtk_style_context_add_provider_for_screen (screen,
                       GTK_STYLE_PROVIDER(provider),
                       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // the window title is allways "HisPixel"
    gtk_window_set_title (GTK_WINDOW (window), "HisPixel");

    // Don't care in i3 WM
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 400);

    // create notebook widget - the "tabbed window".
    tabs = gtk_notebook_new();
    if (!tabs) RAISE(FATAL) << "gtk_notebook_new failed";

    // register page-removed event
    evts.reg_page_removed(tabs);

    // create a helper box widget. It holds the tab-bar (if visible) and the terminals.
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    if (!box) RAISE(FATAL) << "gtk_box_new failed";

    if (tabbar_visible) {
        // the tabbar is a GTK label. Crete the label...
        label = gtk_label_new("");
        if (!label) RAISE(FATAL) << "gtk_label_new failed";

        gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
        gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
        gtk_widget_set_halign(label, GTK_ALIGN_START);

        // ...and put it to top/bottom
        if (config.get<bool>("tabbar_on_bottom")) {
            gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
            gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
        } else {
            gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
            gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
        }

        // ...and show it
        gtk_widget_show(label);
    } else {
        // ...show the box only
        gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
    }

    // open the very first tab
    open_tab();

    // add the box container to the window
    gtk_container_add(GTK_CONTAINER (window), box);

    // show everything
    gtk_widget_show_all(GTK_WIDGET(tabs));
    gtk_widget_show(box);
    gtk_widget_show(window);

    // update tabbar if visible
    update_tabbar();
}

HisPixelApp_t::~HisPixelApp_t() {
    if (window) {
        gtk_widget_hide(window);
        gtk_widget_destroy(window);
        window = nullptr;
    }

    // In very rare case of error when the widgets are not connected to
    // the window yet, it could be nice to release them explicitelly as well.
    // Not sure how to do that...

    if (font_description) {
        pango_font_description_free(font_description);
    }
}

void HisPixelApp_t::read_config() {
    // try to read config file from several possible paths
    if (!config.init(s28::get_config_files())) {
        // no config found. Print warning and continue with default config values.
        std::cerr << "err: config file not found" << std::endl;
    }

    tabbar_visible = config.get<bool>("show_tabbar");

    // get and cache pango font dectiptin used by VTE
    std::string font_name = config.get<std::string>("term_font");
    font_description = pango_font_description_from_string(font_name.c_str());
    if (!font_description) RAISE(FATAL) << "pango_font_description_from_string failed:" << font_name;
    pango_font_description_set_size(font_description, config.get<int>("term_font_size") * PANGO_SCALE);
}

} // namespace s28


