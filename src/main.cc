
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
#include "dbushelper.h"

namespace s28 {

const char * app_name() {
    static std::string name;
    if (name.empty()) {
        std::ostringstream oss;
        oss << "com.hispixel.termapp" << getpid();
        name = oss.str();
    }
    return name.c_str();
}

namespace {

gboolean on_rpc(HisPixelGDBUS *interface, GDBusMethodInvocation *invocation,
        const gchar *greeting, gpointer _udata)
{
    HisPixelApp_t *hispixel = (HisPixelApp_t *)_udata;
    std::string s = hispixel->rpc(greeting);
    his_pixel_gdbus_complete_vte_dump(interface, invocation, s.c_str());
    return TRUE;
}

void activate(GtkApplication* app, gpointer _udata)
{
    GDBusConnection * connection = g_application_get_dbus_connection(G_APPLICATION(app));

    HisPixelGDBUS *interface;
    GError *error = nullptr;
    interface = his_pixel_gdbus_skeleton_new();

    HisPixelApp_t *hispixel = (HisPixelApp_t *)_udata;

    g_signal_connect (interface, "handle-vte-dump", G_CALLBACK (on_rpc), _udata);

    callback::reg(interface, "handle-focus", &HisPixelApp_t::focus, hispixel);
    callback::reg(interface, "handle-feed", &HisPixelApp_t::feed, hispixel);
    callback::reg(interface, "handle-set-name", &HisPixelApp_t::set_name, hispixel);

    g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (interface), connection, "/com/hispixel", &error);

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
    try {
        return s28::run(argc, argv, envp);
    } catch (...) {
        std::cout << "fatal error" << std::endl;
    }

    return 1;
}
