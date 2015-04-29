#ifndef SRC_REGEVENT_H
#define SRC_REGEVENT_H

#include <gtk/gtk.h>


template<typename App_t>
class RegEvents_t {    
public:
    RegEvents_t(App_t *app) : app(app) {}

    void reg_child_exited(GtkWidget * w) {
        g_signal_connect(w, "child-exited", G_CALLBACK(child_exited), app);
    }


    void reg_key_press_event(GtkWidget * w) {
        g_signal_connect(w, "key-press-event", G_CALLBACK(keypress), app);
    }

private:

    static void child_exited(VteTerminal *t, gint status, gpointer _udata) {
        ((App_t *)_udata)->child_exited(t, status);
    }
    
    static gboolean keypress(GtkWidget *widget, GdkEvent *event,
            gpointer _udata)
    {
        return ((App_t *)_udata)->keypress(widget, event);
    }

    App_t *app;
};

#endif /* SRC_REGEVENT_H */
