#include "globals.h"
#include "login/form.h"
#include "log.h"
#include "types.h"
#include <gtk/gtk.h>

volatile sig_atomic_t shutdown_requested = 0;

static void activate(GtkApplication *app, gpointer user_data) {
  login_context_t *login_ctx = g_new0(login_context_t, 1);
  login_form(login_ctx, app);
}

int main(int argc, char **argv) {
  printf("[opengritel]: ==========================================================\n");
  printf("[opengritel]: opengritel dijalankan.\n");
  printf("[opengritel]: ==========================================================\n");

  opengritel_context_t *this = (opengritel_context_t *)calloc(1, sizeof(opengritel_context_t));
  this->log_fp = NULL;
  this->current_log_filename = NULL;

  log_init(this);
  if (!this->log_fp) {
    fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
    return 1;
  }
  pthread_t cleaner_thread;
  pthread_create(&cleaner_thread, NULL, log_cleaner_thread, NULL);

  LOG_ERROR(this, "%s%s", "TEST", "Test 333");

  GtkApplication *app;
  int status;
  app = gtk_application_new("com.opengritel", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  shutdown_requested = 1;
  pthread_join(cleaner_thread, NULL);
  log_close(this);

  free(this);

  printf("[opengritel]: ==========================================================\n");
  printf("[opengritel]: opengritel selesai dijalankan.\n");
  printf("[opengritel]: ==========================================================\n");

  return status;
}
