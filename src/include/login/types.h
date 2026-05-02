#pragma once
#include <gtk/gtk.h>

typedef struct {
  GtkWidget *window;
  GtkWidget *entry_user;
  GtkWidget *entry_pass;
  GtkWidget *button_ok;
  GtkWidget *button_cancel;
} login_context_t;
