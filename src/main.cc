
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/program_options.hpp>
#include <iostream>

#include "regevent.h"
#include "config.hxx"
#include "error.h"
#include "hispixelapp.h"
#include "hisbus.h"
#include "dbushelper.h"
#include "dbushandler.h"

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

int PLOCK[2] = { -1, -1 };
typedef std::pair<HisPixelApp *, DbusHandler *> UserData;

void startup(GApplication *, gpointer) {
    if (PLOCK[1] != -1) {
        if (write(PLOCK[1], " ", 1) != 1) {
            // ... can't do anything here
        }
        ::close(PLOCK[1]);
        PLOCK[1] = -1;
    }
}

void activate(GtkApplication* app, gpointer _udata)
{
    GDBusConnection * connection = g_application_get_dbus_connection(G_APPLICATION(app));

    HisPixelGDBUS *interface;
    GError *error = nullptr;
    interface = his_pixel_gdbus_skeleton_new();

    UserData *udata = (UserData *)_udata;

    HisPixelApp *hispixel = udata->first;
    DbusHandler *dbhandler = udata->second;

    // register his-* callbacks
    callback::reg(interface, "handle-vte-dump", &DbusHandler::dump, dbhandler);
    callback::reg(interface, "handle-focus", &DbusHandler::focus, dbhandler);
    callback::reg(interface, "handle-feed", &DbusHandler::feed, dbhandler);
    callback::reg(interface, "handle-open-tab", &DbusHandler::opentab, dbhandler);
    callback::reg(interface, "handle-set-name", &DbusHandler::rename, dbhandler);

    g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (interface), connection, "/com/hispixel", &error);

    // activate the app
    hispixel->activate(app);
}


int run(int argc, char **argv, char** envp, const char *cfg_file)
{
    int status = 0;
    try {
        HisPixelApp hispixel(argc, argv, envp);
        DbusHandler dbushelper(hispixel);

        hispixel.read_config(cfg_file);

        // create GTK application
        GtkApplication *app;
        app = gtk_application_new (app_name(), G_APPLICATION_FLAGS_NONE);
        if (!app) {
            RAISE(FATAL) << "unable to create GTK application instance";
        }

        g_signal_connect(app, "startup", G_CALLBACK(startup), nullptr);

        UserData userdata(&hispixel, &dbushelper);

        // connect the app and the callbacks
        if (g_signal_connect(app, "activate", G_CALLBACK (activate), &userdata) <= 0) {
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

int daemonise() {
    // Fork, allowing the parent process to terminate.
    pid_t pid = fork();
    if (pid != 0) {
        return pid;
    }

    // Start a new session for the daemon.
   setsid();

    // Fork again, allowing the parent process to terminate.
    signal(SIGHUP, SIG_IGN);
    pid=fork();
    if (pid != 0) {
        _exit(0);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return pid;
}


namespace s28 {
        void dump_default_config();
}

int main(int argc, char **argv, char** envp)
{
    using namespace boost::program_options;
    std::string cfg_file;
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help screen")
        ("daemonize,d", "Run in background")
        ("dump-default-config,D", "Dump default config")
        ("config-file", value<std::string>(&cfg_file), "path to the config file")
        ;

    s28::app_name();

    try {
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        if (vm.count("config-file")) {
                cfg_file = vm["config-file"].as<std::string>();
        }
        const char *c = nullptr;
        if (!cfg_file.empty()) {
                c = cfg_file.c_str();
        }

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 1;
        }

        if (vm.count("dump-default-config")) {
                s28::dump_default_config();
                return 0;
        }

        if (vm.count("daemonize")) {
            if (pipe(s28::PLOCK) == -1) {
                RAISE(FATAL) << "pipe call failed";
            }
            if (daemonise()) {
                char buf[1];
                ::close(s28::PLOCK[1]);
                if (read(s28::PLOCK[0], buf, 1) != 1) {
                    std::cerr << "warn: read from pipe failed" << std::endl;
                }
                ::close(s28::PLOCK[0]);
                std::cout << s28::app_name() << std::endl;
                return 0;
            } else {
                ::close(s28::PLOCK[0]);
                s28::PLOCK[0] = -1;
                return s28::run(1, argv, envp, c);
            }
        } else {
            return s28::run(1, argv, envp, c);
        }
    } catch (...) {
        std::cout << "fatal error" << std::endl;
    }

    return 1;
}
