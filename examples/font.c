#include <gtk/gtk.h>

#include <gtk/gtkgl.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <string.h>

static gchar font_name[] = "-adobe-helvetica-medium-r-normal--*-120-*-*-*-*-*-*";
static GLuint font_list_base;
static gint font_height;

static const gint config_attributes[] = {
  GDK_GL_DOUBLEBUFFER,
  GDK_GL_RGBA,
  GDK_GL_RED_SIZE,        1,
  GDK_GL_GREEN_SIZE,      1,
  GDK_GL_BLUE_SIZE,       1,
  GDK_GL_DEPTH_SIZE,      12,
  GDK_GL_ATTRIB_LIST_NONE
};

static void
print_gl_config_attrib (GdkGLConfig *glconfig,
                        const gchar *attrib_str,
                        gint         attrib,
                        gboolean     is_boolean)
{
  gint value;

  g_print ("%s = ", attrib_str);
  if (gdk_gl_config_get_attrib (glconfig, attrib, &value))
    {
      if (is_boolean)
        g_print ("%s\n", value == TRUE ? "TRUE" : "FALSE");
      else
        g_print ("%d\n", value);
    }
  else
    g_print ("*** Cannot get %s attribute value\n", attrib_str);
}

static void
examine_gl_config_attrib (GdkGLConfig *glconfig)
{
  g_print ("\nOpenGL visual configurations :\n");

  print_gl_config_attrib (glconfig, "GDK_GL_USE_GL",           GDK_GL_USE_GL,           TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_BUFFER_SIZE",      GDK_GL_BUFFER_SIZE,      FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_LEVEL",            GDK_GL_LEVEL,            FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_RGBA",             GDK_GL_RGBA,             TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_DOUBLEBUFFER",     GDK_GL_DOUBLEBUFFER,     TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_STEREO",           GDK_GL_STEREO,           TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_AUX_BUFFERS",      GDK_GL_AUX_BUFFERS,      FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_RED_SIZE",         GDK_GL_RED_SIZE,         FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_GREEN_SIZE",       GDK_GL_GREEN_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_BLUE_SIZE",        GDK_GL_BLUE_SIZE,        FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ALPHA_SIZE",       GDK_GL_ALPHA_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_DEPTH_SIZE",       GDK_GL_DEPTH_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_STENCIL_SIZE",     GDK_GL_STENCIL_SIZE,     FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_RED_SIZE",   GDK_GL_ACCUM_RED_SIZE,   FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_GREEN_SIZE", GDK_GL_ACCUM_GREEN_SIZE, FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_BLUE_SIZE",  GDK_GL_ACCUM_BLUE_SIZE,  FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_ALPHA_SIZE", GDK_GL_ACCUM_ALPHA_SIZE, FALSE);

  g_print ("\n");
}

static void
init (GtkWidget *widget,
      gpointer   data)
{
  GdkFont *font;

  /*
   * Print OpenGL visual configuration.
   */
  examine_gl_config_attrib (gtk_widget_get_gl_config(widget));

  /* OpenGL begin. */
  gtk_widget_gl_begin (widget);

  /*
   * Generate font display lists.
   */
  font_list_base = glGenLists (128);
  font = gdk_font_load(font_name);
  if (!font) {
    g_print("*** Can't load font '%s'\n", font_name);
    gtk_exit (1);
  }
  gdk_gl_font_use_gdk_font (font, 0, 128, font_list_base);
  font_height = font->ascent + font->descent;

  glClearColor (1.0, 1.0, 1.0, 1.0);
  glClearDepth (1.0);

  glViewport (0, 0,
              widget->allocation.width, widget->allocation.height);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0, widget->allocation.width,
           0.0, widget->allocation.height,
           -1.0, 1.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  gtk_widget_gl_end (widget);
  /* OpenGL end. */
}

static gboolean
reshape (GtkWidget         *widget,
         GdkEventConfigure *event,
         gpointer           data)
{
  /* OpenGL begin. */
  gtk_widget_gl_begin (widget);

  glViewport (0, 0,
              widget->allocation.width, widget->allocation.height);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0, widget->allocation.width,
           0.0, widget->allocation.height,
           -1.0, 1.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  gtk_widget_gl_end (widget);
  /* OpenGL end. */

  return TRUE;
}

static gboolean
display (GtkWidget      *widget,
         GdkEventExpose *event,
         gpointer        data)
{
  int i, j;

  /* OpenGL begin. */
  gtk_widget_gl_begin (widget);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*
   * Draw some text.
   */
  glColor3f (0.0, 0.0, 0.0);
  for (i = 2; i >= -2; i--)
    {
      glRasterPos2f (10.0, 0.5*widget->allocation.height + i*font_height);
      for (j = ' '; j <= 'Z'; j++)
        glCallList (font_list_base+j);
    }

  /*
   * Show font name.
   */
  glColor3f (1.0, 0.0, 0.0);
  glRasterPos2f (10.0, 10.0);
  glListBase (font_list_base);
  glCallLists (strlen(font_name), GL_UNSIGNED_BYTE, font_name);

  if (gtk_widget_gl_is_double_buffer (widget))
    gtk_widget_gl_swap_buffers (widget);
  else
    glFlush ();

  gtk_widget_gl_end (widget);
  /* OpenGL end. */

#if 0

  gdk_draw_arc (widget->window,
                widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                TRUE,
                0.4*widget->allocation.width,
                0.4*widget->allocation.height,
                0.2*widget->allocation.width,
                0.2*widget->allocation.height,
                0, 64 * 360);

#endif

  return TRUE;
}

static gint
quit (GtkWidget *widget,
      GdkEvent  *event,
      gpointer   data)
{
  gtk_main_quit ();

  return FALSE;
}

int
main (int argc,
      char *argv[])
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *drawing_area;
  GtkWidget *button;
  gint major, minor;

  gtk_init (&argc, &argv);

  /*
   * OpenGL extension is supported?
   */

  if (!gdk_gl_query_extension ())
    {
      g_print ("\n*** OpenGL extension is not supported\n");
      gtk_exit (1);
    }
  gdk_gl_query_version (&major, &minor);
  g_print ("\nOpenGL extension is supported - version %d.%d\n",
           major, minor);

  /*
   * Top-level window.
   */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (window, "Font");

  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (quit), NULL);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  /*
   * Drawing area for drawing OpenGL scene.
   */

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (drawing_area), 600, 200);

  /* Set OpenGL-capability to the widget. */
  gtk_widget_set_gl_capability (GTK_WIDGET (drawing_area),
				config_attributes,
                                GDK_GL_RGBA_TYPE,
                                NULL,
                                TRUE);

  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

  gtk_widget_set_events (drawing_area,
                         GDK_EXPOSURE_MASK |
                         GDK_BUTTON_PRESS_MASK);

  g_signal_connect (G_OBJECT (drawing_area), "realize",
		    G_CALLBACK (init), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "configure_event",
		    G_CALLBACK (reshape), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (display), NULL);

  gtk_widget_show (drawing_area);

  /*
   * Simple quit button.
   */

  button = gtk_button_new_with_label ("Quit");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (quit), NULL);

  gtk_widget_show (button);

  /*
   * Show window.
   */

  gtk_widget_show (window);

  /*
   * Main loop.
   */

  gtk_main ();

  return 0;
}
