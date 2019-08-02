#ifndef SRC_REGEVENT_H
#define SRC_REGEVENT_H

#include <gtk/gtk.h>
#include <vte/vte.h>

/**
 * This is simple C-GTK event wrapper. It handles C-style GTK callbacks by
 * passing them to App_t class methods.
 *
 * The callbacks are weapped in try/catch statement and potential exception
 * would be passed to App_t::on_exit()
 *
 * see: https://developer.gnome.org/gobject/stable/gobject-Signals.html#g-signal-connect
 */

template<typename App_t>
class RegEvents_t {
public:
    RegEvents_t(App_t *app) : app(app) {}

    // reg_* methods connect GTK signal and the given method

    /**
     * Register GTK signal
     * @param w the widget
     */
    void reg_child_exited(GtkWidget * w) {
        g_signal_connect(w, "child-exited",
                G_CALLBACK(child_exited), app);
    }

    /**
     * Register GTK signal
     * @param w the widget
     */
    void reg_key_press_event(GtkWidget * w) {
        g_signal_connect(w, "key-press-event",
                G_CALLBACK(key_press_event), app);
    }

    /**
     * Register GTK signal
     * @param w the widget
     */
    void reg_page_removed(GtkWidget * w) {
        g_signal_connect(w, "page-removed",
                G_CALLBACK(page_removed), app);
    }

    void reg_selection_changed(GtkWidget * terminal) {
        g_signal_connect(terminal, "selection-changed",
                G_CALLBACK(selection_changed), app);
    }

private:

    /**
     * child_exited GTK signal callback wrap
     */
    static void child_exited(VteTerminal *t, gint status, gpointer _udata) {
        try {
            ((App_t *)_udata)->child_exited(t, status);
        } catch(const std::exception &e) {
            ((App_t *)_udata)->on_error(&e);
        } catch(...) {
            ((App_t *)_udata)->on_error(nullptr);
        }
    }

    /**
     * key_press_event GTK signal callback wrap.
     */
    static gboolean key_press_event(GtkWidget *widget, GdkEvent *event,
            gpointer _udata)
    {
        try {
            return ((App_t *)_udata)->key_press_event(widget, event);
        } catch(const std::exception &e) {
            ((App_t *)_udata)->on_error(&e);
        } catch(...) {
            ((App_t *)_udata)->on_error(nullptr);
        }

        return FALSE;
    }

    /**
     * page_removed GTK signal callback wrap
     */
    static void page_removed(GtkNotebook *notebook, GtkWidget *child,
        guint page_num, gpointer _udata)
    {
        try {
            ((App_t *)_udata)->page_removed(notebook, child, page_num);
        } catch(const std::exception &e) {
            ((App_t *)_udata)->on_error(&e);
        } catch(...) {
            ((App_t *)_udata)->on_error(nullptr);
        }
    }

    static void selection_changed(VteTerminal *t, gpointer _udata) {
         try {
            ((App_t *)_udata)->selection_changed(t);
        } catch(const std::exception &e) {
            ((App_t *)_udata)->on_error(&e);
        } catch(...) {
            ((App_t *)_udata)->on_error(nullptr);
        }
    }

    // the application class
    App_t *app;
};

#endif /* SRC_REGEVENT_H */
