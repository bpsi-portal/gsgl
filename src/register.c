#include "gsaf.h"
#include "utils.h"
#include "base.h"
#include "drv_wrapper.h"

typedef struct _registered_data {
	int handler;
	float *data;
	int dim;
	int *n;
	int type;
	gsglboolean show;
	char *label;
	int  labelfont;
	float labelfontsize;
	int   colorindex;
	float rgb[3];
	float linewidth_cm;
	int   linewidth_index;
	int linetype;
	int marktype;
	int markstep;
	gsglboolean logscale;
	float min;
	float max;
	int scale_valueindex;
	int scale_type;
	float scale_step;
	float scale_length;
	float shown_min;
	float shown_max;
	struct _registered_data *next;
} registered_data;

#define REGISTER_DATA_NON   0
#define REGISTER_DATA_DATA  5
#define REGISTER_DATA_X     6
#define REGISTER_DATA_Y     7

registered_data *Start;
int Handlermax;
int Colorindex;
int Linetype;

void gsgl_init_register_every_page ()
{
	Start = NULL;
	Handlermax = 0;
	Colorindex = 7;
	Linetype = 0;
}

registered_data * gsgl_registered_data_new ()
{
	registered_data *rdata;

	rdata = gsgl_malloc (sizeof (registered_data));
	
	rdata->handler = 0;
	rdata->data = NULL;
	rdata->dim = 0;
	rdata->n = NULL;
	rdata->type = REGISTER_DATA_NON;
	rdata->show = GSGL_TRUE;
	rdata->label = NULL;
	rdata->labelfont = 0;
	rdata->labelfontsize = 0.35;
	rdata->linewidth_cm = -1.0;
	rdata->linewidth_index = 2;
	rdata->linetype = 0;
	rdata->colorindex = 7;
	rdata->rgb[0] = -1.0;
	rdata->rgb[1] = -1.0;
	rdata->rgb[2] = -1.0;
	rdata->marktype = 0;
	rdata->markstep = 0;
	rdata->logscale = GSGL_FALSE;
	rdata->scale_valueindex = 0;
	rdata->scale_type = 1;
	rdata->scale_length = 0.3;
	rdata->scale_step = 0.0;
	rdata->shown_min = 0.0;
	rdata->shown_max = 0.0;
	rdata->min = 0.0;
	rdata->max = 0.0;
	rdata->next = NULL;

	return rdata;
}

int gsgl_register_data (float *data, int dim, int *n, 
			 char *type, int ltype,
			 char *label, int llabel)
{
	registered_data *rdata, *rdatanew;
	/*int nx;
	int i;
	float totalmin, totalmax;

	if (dim < 1 || dim > 1) {
		gsgl_warning ("gsgl_register_data", "dim != 1");
		return 0;
	}
	if (data == NULL) {
		gsgl_warning ("gsgl_register_data", "data == NULL");
		return 0;
	}
	if (n == NULL) {		
		gsgl_warning ("gsgl_register_data", "n == NULL");
		return 0;
	}
	if (type == NULL) {
		gsgl_warning ("gsgl_register_data", "type == NULL");
		return 0;
	}

	if (Start == NULL) {
		Start = gsgl_registered_data_new ();
		rdata = Start;
	} else {
		rdata = Start;
		while (rdata->next != NULL)
			rdata = rdata->next;
		rdatanew = gsgl_registered_data_new ();
		rdata->next = rdatanew;
		rdata = rdatanew;
	}

	Handlermax += 1;

	rdata->handler = Handlermax;
	rdata->data = data;
	rdata->dim = dim;
	rdata->n = n;
	if ((type[0] == 'D' || type[0] == 'd') &&
	    (type[1] == 'A' || type[1] == 'a') &&
	    (type[2] == 'T' || type[2] == 't') &&
	    (type[3] == 'A' || type[3] == 'a')) {
		rdata->type = REGISTER_DATA_DATA;
	} else if ((type[0] == 'X' || type[0] == 'x') &&
		   (type[1] == 'A' || type[1] == 'a') &&
		   (type[2] == 'X' || type[2] == 'x') &&
		   (type[3] == 'I' || type[3] == 'i') &&
		   (type[4] == 'S' || type[4] == 's')) {
		rdata->type = REGISTER_DATA_X;
	} else if ((type[0] == 'Y' || type[0] == 'y') &&
		   (type[1] == 'A' || type[1] == 'a') &&
		   (type[2] == 'X' || type[2] == 'x') &&
		   (type[3] == 'I' || type[3] == 'i') &&
		   (type[4] == 'S' || type[4] == 's')) {
		rdata->type = REGISTER_DATA_Y;
	} else {
		gsgl_warning ("gsgl_register_data", "registered data type is not valid");
	}
	rdata->show = GSGL_TRUE;
	rdata->label = label;

	INQFNT_F77 (&(rdata->labelfont));
	INQCHR_F77 (&(rdata->labelfontsize), dummy, dummy, dummy, dummy);

	INQLNW_F77 (&(rdata->linewidth_cm));
	INQLIN_F77 (&(rdata->linetype), &(rdata->linewidth_index), &(rdata->colorindex));
	if (rdata->linewidth_cm > 0.0) {
		rdata->linewidth_index = -1;
	}
	if (rdata->type == REGISTER_DATA_DATA) {
		rdata->linetype = Linetype;
		rdata->colorindex = Colorindex;
		Colorindex = Colorindex - 1;
		if (Colorindex <= 0) Colorindex = 7;
	} else {
		rdata->linetype = 0;
		rdata->colorindex = 7;
	}
	rdata->logscale = GSGL_FALSE;

	if (rdata->dim == 1) {
		nx = rdata->n[0];
		rdata->min = rdata->data[0];
		rdata->max = rdata->data[0];
		for (i = 0; i < nx; i++) {
			if (rdata->min > rdata->data[i]) {
				rdata->min = rdata->data[i];
			} else if (rdata->max < rdata->data[i]) {
				rdata->max = rdata->data[i];
			}
		}
		if (rdata->type != REGISTER_DATA_DATA) {
			GQSCAL_F77 (&(rdata->min), &(rdata->max),
				    &(rdata->shown_min), &(rdata->shown_max),
				    &(rdata->scale_step));
			rdata->scale_valueindex = NGULEN_F77 (&(rdata->scale_step));
		} else {
			rdatanew = Start;
			totalmin = rdatanew->min;
			totalmax = rdatanew->max;
			while (rdatanew != NULL) {
				if (rdatanew->type == REGISTER_DATA_DATA) {
					if (totalmin > rdata->min) {
						totalmin = rdata->min;
					} else if (totalmax < rdata->max) {
						totalmax = rdata->max;
					}
				}
				rdatanew = rdatanew->next;
			}
			GQSCAL_F77 (&totalmin, &totalmax,
				    &(rdata->shown_min), &(rdata->shown_max),
				    &(rdata->scale_step));
			rdata->scale_valueindex = NGULEN_F77 (&(rdata->scale_step));

			rdatanew = Start;
			while (rdatanew != NULL) {
				if (rdatanew->type == REGISTER_DATA_DATA) {
					rdatanew->shown_min = rdata->shown_min;
					rdatanew->shown_max = rdata->shown_max;
					rdatanew->scale_step = rdata->scale_step;
					rdatanew->scale_valueindex = rdata->scale_valueindex;
				}
				rdatanew = rdatanew->next;
			}
		}
	} else {
		
	}*/
	return rdata->handler;
}

void gsgl_plot_registered_data ()
{
	/*
	  regaistered_data *data, *xdata = NULL, *ydata = NULL;
	float pxmin = 2.0, pxmax = 24.0, pymin = 1.5, pymax = 17.5;
	int nx, one = 1;
	int minusone = -1;
	int seven = 7;
	int two = 2;
	float fzero = 0.0;
	float temp;
	float temp2;
	float xorg, yorg;
	gsglboolean mem = gsgl_inq_drv_mem ();
	gsglboolean tcp = gsgl_inq_drv_tcp ();
	gsglboolean hd = gsgl_inq_drv_hd ();

	if (Start == NULL) {
		gsgl_warning ("gsgl_plot_registered_data", "no registered data");
		return;
	}
	data = Start;
	while (data != NULL) {
		if (data->type == REGISTER_DATA_X) {
			xdata = data;
		}
		data = data->next;
	}
	if (xdata == NULL) {
		gsgl_warning ("gsgl_plot_registered_data", "no x-axis data");
		return;
	}
	data = Start;
	while (data != NULL) {
		if (data->type == REGISTER_DATA_DATA) {
			ydata = data;
		}
		data = data->next;
	}
	if (ydata == NULL) {
		gsgl_warning ("gsgl_plot_registered_data", "no target data");
		return;
	}

	gsgl_disable_drv_mem ();
	gsgl_disable_drv_tcp ();
	gsgl_disable_drv_hd ();

	GDEFIN_F77 (&pxmin, &pxmax, &pymin, &pymax,
		    &(xdata->shown_min), &(xdata->shown_max),
		    &(ydata->shown_min), &(ydata->shown_max));
	GFRAME_F77 ();

	nx = xdata->n[0];
	
	data = Start;
	while (data != NULL) {
		if (data->show && data->type == REGISTER_DATA_DATA && data->n[0] == nx) {
			if (data->colorindex > 0) {
				SETLIN_F77 (&minusone, &minusone, &(data->colorindex));
			} else {
				SETRGB_F77 (&(data->rgb[0]), &(data->rgb[1]), &(data->rgb[2]));
			}
			GPLOTP_F77 (xdata->data, data->data, &one, &nx, &one,
				    &(data->marktype), &(data->markstep), &(data->linetype));
		}
		data = data->next;
	}
	SETLIN_F77 (&(minusone), &(minusone), &(seven));

	if (xdata->shown_min * xdata->shown_max < 0.0) {
		xorg = 0.0;
	} else {
		xorg = xdata->shown_min;
	}
	GSCALE_F77 (&xorg, &(xdata->scale_step), &(fzero), &(fzero),
		    &(xdata->scale_length), &(xdata->scale_type));

	if (ydata->shown_min * ydata->shown_max < 0.0) {
		yorg = 0.0;
	} else {
		yorg = ydata->shown_min;
	}
	GSCALE_F77 (&(fzero), &(fzero), &yorg,
		    &(ydata->scale_step), &(ydata->scale_length), &(ydata->scale_type));

	temp = 2 * xdata->scale_step;
	GVALUE_F77 (&xorg, &temp, &fzero, &fzero, &(xdata->scale_valueindex));
	temp = 2 * ydata->scale_step;
	GVALUE_F77 (&fzero, &fzero, &yorg, &temp, &(ydata->scale_valueindex));
	
	temp = (xdata->shown_min + xdata->shown_max) * 0.5;
	temp2 = ydata->shown_min - 1.0;
	GTEXTX2D_F77 (&temp, &temp2, xdata->label, &two);

	if (mem) gsgl_enable_drv_mem ();
	if (tcp) gsgl_enable_drv_tcp ();
	if (hd) gsgl_enable_drv_hd ();*/
}

static registered_data * data_from_handler (int handler)
{
	registered_data *data;
	data = Start;
	while (data != NULL) {
		if (data->handler == handler) return data;
		data = data->next;
	}
	return NULL;
}

void gsgl_set_data_label (int handler, char *label)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->label = label;
}

void gsgl_set_data_labelfont (int handler, int font)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->labelfont = font;
}

void gsgl_set_data_labelfontsize (int handler, float size)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->labelfontsize = size;
}

void gsgl_set_data_linewidth (int handler, float width)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->linewidth_cm = width;
}

void gsgl_set_data_rgb (int handler, float r, float g, float b)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->rgb[0] = r;
	rdata->rgb[1] = g;
	rdata->rgb[2] = b;
}

void gsgl_set_data_mark (int handler, int mark)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->marktype = mark;
}

void gsgl_set_data_markstep (int handler, int markstep)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->markstep = markstep;
}

void gsgl_set_data_scalevalueindex (int handler, int scaleindex)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->scale_valueindex = scaleindex;
}

void gsgl_set_data_scalestep (int handler, float scalestep)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->scale_step = scalestep;
}

void gsgl_set_data_linetype (int handler, int linetype)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->linetype = linetype;
}

void gsgl_set_data_shownmin (int handler, float shownmin)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->shown_min = shownmin;
}

void gsgl_set_data_shownmax (int handler, float shownmax)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->shown_max = shownmax;
}

void gsgl_set_data_logscale (int handler, gsglboolean log)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->logscale = log;
}

void gsgl_set_data_show (int handler, gsglboolean show)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->show = show;
}

void gsgl_set_data_linewidthindex (int handler, int linewidthindex)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->linewidth_index = linewidthindex;
}

void gsgl_set_data_colorindex (int handler, int colorindex)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->colorindex = colorindex;
}

void gsgl_set_data_scale_type (int handler, int scale_type)
{
	registered_data *rdata;

	rdata = data_from_handler (handler);
	rdata->scale_type = scale_type;
}

