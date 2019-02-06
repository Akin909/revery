#ifdef __linux__
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

// The callback to g_signal_connect MUST be an `activate` function
static void activate(GtkApplication *app, const char *user_data) {
  GtkWidget *dialog;

  GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  dialog = gtk_message_dialog_new(NULL, flags, GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_CLOSE, user_data);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

static void create_gtk_window(GtkWidget *widget, gpointer user) {
  gtk_widget_set_window(widget, (GdkWindow *)user);
}

void revery_alert_gtk(void *pWin, const char *szMessage) {
  /*
   * TODO:
   * 1. figure out how to convert the pointer from an X11 window handle
   * to a GTK window, see (for inspiration):
   * https://gist.github.com/mmozeiko/2401933b1fa89e5d5bd238b33eab0465
   *
   * 2. Get reference to revery application, is there an existing
   * gtk application reference when a glfw window is created that can be reused?
   */
  GtkApplication *app;

  GdkDisplay *gd = gdk_display_get_default();
  GdkWindow *gw = gdk_x11_window_foreign_new_for_display(gd, pWin);

  GtkWidget *gtk = gtk_widget_new(GTK_TYPE_WINDOW, NULL);
  g_signal_connect(gtk, "realize", G_CALLBACK(create_gtk_window), gw);
  gtk_widget_set_has_window(gtk, TRUE);
  gtk_widget_realize(gtk);

  GtkWidget *menubar = gtk_menu_bar_new();
  GtkWidget *file = gtk_menu_item_new_with_label("File");
  GtkWidget *filemenu = gtk_menu_new();
  GtkWidget *quit = gtk_menu_item_new_with_label("Quit");
  gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 3);
  gtk_container_add(GTK_CONTAINER(gtk), box);

  app = gtk_application_new("org.gtk.revery", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), (gpointer)szMessage);

  gtk_widget_show_all(gtk);
  /* argv the final argument to run can be set to NULL in which case argc should
   * be set to 0 */
  g_application_run(G_APPLICATION(app), 0, NULL);
  g_object_unref(app);
}
#endif
