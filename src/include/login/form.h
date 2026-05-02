#pragma once
#include "login/on_button_ok_clicked.h"
#include "login/types.h"
#include <gtk/gtk.h>

static inline void login_form(login_context_t *ctx, GtkApplication *app) {
  GtkBuilder *builder = gtk_builder_new_from_file("forms/login.ui");
  ctx->window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  ctx->entry_user = GTK_WIDGET(gtk_builder_get_object(builder, "entry_user"));
  ctx->entry_pass = GTK_WIDGET(gtk_builder_get_object(builder, "entry_pass"));
  ctx->button_ok = GTK_WIDGET(gtk_builder_get_object(builder, "button_ok"));
  ctx->button_cancel = GTK_WIDGET(gtk_builder_get_object(builder, "button_cancel"));
  gtk_window_set_application(GTK_WINDOW(ctx->window), app);
  gtk_window_fullscreen(GTK_WINDOW(ctx->window));
  g_signal_connect(ctx->button_ok, "clicked", G_CALLBACK(on_button_ok_clicked), ctx);
  g_signal_connect_swapped(ctx->button_cancel, "clicked", G_CALLBACK(gtk_window_destroy), ctx->window);
  gtk_window_present(GTK_WINDOW(ctx->window));
  g_object_unref(builder);
}
