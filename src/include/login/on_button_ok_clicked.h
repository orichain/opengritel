#pragma once
#include "login/types.h"
#include <gtk/gtk.h>

static inline void on_button_ok_clicked(GtkButton *button, gpointer user_data) {
  login_context_t *ctx = (login_context_t *)user_data;
  if (ctx == NULL || ctx->entry_user == NULL) return;
  GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(ctx->entry_user));
  const char *text = gtk_entry_buffer_get_text(buffer);
  g_print("User: %s\n", text);
}
