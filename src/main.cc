
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "regevent.h"
#include "config.hxx"
#include "error.h"
#include "hispixelapp.h"
#include "hisbus.h"

namespace s28 {
namespace {

/*
static void signal_cb(GDBusConnection *connection,
        const gchar *sender_name,const gchar *object_path,
        const gchar *interface_name,const gchar *signal_name,
        GVariant *parameters,gpointer user_data)
{
    printf("%s: %s.%s %s\n",object_path,interface_name,signal_name,
        g_variant_print(parameters,TRUE));
}

*/

const char * app_name() {
    static std::string name;
    if (name.empty()) {
        std::ostringstream oss;
        oss << "com.hispixel.termapp" << getpid();
        name = oss.str();
    }
    return name.c_str();
}

gboolean on_rpc(HisPixelGDBUS *interface, GDBusMethodInvocation *invocation,
        const gchar *greeting, gpointer user_data)
{
    std::cout << "hi" << std::endl;
    return TRUE;
}

void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    HisPixelGDBUS *interface;
    GError *error = nullptr;
    interface = his_pixel_gdbus_skeleton_new();
    g_signal_connect (interface, "handle-rpc", G_CALLBACK (on_rpc), NULL);

    g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (interface), connection,
            std::string(app_name()).c_str(), &error);


    error = nullptr;
}


void activate(GtkApplication* app, gpointer _udata)
{
    GDBusConnection * connection = g_application_get_dbus_connection(G_APPLICATION(app));

    HisPixelGDBUS *interface;
    GError *error = nullptr;
    interface = his_pixel_gdbus_skeleton_new();
    g_signal_connect (interface, "handle-rpc", G_CALLBACK (on_rpc), NULL);
    g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (interface), connection, "/com/hispixel", &error);

    //g_bus_own_name(G_BUS_TYPE_SESSION, app_name(), G_BUS_NAME_OWNER_FLAGS_NONE, NULL,
    //        on_name_acquired, NULL, NULL, NULL);


    HisPixelApp_t *hispixel = (HisPixelApp_t *)_udata;
    hispixel->activate(app);
}


int run(int argc, char **argv, char** envp)
{
    int status;
    try {
        HisPixelApp_t hispixel(argc, argv, envp);
        hispixel.read_config();

        // create GTK application
        GtkApplication *app;
        app = gtk_application_new (app_name(), G_APPLICATION_FLAGS_NONE);
        if (!app) {
            RAISE(FATAL) << "unable to create GTK application instance";
        }

        // connect the app and the callbacks
        if (g_signal_connect(app, "activate", G_CALLBACK (activate), &hispixel) <= 0) {
            RAISE(FATAL) << "g_signal_connect failed";
        }



        // run GTK application
        status = g_application_run (G_APPLICATION (app), argc, argv);

        // release the app
        g_object_unref (app);
    } catch (const s28::Error_t &e) {
        std::cout << "err(" << e.code() << "): " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cout << "err: " << e.what() << std::endl;
    }

    return status;
}
} // namespace
} // namespace s28



int main(int argc, char **argv, char** envp)
{
    /*
    if (argc == 2) {
        GtkApplication *app;
        app = gtk_application_new (argv[1], G_APPLICATION_HANDLES_COMMAND_LINE);
        g_signal_connect(app, "activate", G_CALLBACK (activate2), nullptr);
        g_application_run (G_APPLICATION (app), 0, 0);

        return 0;
    }
*/
    std::cout << s28::app_name() << std::endl;

    try {
        return s28::run(argc, argv, envp);
    } catch (...) {
        std::cout << "fatal error" << std::endl;
    }

    return 1;
}
