#include <vte/vte.h>

#include "pwm4term.hpp"

static gboolean on_title_changed(GtkWidget *terminal, gpointer user_data) {
    GtkWindow *window = static_cast<GtkWindow*>(user_data);
    gtk_window_set_title(window, vte_terminal_get_window_title(VTE_TERMINAL(terminal)) ? : "Terminal");
    return TRUE;
}


int main(int argc, char **argv) {    
    gtk_init(&argc, &argv);
    
    Pwm4Term pwm4term;
    GtkWidget *terminal = vte_terminal_new();
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "pwm4term");
    
    gchar **envp = g_get_environ();
    gchar *command[2] = {g_strdup(g_environ_getenv(envp, "SHELL")), NULL};
    
    g_strfreev(envp);
    vte_terminal_spawn_async(
        VTE_TERMINAL(terminal),    // VteTerminal *terminal
        VTE_PTY_DEFAULT,           // VtePtyFlags pty_flags
        NULL,                      // const char *working_directory
        command,                   // char **argv
        NULL,                      // char **envv
        (GSpawnFlags) 0,           // GSpawnFlags spawn_flags_
        NULL,                      // GSpawnChildSetupFunc child_setup
        NULL,                      // gpointer child_setup_data
        NULL,                      // GDestroyNotify child_setup_data_destroy
        -1,                        // int timeout
        NULL,                      // GCancellable *cancellable
        NULL,                      // VteTerminalSpawnAsyncCallback callback
        NULL                       // gpointer user_data
    );
    
    g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
    g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);
    g_signal_connect(terminal, "window-title-changed", G_CALLBACK(on_title_changed), GTK_WINDOW(window));
    g_signal_connect(terminal, "key_press_event", G_CALLBACK(Pwm4Term::key_handler), &pwm4term);
    
    gtk_container_add(GTK_CONTAINER(window), terminal);
    gtk_widget_show_all(window);
    gtk_main();
}
