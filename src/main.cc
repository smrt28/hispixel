#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <iostream>

#include "regevent.h"
#include "keyevent.h"
#include "tconfig.h"
#include "error.h"
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

    void activate(GtkApplication* app);
    void open_tab();
    void child_exited(VteTerminal *t, gint status);
    gboolean key_press_event(GtkWidget *widget, GdkEvent *event);
    void page_removed(GtkNotebook *notebook, GtkWidget *child, guint page_num);


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

    bool tabbar_visible;

    s28::TConfig_t config;
};

gboolean HisPixelApp_t::key_press_event(GtkWidget * /*widget*/,
        GdkEvent *event)
{
    typedef s28::TConfig_t::Action_t Action_t;
    Action_t ac = config.find_action(event);

    switch (ac.type) {
        case Action_t::ACTION_OPENTAB:
            open_tab();
            return TRUE;
        case Action_t::ACTION_FOCUS:
            gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), ac.data - 1);
            update_tabbar();
            return TRUE;
        case Action_t::ACTION_TOGGLE_TABBAR:
            if (gtk_widget_get_visible(label)) {
                gtk_widget_hide(GTK_WIDGET(label));
            } else {
                gtk_widget_show(GTK_WIDGET(label));
            }
            return TRUE;
        case Action_t::ACTION_FOCUS_NEXT: {
            gint n = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)) + 1;
            gint total = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));
            if (n >= total) n = 0;
            gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), n);
            update_tabbar();
            return TRUE;
            }
        case Action_t::ACTION_FOCUS_PREV: {
            gint n = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));
            gint total = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));
            if (n == 0) {
                n = total - 1;
            } else {
                n -= 1;
            }
            gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), n);
            update_tabbar();
            return TRUE;
            }

        default:
            break;
    }
    return FALSE;
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
    if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs)) == 0) {
        g_application_quit(G_APPLICATION(app));
    }
}

static void activate(GtkApplication* app, gpointer _udata)
{
    HisPixelApp_t *hispixel = (HisPixelApp_t *)_udata;
    hispixel->activate(app);
}


std::string HisPixelApp_t::tabbar_text() {
    gint n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs));
    std::ostringstream oss;
    gint c = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));
    for (int i = 0; i < n; i++) {
        if (i == c) {
            oss << "<span foreground=\"#ffffff\"";
            oss << " font_weight=\"bold\">";
            oss << "[" << i << "]";
            oss << "</span>";
        } else {
            oss << "[" << i << "]";
        }
    }
    return oss.str();
}

void HisPixelApp_t::update_tabbar() {
    if (!tabbar_visible) return;
    std::string s = tabbar_text();
    gtk_label_set_markup(GTK_LABEL(label), s.c_str());
}

void HisPixelApp_t::open_tab() {

    RegEvents_t<HisPixelApp_t> evts(this);
    GtkWidget * terminal = vte_terminal_new();

    if (config.get<bool>("allow_bold")) {
        vte_terminal_set_allow_bold(VTE_TERMINAL(terminal), TRUE);
    } else {
        vte_terminal_set_allow_bold(VTE_TERMINAL(terminal), FALSE);
    }

    std::string font_name = config.get<std::string>("term_font");

    PangoFontDescription *description =
        pango_font_description_from_string(font_name.c_str());

    pango_font_description_set_size(description,
            config.get<int>("term_font_size") * PANGO_SCALE);

    vte_terminal_set_font (VTE_TERMINAL(terminal), description);

    evts.reg_child_exited(terminal);
    evts.reg_key_press_event(terminal);

    GPid childpid;
    char *argv[2];
    argv[0] = strdup("/bin/bash");
    argv[1] = 0;

    /*int ret =*/// vte_terminal_fork_command_full
        vte_terminal_spawn_sync(VTE_TERMINAL(terminal),
            VTE_PTY_DEFAULT, /* VtePtyFlags pty_flags */
            NULL, /* const char *working_directory */
            argv, /* char **argv */
            NULL, /* char **envv */
            G_SPAWN_SEARCH_PATH,    /* GSpawnFlags spawn_flags */
            NULL, /* GSpawnChildSetupFunc child_setup */
            NULL, /* gpointer child_setup_data */
            &childpid, /* GPid *child_pid */
            NULL  /* GError **error */
            , NULL);

    ::free(argv[0]);
    int sel = gtk_notebook_append_page(GTK_NOTEBOOK(tabs), terminal, 0);
    gtk_widget_show(terminal);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), sel);
    gtk_notebook_next_page (GTK_NOTEBOOK(tabs));
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tabs), 0);
    update_tabbar();

}

void HisPixelApp_t::activate(GtkApplication* _app) {
    app = _app;
    RegEvents_t<HisPixelApp_t> evts(this);
    window = gtk_application_window_new (app);

    GdkRGBA color;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 1;

    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 400);

    tabs = gtk_notebook_new();
    evts.reg_page_removed(tabs);

    label = gtk_label_new("");
    color = config.get<GdkRGBA>("tabbar_bg_color");
    color.alpha = 1;

    gtk_widget_override_background_color(label, GTK_STATE_FLAG_NORMAL, &color);

    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment(GTK_MISC(label), 0, .5);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

    if (config.get<bool>("tabbar_on_bottom")) {
        gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
        gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
    } else {
        gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
        gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
    }

    open_tab();

    gtk_container_add (GTK_CONTAINER (window), box);

    gtk_widget_show_all(GTK_WIDGET(tabs));
    gtk_widget_show(box);
    gtk_widget_show(window);

    tabbar_visible = config.get<bool>("show_tabbar");
    if (tabbar_visible) {
        gtk_widget_show(label);
        update_tabbar();
    }
}


static std::string homedir() {
    const char *h = 0;
    if ((h = getenv("HOME")) == NULL) {
        h = getpwuid(getuid())->pw_dir;
    }
    if (!h) return "/";
    return h;
}


static std::vector<std::string> get_config_files() {
    static const char * HISPIXE_CFG = ".hispixel";
    static const char * HISPIXE_CFG2 = "hispixel.conf";
    std::string h = homedir();
    std::vector<std::string> rv;
    rv.push_back(h + "/" + HISPIXE_CFG);
    rv.push_back(h + "/" + HISPIXE_CFG2);
    rv.push_back(h + "/.config/" + HISPIXE_CFG);
    rv.push_back(h + "/.config/" + HISPIXE_CFG2);
    rv.push_back(h + "/etc/" + HISPIXE_CFG);
    rv.push_back(h + "/etc/" + HISPIXE_CFG2);
    return rv;
}


int main(int argc, char **argv, char** envp)
{
    int status;
    try {
        HisPixelApp_t hispixel(argc, argv, envp);
        hispixel.config.init_defaults();

        bool cfgok = false;

        for (auto cfile: get_config_files()) {
            if (hispixel.config.init(cfile)) {
                cfgok = true;
                break;
            }
        }

        if (!cfgok) {
            std::cerr << "err: config file not found" << std::endl;
        }

        GtkApplication *app;

        app = gtk_application_new (NULL /* application name */, G_APPLICATION_FLAGS_NONE);
        g_signal_connect (app, "activate", G_CALLBACK (activate), &hispixel);

        status = g_application_run (G_APPLICATION (app), argc, argv);
        g_object_unref (app);
    } catch(const s28::Error_t &e) {
        std::cout << "err(" << e.code() << "): " << e.what() << std::endl;
    }

    return status;
}
