
/**
* SECTION:element-painter
*
* FIXME:Describe painter here.
*
* <refsect2>
* <title>Example launch line</title>
* |[
* gst-launch -v -m fakesrc ! painter ! fakesink silent=TRUE
* ]|
* </refsect2>
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video-format.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "gstpainter.h"

GST_DEBUG_CATEGORY_STATIC (gst_painter_debug);
#define GST_CAT_DEFAULT gst_painter_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_SILENT
};

/* the capabilities of the inputs and outputs.
*
* describe the real formats here.
*/
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS ( GST_VIDEO_CAPS_MAKE ("{  BGRx, RGBx }") )
	);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
	GST_PAD_SRC,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS ( GST_VIDEO_CAPS_MAKE ("{  BGRx, RGBx }") )
	);

#define gst_painter_parent_class parent_class
G_DEFINE_TYPE (Gstpainter, gst_painter, GST_TYPE_ELEMENT);

static void gst_painter_set_property (GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec);
static void gst_painter_get_property (GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec);

static gboolean gst_painter_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_painter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the painter's class */
static void
	gst_painter_class_init (GstpainterClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_painter_set_property;
	gobject_class->get_property = gst_painter_get_property;

	g_object_class_install_property (gobject_class, PROP_SILENT,
		g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
		FALSE, (GParamFlags)G_PARAM_READWRITE));

	gst_element_class_set_details_simple(gstelement_class,
		"painter",
		"FIXME:Generic",
		"FIXME:Generic Template Element",
		"Charlie AUTHOR_NAME <<user@hostname.org>>");

	gst_element_class_add_pad_template (gstelement_class,
		gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (gstelement_class,
		gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
* instantiate pads and add them to element
* set pad calback functions
* initialize instance structure
*/
static void
	gst_painter_init (Gstpainter * filter)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,
		GST_DEBUG_FUNCPTR(gst_painter_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,
		GST_DEBUG_FUNCPTR(gst_painter_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	filter->silent = FALSE;
}

static void
	gst_painter_set_property (GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec)
{
	Gstpainter *filter = GST_PAINTER (object);

	switch (prop_id) {
	case PROP_SILENT:
		filter->silent = g_value_get_boolean (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
	gst_painter_get_property (GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec)
{
	Gstpainter *filter = GST_PAINTER (object);

	switch (prop_id) {
	case PROP_SILENT:
		g_value_set_boolean (value, filter->silent);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
	gst_painter_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
	gboolean ret;
	Gstpainter *filter;

	filter = GST_PAINTER (parent);

	switch (GST_EVENT_TYPE (event)) {
	case GST_EVENT_CAPS:
		{
			GstCaps * caps;

			gst_event_parse_caps (event, &caps);

			GstStructure *structure = gst_caps_get_structure (caps, 0);

			gst_structure_get_int (structure, "width", &filter->width);
			gst_structure_get_int (structure, "height", &filter->height);

			filter->format = gst_structure_get_string (structure, "format");

			//gst_video_info_from_caps (&filter->input_vi, caps);

			ret = gst_pad_event_default (pad, parent, event);
			break;
		}
	default:
		ret = gst_pad_event_default (pad, parent, event);
		break;
	}
	return ret;
}

static GstBuffer *
	gst_painter_process_data (Gstpainter * filter, GstBuffer * buf)
{
	GstMapInfo srcmapinfo;
	gst_buffer_map (buf, &srcmapinfo, GST_MAP_READ);

	IplImage * image = cvCreateImageHeader (cvSize (filter->width, filter->height), IPL_DEPTH_8U, 3);
	image->imageData = (char*)srcmapinfo.data;

	cvReleaseImageHeader(&image);

	GstBuffer * outbuf = gst_buffer_new ();
	
	IplImage * dst = cvCreateImageHeader (cvSize (filter->width, filter->height), IPL_DEPTH_8U, 4);
	GstMemory * memory = gst_allocator_alloc (NULL, dst->imageSize, NULL);
	GstMapInfo dstmapinfo;
	if (gst_memory_map(memory, &dstmapinfo, GST_MAP_WRITE)) {

		memcpy (dstmapinfo.data, srcmapinfo.data, srcmapinfo.size);

		dst->imageData = (char*)dstmapinfo.data;

		cvRectangle (dst, cvPoint(10,10), cvPoint(100, 100), CV_RGB(0, 255, 0), 1, 0);
		/*
		IplImage * new_img = cvCreateImage(cvGetSize(dst), IPL_DEPTH_8U, 1); 
		cvCanny ( dst, new_img, 50, 200, 3 );

		if (FILE *file = fopen("c:\\work\\Progs\\GstTestLib\\Debug\\0dst.jpg", "r")) {
			fclose(file);
		}
		else {
			cvSaveImage("c:\\work\\Progs\\GstTestLib\\Debug\\0dst.jpg",new_img);
		}

		cvCvtColor (new_img, dst, CV_GRAY2BGRA);

		cvReleaseImage (&new_img);
		*/

		gst_buffer_insert_memory (outbuf, -1, memory);

		gst_memory_unmap(memory, &dstmapinfo);
	}

	cvReleaseImageHeader(&dst);
	
	gst_buffer_unmap(buf, &srcmapinfo);

	return outbuf;
}

/* chain function
* this function does the actual processing
*/
static GstFlowReturn
	gst_painter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	Gstpainter *filter;

	filter = GST_PAINTER (parent);

	if (filter->silent == FALSE)
		g_print ("I'm plugged, therefore I'm in.\n");


	GstBuffer *outbuf;
	outbuf = gst_painter_process_data (filter, buf);

	gst_buffer_unref (buf);
	if (!outbuf) {
		/* something went wrong - signal an error */
		GST_ELEMENT_ERROR (GST_ELEMENT (filter), STREAM, FAILED, (NULL), (NULL));
		return GST_FLOW_ERROR;
	}

	return gst_pad_push (filter->srcpad, outbuf);
}
