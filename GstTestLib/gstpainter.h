
#ifndef __GST_PAINTER_H__
#define __GST_PAINTER_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_PAINTER \
  (gst_painter_get_type())
#define GST_PAINTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PAINTER,Gstpainter))
#define GST_PAINTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PAINTER,GstpainterClass))
#define GST_IS_PAINTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PAINTER))
#define GST_IS_PAINTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PAINTER))

typedef struct _Gstpainter      Gstpainter;
typedef struct _GstpainterClass GstpainterClass;

struct _Gstpainter
{
  GstElement element;

  GstPad *sinkpad, *srcpad;

  gboolean silent;

  const char *format;

  int width;
  int height;

//  GstVideoInfo input_vi, output_vi;
};

struct _GstpainterClass 
{
  GstElementClass parent_class;
};

GType gst_painter_get_type (void);

G_END_DECLS

#endif /* __GST_PAINTER_H__ */
