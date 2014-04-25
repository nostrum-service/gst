
#ifndef __GST_DUMMY_H__
#define __GST_DUMMY_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_DUMMY \
  (gst_dummy_get_type())
#define GST_DUMMY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DUMMY,Gstdummy))
#define GST_DUMMY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DUMMY,GstdummyClass))
#define GST_IS_DUMMY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DUMMY))
#define GST_IS_DUMMY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DUMMY))

typedef struct _Gstdummy      Gstdummy;
typedef struct _GstdummyClass GstdummyClass;

struct _Gstdummy
{
  GstElement element;

  GstPad *sinkpad, *srcpad;

  gboolean silent;
};

struct _GstdummyClass 
{
  GstElementClass parent_class;
};

GType gst_dummy_get_type (void);

G_END_DECLS

#endif /* __GST_DUMMY_H__ */
