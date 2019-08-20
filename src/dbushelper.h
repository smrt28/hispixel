#ifndef DBUSHELPER_H
#define DBUSHELPER_H

namespace s28 {

namespace callback {
    class RouteBase {
        public:
            virtual void call(std::string) = 0;

    };

    template<typename FN>
    class Route : public RouteBase {
    public:
        virtual ~Route() {}
        FN fn;
        HisPixelApp_t *hispixel;
        void call(std::string s) {
            (hispixel ->* fn)(s);
        }
    };

    gboolean i_s_call(HisPixelGDBUS *, GDBusMethodInvocation *invocation, const gchar *text, gpointer rawroute) {
        RouteBase *route = (RouteBase *)rawroute;
        int rv = 0;
        try {
            route->call(std::string(text));
        } catch(...) {
            rv = 1;
        }
        g_dbus_method_invocation_return_value (invocation, g_variant_new ("(i)", rv));

        return TRUE;
    }

    template<typename FN>
    void reg(HisPixelGDBUS *interface, const char *handle, FN fn, HisPixelApp_t *hp) {

        Route<FN> *route = new Route<FN>();
        route->fn = fn;
        route->hispixel = hp;
        g_signal_connect(interface, handle, G_CALLBACK(i_s_call), (gpointer)route);
    }
}

}

#endif // DBUSHELPER_H
