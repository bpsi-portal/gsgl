#ifndef __clientstate_h__
#define __clientstate_h__

struct _gsglClientState
{
	gsgllist *VertexPointer0;
	gsgllist *ColorPointer0;
	gsgllist *NormalPointer0;
};

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_array_element (unsigned int i);
void gsgl_pointer_set (int datatype, int nxm, int nx, int ny, float *z, float *x, float *y,
		       float xmin, float xmax, float ymin, float ymax, int *idata, 
		       float *fdata, gsglPage *);
void gsgl_clientstate_disable (gsglPage *);

void gsgl_init_client_state_every_page (gsglPage *);
int gsgl_inq_clientstate_color_enabled_num (gsglPage *);
gsgllist * gsgl_inq_first_clientstate_color_pointer (gsglPage *);
int gsgl_inq_clientstate_vertex_enabled_num (gsglPage *);
gsgllist * gsgl_inq_first_clientstate_vertex_pointer (gsglPage *);
int gsgl_inq_clientstate_normal_enabled_num (gsglPage *);
gsgllist * gsgl_inq_first_clientstate_normal_pointer (gsglPage *);
void gsgl_clientstate_color_append (GLfloat *data, gsglPage *);
void gsglsetdatapointer1 (int nmx, int nx, int ny, float *z, 
			  float xmin, float xmax, float ymin, float ymax, gsglPage *);
void gsglsetdatapointer2 (int nmx, int nx, int ny, float *z, float *x, float *y, gsglPage *);
void gsglsetdatapointer3 (int nmx, int nr, int nt, float *z, float *r, gsglPage *);
void gsglsetdatapointer4 (int nmx, int nr, int nt, float *z, float *r, float *t, gsglPage *);
void gsglsetdatapointer5 (int nmx, int nx, int ny, float *z, float *x, float *y, gsglPage *);
void gsglsetdatapointerf (int nnode, int nelm, int *ielm, float *z, float *x, float *y, gsglPage *);
void gsglsetcolorpointer (int nx, int ny, int i,int j, float *rgb, gsglPage *);

#ifdef __cplusplus
}
#endif

#endif

