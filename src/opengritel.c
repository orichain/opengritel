#include "login/form.h"
#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
  login_context_t *login_ctx = g_new0(login_context_t, 1);
  login_form(login_ctx, app);
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;
  app = gtk_application_new("com.opengritel", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
