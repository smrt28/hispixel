#include <string.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <pwd.h>
#include <stdlib.h>

#include <iostream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "regevent.h"
#include "config.hxx"
#include "error.h"
#include "hispixelapp.h"
#include "envfactory.h"
#include "parslet.h"
#include "tabs.h"
#include "colormanager.h"
#include "utils.h"
//#define DEBUG_LOG_KEY_EVENTS

namespace s28 {
const char * app_name();

int TerminalContext::id_counter = 0;

namespace {

#ifdef DEBUG_LOG_KEY_EVENTS
std::string gtkkey_mask(guint m) {
    std::ostringstream oss;
    if (m & GDK_MOD1_MASK) oss << " mod1";
    if (m & GDK_MOD2_MASK) oss << " mod2";
    if (m & GDK_MOD3_MASK) oss << " mod3";
    if (m & GDK_MOD4_MASK) oss << " mod4";
    if (m & GDK_MOD5_MASK) oss << " mod5";
    if (m & GDK_CONTROL_MASK) oss << " ctl";
    if (m & GDK_SHIFT_MASK) oss << " shift";

    return oss.str();
}
#endif

static const char * CONTEXT28_ID = "context28";

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
    std::string h = homedir();
    std::vector<std::string> rv;
    rv.push_back(h + "/.config/hispixel.1"); // ~/.config directory
    rv.push_back(h + "/.config/hispixel"); // ~/.config directory
    rv.push_back(h + "/.hispixel/config");
    rv.push_back("/etc/hispixel"); // /etc/hispixel
    return rv;
}


// match GTK event and KeySym_t
bool match_gtk_ks_event(GdkEvent *event, const KeySym_t &ks) {
    static guint total_mask =
        GDK_MOD1_MASK | // alt
//      GDK_MOD2_MASK | //numlock
        GDK_MOD3_MASK |
        GDK_MOD4_MASK | // win
        GDK_MOD5_MASK |
        GDK_CONTROL_MASK | // ctrl
        GDK_SHIFT_MASK; // shift

    if (event->type != GDK_KEY_PRESS &&
        event->type != GDK_KEY_RELEASE) return false;


    guint state = event->key.state & total_mask;

    if (ks.key == 0) return false;

    if (ks.key == event->key.keyval && (total_mask & ks.mask) == state) {
        return true;
    }

    return false;
}

} // namespace


HisPixelApp::Action_t HisPixelApp::find_action(GdkEvent *event)
{
    Action_t ac;
    const s28::Config_t::KeyBindings_t &keybindings = config.get_keybindings();

    // search key bindings
    for (auto binding: keybindings) {
        if (match_gtk_ks_event(event, binding.keysym)) {
            return binding.action;
        }
    }


    return Action_t();
}


gboolean HisPixelApp::key_release_event(GtkWidget *, GdkEvent *event)
{
    Tabs tt(tabs, z_axe);
    Action_t ac = find_action(event);
    switch (ac.type) {
        case Action_t::ACTION_CLOSE_LAST:
            if (tt.empty_all_axes()) {
                close_last_fuse_enabled = false;
            }
            break;
        default:
            break;
    }
    return FALSE;
}

gboolean HisPixelApp::key_press_event(GtkWidget *, GdkEvent *event)
{
    Action_t ac = find_action(event);
    Tabs tt(tabs, z_axe);

    switch (ac.type) {
        case Action_t::ACTION_BE_FIRST:
            {
            Tab tc = tt.current();
            Tab tn = tt.at(ac.data);
            if (!tc || !tn) return TRUE;
            tc.swap(tn);
            update_tabbar();
            return TRUE;
            }
        case Action_t::ACTION_OPENTAB:
            open_tab();
            return TRUE;
        case Action_t::ACTION_FOCUS_Z:
            {
                if (ac.data > 50) { // max 50 tabs
                    break;
                }

                z_axe = ac.data - 1;
                tt.set_z_axe(z_axe);
                if (tt.size() == 0) {
                    open_tab();
                    tt.sync();
                }

                Tab t = tt.get_focus();
                if (t.is_valid()) {
                        t.focus();
                }
                update_tabbar();
                return TRUE;
            }
        case Action_t::ACTION_FOCUS:
            {
                if (ac.data > 50) { // max 50 tabs
                    break;
                }

                if (config.get<bool>("auto_open_tabs")) {
                    int ts = tt.size();
                    while (ts < ac.data) {
                        open_tab();
                        ts ++;
                    }
                }

                tt.at(ac.data - 1).focus();
                update_tabbar();
                return TRUE;
            }
        case Action_t::ACTION_FOCUS_NEXT:
            tt.current().next().focus();
            update_tabbar();
            return TRUE;
        case Action_t::ACTION_FOCUS_PREV:
            tt.current().prev().focus();
            update_tabbar();
            return TRUE;
        case Action_t::ACTION_CLOSE_LAST:
            if (tt.empty_all_axes()) {
                if (!close_last_fuse_enabled) {
                    g_application_quit(G_APPLICATION(app));
                    return TRUE;
                }
                z_axe = 0;
            }
            tt.sync();

            update_tabbar();
            return FALSE;
        case Action_t::ACTION_TOGLE_TABBAR:
            tabbar_visible = !tabbar_visible;
            update_tabbar(true);
            return TRUE;
        case Action_t::ACTION_NONE:
            return FALSE;
        default:
            break;
    }
    return FALSE;
}


void HisPixelApp::on_error(const std::exception *e) {
    if (e) {
        std::cerr << "HisPixelApp error: " << e->what() << std::endl;
    } else {
        std::cerr << "HisPixelApp general error"  << std::endl;
    }
}

void HisPixelApp::page_removed(GtkNotebook * /*notebook*/,
        GtkWidget * child, guint /*page_num*/)
{
    TerminalContext *tc = (TerminalContext *)g_object_get_data(G_OBJECT(child), CONTEXT28_ID);
    if (tc) delete tc;
    update_tabbar();
}

void HisPixelApp::child_exited(VteTerminal *t, gint /*status*/) {
    Tabs tt(tabs, z_axe);
    int i = tt.index_of(GTK_WIDGET(t));
    tt.remove(GTK_WIDGET(t));

    if (tt.empty() && tt.get_total_tabs() > 0) {
        auto all = tt.get_all_tabs(-1);
        int z = -1;
        for (auto t: all) {
            int zz = t.get_z_axe();
            if (zz < z_axe) {
                z = zz;
            }

            if (zz > z_axe) {
                z = zz;
                break;
            }
        }

        if (z != -1) {
            z_axe = z;
        }
        tt.set_z_axe(z_axe);
        tt.get_focus().focus();

        update_tabbar();
        return;
    }

    tt.sync();
    if (i >= tt.size()) {
        i--;
    }

    tt.at(i).focus();
    update_tabbar();

    // exit the app if there is no tab reminding
    if (!config.has_close_last && tt.empty()) {
        g_application_quit(G_APPLICATION(app));
    }
}


std::string HisPixelApp::tabbar_text() {
    Tabs t(tabs, z_axe);
    // get number of tabs
    gint n = t.size();
    if (n <= 0) return std::string();

    std::ostringstream oss;
  
    auto span = [this]() -> std::string {
        std::ostringstream oss;
        oss << "<span font_desc=\"" << config.get<std::string>("label_font") << "\"";
        return oss.str();
    };

    oss <<  span() << " foreground=\"#" <<
        config.get<std::string>("z_name_color") << "\"";
    oss << " font_weight=\"bold\">"; // the selected tab is bold
    oss << z_manager.z_to_name(z_axe) << " - "; // tab number
    oss << "</span>";
    for (auto tt: t) {
        std::string name = tt.get_name();
        std::string color1, color2;

        color1 = z_manager.get_z_color_light(z_axe);
        color2 = z_manager.get_z_color_dark(z_axe);

        if (tt.has_focus()) {
            oss << span() << " foreground=\"#" << color1 << "\"";
            oss << " font_weight=\"bold\">"; // the selected tab is bold
            oss << "[" << name << "]"; // tab number
            oss << "</span>";
        } else {
            oss << span() << " foreground=\"#" << color2 << "\"";
            oss << " font_weight=\"bold\">"; // the selected tab is bold
            oss << "[" << name << "]"; // tab number
            oss << "</span>";
        }
    }

    return oss.str();
}

void HisPixelApp::update_tabbar(bool togle) {
    // tabbar is hidden, update is not needed
    if (!tabbar_visible) {
        if (togle) gtk_widget_hide(label);
        return;
    }

    if (tabbar_visible && togle) {
        gtk_widget_show(label);
    }

    // get the tab-bar content
    std::string s = tabbar_text();

    // and show it
    gtk_label_set_markup(GTK_LABEL(label), s.c_str());
}


void HisPixelApp::selection_changed(VteTerminal *) {
    /*
    GdkDisplay *display = gdk_display_get_default();
    GtkClipboard *c = gtk_clipboard_get_for_display(display, GDK_SELECTION_PRIMARY);
    std::string text = gtk_clipboard_wait_for_text(c);
    std::cout << "sel: " << text << std::endl;
    */
}


void HisPixelApp::open_tab(TabConfig tabconfig) {
    close_last_fuse_enabled = true;
    Tabs tt(tabs, z_axe);
    std::unique_ptr<TerminalContext> tc(new TerminalContext(z_axe));

    GtkWidget * terminal = vte_terminal_new();

    if (!terminal) {
        RAISE(FAILED) << "vte_terminal_new failed";
    }

    SignalRegister_t sr(this);
    sr.reg_selection_changed(terminal);


    // set scrollback-limit property (50000 default)
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal),
            config.get<uint32_t>("scrollback_lines"));

    // set audible bell
    if (config.get<bool>("audible_bell")) {
        vte_terminal_set_audible_bell(VTE_TERMINAL(terminal), TRUE);
    } else {
        vte_terminal_set_audible_bell(VTE_TERMINAL(terminal), FALSE);
    }

    GdkRGBA color_palette[16];
    GdkRGBA color_fg, color_bg;

    static const char *colors[] = {
        "color_0",
        "color_1",
        "color_2",
        "color_3",
        "color_4",
        "color_5",
        "color_6",
        "color_7",
        "color_8",
        "color_9",
        "color_10",
        "color_11",
        "color_12",
        "color_13",
        "color_14",
        "color_15",
    } ;

    int gama = config.get<int>("gama");
    for (int i = 0; i < 16; i++) {
        GdkRGBA c = config.get<GdkRGBA>(colors[i]);
        utils::apply_gama(c, gama);
        color_palette[i] = c;
    }

    color_fg = config.get<GdkRGBA>("color_fg");
    color_bg = config.get<GdkRGBA>("color_bg");

    vte_terminal_set_colors(VTE_TERMINAL(terminal), &color_fg, &color_bg, color_palette, 16);
    vte_terminal_set_font (VTE_TERMINAL(terminal), font_description);

    RegEvents_t<HisPixelApp> evts(this);

    // when the terminal exits it has to be removed from tab-bar, so
    // we need to register for the child-exited signal
    evts.reg_child_exited(terminal);

    ArgsFactory argv;
    argv.add(config.get<std::string>("command"));

    ArgsFactory envv;
    envv.add("HISPIXEL_APP_ID", app_name());
    envv.add("HISPIXEL_APP_TID", std::to_string(tc->get_id()));

    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT, /* VtePtyFlags pty_flags */
        NULL, /* const char *working_directory */
        argv.build(), /* char **argv */
        envv.build(), /* char **envv */
        G_SPAWN_SEARCH_PATH,    /* GSpawnFlags spawn_flags */
        NULL, /* GSpawnChildSetupFunc child_setup */
        NULL, /* gpointer child_setup_data */
        NULL, /* GDestroyNotify child_setup_data_destroy */
        -1, /* int timeout */
        NULL, /* GCancellable *cancellable, */
        NULL, /* GPid *child_pid */
        NULL  /* GError **error */
        );

    g_object_set_data(G_OBJECT(terminal), CONTEXT28_ID, tc.get());

    int sel = gtk_notebook_append_page(GTK_NOTEBOOK(tabs), terminal, 0);
    if (sel == -1) {
        gtk_widget_destroy(terminal);
        return;
    }

    tc.release();

    gtk_widget_show(terminal);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), sel);
    gtk_notebook_next_page (GTK_NOTEBOOK(tabs));
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tabs), 0);

    gtk_widget_show(tabs);
    tt.sync();
    Tab current = tt.last();
    current.focus();


    if (tabconfig.focus)
        gtk_widget_grab_focus(terminal);
    else
        current.focus();
    update_tabbar(true);
}


std::string HisPixelApp::gtk_css() {
    // create the CSS style according to the hispixel.conf
    std::ostringstream oss;
    GdkRGBA clor = config.get<GdkRGBA>("tabbar_bg_color");
    oss << "* { "
        <<       "background-color: " << gdk_rgba_to_string (&clor)   << ";"
        <<       "border-color: black;"
        <<" }";

    return oss.str();
}


void HisPixelApp::activate(GtkApplication* theApp) {
    app = theApp;


    window = gtk_application_window_new (app);
    if (!window) RAISE(FATAL) << "gtk_application_window_new failed";

    // event weapper
    RegEvents_t<HisPixelApp> evts(this);

    // register window key_press signal callbacks
    evts.reg_key_press_event(window);
    evts.reg_key_release_event(window);

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
    gtk_window_set_default_size (GTK_WINDOW (window),
                    config.get<uint32_t>("window_width"),
                    config.get<uint32_t>("window_height"));

    // create notebook widget - the "tabbed window".
    tabs = gtk_notebook_new();
    if (!tabs) RAISE(FATAL) << "gtk_notebook_new failed";

    // register page-removed event
    evts.reg_page_removed(tabs);

    // create a helper box widget. It holds the tab-bar (if visible) and the terminals.
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    if (!box) RAISE(FATAL) << "gtk_box_new failed";

    // the tabbar is a GTK label. Crete the label...
    label = gtk_label_new("");
    if (!label) RAISE(FATAL) << "gtk_label_new failed";

    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    // put tabbar to top/bottom
    if (config.get<bool>("tabbar_on_bottom")) {
        gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
        gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
    } else {
        gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
        gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
    }

    // open the very first tab
    TabConfig tc;
    tc.focus = true;

    for (uint32_t i = 0; i < config.get<uint32_t>("startup_tabs"); ++i) {
            if (i > 50) {
                    break; // prevent a "tab-bomb"
            }
            open_tab(tc);
            tc.focus = false;
    }

    // add the box container to the window
    gtk_container_add(GTK_CONTAINER (window), box);

    // show everything
    gtk_widget_show_all(GTK_WIDGET(tabs));
    gtk_widget_show(box);
    gtk_widget_show(window);

    // update tabbar if visible
    update_tabbar();
}

HisPixelApp::~HisPixelApp() {
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

void HisPixelApp::read_config(const char *cfg_file) {
    if (args.verbose) {
        config.verbose = true;
    }
    if (cfg_file) {
            if (!config.init(cfg_file)) {
                    RAISE(NOT_FOUND) << "err: config file not found at " << cfg_file;
            }
    } else {
            // try to read config file from several possible paths
            if (!config.init(s28::get_config_files())) {
                // no config found. Print warning and continue with default config values.
                RAISE(NOT_FOUND) << "err: config file not found at ~/.hispixel/config or ~/.config/hispixel";
            }
    }

    // get and cache pango font dectiptin used by VTE
    std::string font_name = config.get<std::string>("term_font");
    font_description = pango_font_description_from_string(font_name.c_str());
    if (!font_description) RAISE(FATAL) << "pango_font_description_from_string failed:" << font_name;
    pango_font_description_set_size(font_description, config.get<int>("term_font_size") * PANGO_SCALE);
}

} // namespace s28
