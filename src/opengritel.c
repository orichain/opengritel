#include <gtk/gtk.h>

static void on_button_clicked(GtkWidget *widget, gpointer data) {
  GtkEntry *entry = GTK_ENTRY(data);
  const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));

  g_print("Halo, %s!\n", text);
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *button;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Form GTK4");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
  gtk_window_set_child(GTK_WINDOW(window), box);

  label = gtk_label_new("Masukkan Nama Anda:");
  gtk_box_append(GTK_BOX(box), label);

  entry = gtk_entry_new();
  gtk_box_append(GTK_BOX(box), entry);

  button = gtk_button_new_with_label("Kirim");
  g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
  gtk_box_append(GTK_BOX(box), button);

  gtk_window_present(GTK_WINDOW(window));
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
