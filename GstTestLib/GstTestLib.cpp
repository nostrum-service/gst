#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstdummy.h"
#include "gstpainter.h"

struct _elements_entry
{
  const gchar *name;
    GType (*type) (void);
};

static const struct _elements_entry _elements[] = {
  {"dummy", gst_dummy_get_type},
  {"painter", gst_painter_get_type},
  {NULL, 0},
};

static gboolean
plugin_init (GstPlugin * plugin)
{
  gint i = 0;

  while (_elements[i].name) {
    if (!gst_element_register (plugin, _elements[i].name,
            GST_RANK_NONE, (_elements[i].type) ()))
      return FALSE;
    i++;
  }

  return TRUE;
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
* in configure.ac and then written into and defined in config.h, but we can
* just set it ourselves here in case someone doesn't use autotools to
* compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
*/
#ifndef PACKAGE
#define PACKAGE "gsttestlib"
#endif

#ifndef VERSION
#define VERSION "1.0.0.0"
#endif

#ifndef GST_PACKAGE_NAME
#define GST_PACKAGE_NAME "gsttest"
#endif

#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "Nostrum"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    gsttestlib,
    "gsttest",
    plugin_init, 
	VERSION, 
	"LGPL", 
	GST_PACKAGE_NAME, 
	GST_PACKAGE_ORIGIN);