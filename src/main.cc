#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <vte/vte.h>

#include <iostream>

class HisPixelApp_t {
public:
    HisPixelApp_t(int argc, char **argv, char** envp) :
        argc(argc), argv(argv), envp(envp)
    {}

    int argc;
    char **argv;
    char **envp;
    GtkWidget *label;
};

static void term_exited(GtkApplication* appx, gpointer _udata) {
    GtkApplication* app = (GtkApplication*)_udata;
    std::cout << app << std::endl;
    g_application_quit(G_APPLICATION(app));
}

static gboolean keypress (GtkWidget *widget, GdkEvent *event,
        gpointer _udata)
{
    if (event->type != GDK_KEY_PRESS) return 0;
    HisPixelApp_t *hispixel = (HisPixelApp_t *)_udata;
    if (event->key.hardware_keycode == 38) {
        gtk_widget_hide(hispixel->label);
    }

    //if (event->key.hardware_keycode == 38) return 1;
    std::cout << "KEY " << event->key.hardware_keycode << std::endl;
    return 0;
}

static void activate(GtkApplication* app, gpointer _udata)
{
    std::cout << app << std::endl;
    HisPixelApp_t *hispixel = (HisPixelApp_t *)_udata;
    GtkWidget *window;

    window = gtk_application_window_new (app);

    GdkColor color;
    color.red = 0xffff;
    color.green = 0;
    color.blue = 0;

    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 400);



    GtkWidget * terminal = vte_terminal_new();
    g_signal_connect(terminal, "child-exited", G_CALLBACK(term_exited), app);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(keypress), hispixel);

    GPid childpid;

    const char * working_dir = 0;

    char *argv[2];
    argv[0] = strdup("/bin/bash");
    argv[1] = 0;

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

    ::free(argv[0]);


    GtkWidget *label = gtk_label_new("smrt");
    hispixel->label = label;

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);



    //gtk_box_set_homogeneous(GTK_BOX(box), 1);

//    gtk_container_add (GTK_CONTAINER (box), label);
    gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(box), terminal, 1, 1, 0);
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_widget_show_all (window);
}

int main(int argc, char **argv, char** envp)
{
    HisPixelApp_t hispixel(argc, argv, envp);

    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), &hispixel);



    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
