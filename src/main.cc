
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>

#include <iostream>

#include "regevent.h"
#include "config.h"
#include "error.h"
#include "hispixelapp.h"

namespace s28 {
namespace {

void activate(GtkApplication* app, gpointer _udata)
{
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
        app = gtk_application_new (NULL /* application name */, G_APPLICATION_FLAGS_NONE);
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
