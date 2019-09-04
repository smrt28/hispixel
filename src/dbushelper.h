#ifndef DBUSHELPER_H
#define DBUSHELPER_H

#include "error.h"

namespace s28 {

namespace callback {
    class RouteBase {
        public:
            virtual std::string call(std::string) = 0;
    };

    template<typename FN, typename Handler>
    class Route : public RouteBase {
    public:
        virtual ~Route() {}
        FN fn;
        Handler *hispixel;
        std::string call(std::string s) {
            return (hispixel ->* fn)(s);
        }
    };

    gboolean call(HisPixelGDBUS *, GDBusMethodInvocation *invocation, const gchar *text, gpointer rawroute) {
        RouteBase *route = (RouteBase *)rawroute;
        std::string rv;
        try {
            rv = route->call(std::string(text));
        } catch(const Error_t &e) {
            std::cout << e.code() << ":" << e.what() << std::endl;
            rv = std::to_string(e.code());
        } catch(const std::exception &e) {
            std::cout << e.what() << std::endl;
            rv = "1";
        } catch(...) {
        }
        g_dbus_method_invocation_return_value (invocation, g_variant_new ("(s)", rv.c_str()));

        return TRUE;
    }

    template<typename FN, typename Handler>
    void reg(HisPixelGDBUS *interface, const char *handle, FN fn, Handler *hp) {
        Route<FN, Handler> *route = new Route<FN, Handler>();
        route->fn = fn;
        route->hispixel = hp;
        g_signal_connect(interface, handle, G_CALLBACK(call), (gpointer)route);
    }
}

}

#endif // DBUSHELPER_H
