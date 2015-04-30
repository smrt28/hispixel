#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <vte/vte.h>

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
    gboolean keypress(GtkWidget *widget, GdkEvent *event);


    int argc;
    char **argv;
    char **envp;

    GtkApplication* app;
    GtkWidget *label;
    GtkWidget *tabs;
    GtkWidget *window;
    GtkWidget *box;

    s28::TConfig_t config;
};

gboolean HisPixelApp_t::keypress(GtkWidget *widget, GdkEvent *event) {
    s28::TConfig_t::Action_t ac = config.find_action(event);

    if (ac.type == s28::TConfig_t::Action_t::ACTION_OPENTAB) {
        open_tab();
        return TRUE;
    }

    if (ac.type == s28::TConfig_t::Action_t::ACTION_FOCUS) {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), ac.data - 1);
        return TRUE;
    }

    //gtk_widget_hide(GTK_WIDGET(label));
    return FALSE;
}

void HisPixelApp_t::child_exited(VteTerminal *t, gint status) {
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


void HisPixelApp_t::open_tab() {

    RegEvents_t<HisPixelApp_t> evts(this);
    GtkWidget * terminal = vte_terminal_new();

    std::string font_name = config.get_value("term_font");

    PangoFontDescription *description =
        pango_font_description_from_string(font_name.c_str());
    vte_terminal_set_font (VTE_TERMINAL(terminal), description);

    evts.reg_child_exited(terminal);
    evts.reg_key_press_event(terminal);

    GPid childpid;
    char *argv[2];
    argv[0] = strdup("/bin/bash");
    argv[1] = 0;

    const char * working_dir = 0;

    int ret = vte_terminal_fork_command_full(VTE_TERMINAL(terminal),
            VTE_PTY_DEFAULT, /* VtePtyFlags pty_flags */
            working_dir, /* const char *working_directory */
            argv, /* char **argv */
            NULL, /* char **envv */
            G_SPAWN_SEARCH_PATH,    /* GSpawnFlags spawn_flags */
            NULL, /* GSpawnChildSetupFunc child_setup */
            NULL, /* gpointer child_setup_data */
            &childpid, /* GPid *child_pid */
            NULL  /* GError **error */
            );

    working_dir = 0;

    ::free(argv[0]);
    int sel = gtk_notebook_append_page(GTK_NOTEBOOK(tabs), terminal, 0);
    std::cout << sel << std::endl;
    gtk_widget_show(terminal);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), sel);
    gtk_notebook_next_page (GTK_NOTEBOOK(tabs));
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tabs), 0);

}

void HisPixelApp_t::activate(GtkApplication* _app) {
    app = _app;
    window = gtk_application_window_new (app);

    GdkColor color;
    color.red = 0;
    color.green = 0;
    color.blue = 0;

    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 400);


    const char * working_dir = 0;


    tabs = gtk_notebook_new();
    
    label = gtk_label_new("smrt");

    color.red = 0xffff;
    gtk_widget_modify_bg(label, GTK_STATE_NORMAL, &color);


    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

    gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(box), tabs, 1, 1, 0);
    
    open_tab();

    gtk_container_add (GTK_CONTAINER (window), box);
    
    gtk_widget_show_all(GTK_WIDGET(tabs));
    gtk_widget_show(box);
    gtk_widget_show(window);
   // gtk_widget_show(label);
}

int main(int argc, char **argv, char** envp)
{
    int status;
    try {
        HisPixelApp_t hispixel(argc, argv, envp);
        hispixel.config.init_defaults();

        GtkApplication *app;

        app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
        g_signal_connect (app, "activate", G_CALLBACK (activate), &hispixel);

        status = g_application_run (G_APPLICATION (app), argc, argv);
        g_object_unref (app);
    } catch(const s28::Error_t &e) {
        std::cout << "err(" << e.code() << "): " << e.what() << std::endl;
    }

    return status;
}
