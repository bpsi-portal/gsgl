#include "gsaf.h"
#include "fileformat.h"
#include "displaylist.h"
#include "clientstate.h"
#include "base.h"
#include "utils.h"
#include "drv_wrapper.h"

static void gsgl_pointer12_normal (int i, int j, int nx, int ny, float vertex[], float n[3]);
static void gsgl_pointer3_normal (int i, int j, int nx, int ny, float vertex[], float n[3]);
static void gsgl_pointerf_normal (int i, int nnode, int nelm, int *idata,
				  float vertex[], float n[3]);

void gsgl_init_client_state_every_page (gsglPage *p)
{
	p->client->VertexPointer0 = p->client->ColorPointer0 = p->client->NormalPointer0 = NULL;
}

int gsgl_inq_clientstate_vertex_enabled_num (gsglPage *p)
{
	return gsgl_list_count (p->client->VertexPointer0);
}

gsgllist * gsgl_inq_first_clientstate_vertex_pointer (gsglPage *p)
{
	return p->client->VertexPointer0;
}

int gsgl_inq_clientstate_normal_enabled_num (gsglPage *p)
{
	return gsgl_list_count (p->client->NormalPointer0);
}

gsgllist * gsgl_inq_first_clientstate_normal_pointer (gsglPage *p)
{
	return p->client->NormalPointer0;
}

int gsgl_inq_clientstate_color_enabled_num (gsglPage *p)
{
	return gsgl_list_count (p->client->ColorPointer0);
}

gsgllist * gsgl_inq_first_clientstate_color_pointer (gsglPage *p)
{
	return p->client->ColorPointer0;
}

/******************************************************/
/*        Client State を disable して，さらに        */     
/*        データ を Free する．                       */
/******************************************************/
void gsgl_clientstate_disable (gsglPage *p)
{
	gsgllist *list;

	if (gsgl_inq_clientstate_vertex_enabled_num (p) > 0) {
		gsgl_drv_glarray_disable_vertex (p);
		list = p->client->VertexPointer0;
		p->client->VertexPointer0 = NULL;
		while (list) {
			if (list->data) {
				free (list->data);
				list->data=NULL;
			}
			list = list->next;
		}
	}
	if (gsgl_inq_clientstate_color_enabled_num (p) > 0) {
		gsgl_drv_glarray_disable_color (p);
		list = p->client->ColorPointer0;
		p->client->ColorPointer0 = NULL;
		while (list) {
			if (list->data) {
				free(list->data);
				list->data=NULL;
			}
			list = list->next;
		}
	}
	if (gsgl_inq_clientstate_normal_enabled_num (p) > 0) {
		gsgl_drv_glarray_disable_normal (p);
		list = p->client->NormalPointer0;
		p->client->NormalPointer0 = NULL;
		while (list) {
			if (list->data) {
				free (list->data);
				list->data=NULL;
			}
			list = list->next;
		}
	}
}

static GLfloat * append_vertex (int size, gsglPage *p)
{
	GLfloat *data;
	data = malloc (size*sizeof(GLfloat));
	
	p->client->VertexPointer0 = gsgl_list_append (p->client->VertexPointer0, data);
	return data;
}

static GLfloat * append_normal (int size, gsglPage *p)
{
	GLfloat *data;
	data = malloc (size*sizeof(GLfloat));

	p->client->NormalPointer0 = gsgl_list_append (p->client->NormalPointer0, data);
	return data;
}

void gsgl_pointer_set (int datatype, int nxm, int nx, int ny1, float *z, float *x, float *y,
		       float xmin, float xmax, float ymin, float ymax,
		       int *idata, float *fdata, gsglPage *p)
{
	int i, j;
	GLfloat *vertex, *normal;
	double dt, rad;
	double angle/*, tmp*/;
	int ny;

	if (datatype == GSGL_DATA_FEM_TRIANGLE) {
		ny = 1;
	} else {
		ny = ny1;
	}

	/* Vertex array */
	if (nx < 1 || ny < 1) {
		gsgl_error ("gsgl_pointer_ser", "nx or ny < 1 (nx=%d,ny=%d)", nx,ny);
	}

	if (datatype == GSGL_DATA_ORTHO_EQUIV_INTERVAL) {
		if (z == NULL) gsgl_error ("gsgl_pointer_ser", "z == NULL");
	} else if (datatype == GSGL_DATA_ORTHO_UNEQU_INTERVAL) {
		if (x == NULL) gsgl_error ("gsgl_pointer_ser", "x == NULL");
		if (y == NULL) gsgl_error ("gsgl_pointer_ser", "y == NULL");
		if (z == NULL) gsgl_error ("gsgl_pointer_ser", "z == NULL");
	} else if (datatype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
		if (x == NULL) gsgl_error ("gsgl_pointer_ser", "x == NULL");
		if (z == NULL) gsgl_error ("gsgl_pointer_ser", "z == NULL");
	} else if (datatype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
		if (x == NULL) gsgl_error ("gsgl_pointer_ser", "x == NULL");
		if (y == NULL) gsgl_error ("gsgl_pointer_ser", "y == NULL");
		if (z == NULL) gsgl_error ("gsgl_pointer_ser", "z == NULL");
	} else if (datatype == GSGL_DATA_X_Y_Z_2D) {
		if (x == NULL) gsgl_error ("gsgl_pointer_ser", "x == NULL");
		if (y == NULL) gsgl_error ("gsgl_pointer_ser", "y == NULL");
		if (z == NULL) gsgl_error ("gsgl_pointer_ser", "z == NULL");
	} else if (datatype == GSGL_DATA_FEM_TRIANGLE) {
		if (x == NULL) gsgl_error ("gsgl_pointer_ser", "x == NULL");
		if (y == NULL) gsgl_error ("gsgl_pointer_ser", "y == NULL");
		if (z == NULL) gsgl_error ("gsgl_pointer_ser", "z == NULL");
	} else {
		gsgl_error ("gsgl_pointer_ser", 
			    "Unexpected data type %d", datatype);
	}
	vertex = append_vertex (nx*ny*3, p);

	dt =  2.0*M_PI/ny;
	rad = M_PI/180.0;

	for (i=0; i<nx; i++) {
		for (j=0; j<ny; j++) {
			if (datatype == GSGL_DATA_ORTHO_EQUIV_INTERVAL) {
				vertex[i*ny*3+j*3+0] = xmin+(xmax-xmin)*(i/(float) (nx-1));
				vertex[i*ny*3+j*3+1] = ymin+(ymax-ymin)*(j/(float) (ny-1));
				vertex[i*ny*3+j*3+2] = z[j*nxm+i];
			} else if (datatype == GSGL_DATA_ORTHO_UNEQU_INTERVAL) {
				vertex[i*ny*3+j*3+0] = x[i];
				vertex[i*ny*3+j*3+1] = y[j];
				vertex[i*ny*3+j*3+2] = z[j*nxm+i];
			} else if (datatype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
				angle = dt * j;
				vertex[i*ny*3+j*3+0] = x[i] * cos (angle);
				vertex[i*ny*3+j*3+1] = x[i] * sin (angle);
				vertex[i*ny*3+j*3+2] = z[j*nxm+i];
			} else if (datatype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
				angle = rad * y[j];
				vertex[i*ny*3+j*3+0] = x[i] * cos (angle);
				vertex[i*ny*3+j*3+1] = x[i] * sin (angle);
				vertex[i*ny*3+j*3+2] = z[j*nxm+i];
			} else if (datatype == GSGL_DATA_X_Y_Z_2D) {
				vertex[i*ny*3+j*3+0] = x[j*nxm+i];
				vertex[i*ny*3+j*3+1] = y[j*nxm+i];
				vertex[i*ny*3+j*3+2] = z[j*nxm+i];
			} else if (datatype == GSGL_DATA_FEM_TRIANGLE) {
				vertex[i*3+0] = x[i];
				vertex[i*3+1] = y[i];
				vertex[i*3+2] = z[i];
			}
		}
	}
	gsgl_glarray3d_vertex (datatype, 3, nx*ny*3, vertex, nx, ny1, x, y, xmin, xmax, ymin, ymax, idata, fdata, p);

	normal = append_normal (nx*ny*3, p);
	for (i=0; i<nx; i++) {
		for (j=0; j<ny; j++) {
			if (datatype == GSGL_DATA_ORTHO_EQUIV_INTERVAL ||
			    datatype == GSGL_DATA_ORTHO_UNEQU_INTERVAL ||
			    datatype == GSGL_DATA_CYLIN_UNEQU_INTERVAL ||
			    datatype == GSGL_DATA_X_Y_Z_2D) {
				gsgl_pointer12_normal (i, j, nx, ny, vertex, &(normal[i*ny*3+j*3+0]));
			} else if (datatype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
				gsgl_pointer3_normal (i, j, nx, ny, vertex, &(normal[i*ny*3+j*3+0]));
			} else if (datatype == GSGL_DATA_FEM_TRIANGLE) {
				gsgl_pointerf_normal (i, nx, ny1, idata, vertex, &(normal[i*3]));
			}
		}
	}
	gsgl_glarray3d_normal (datatype, 3, nx*ny*3, normal, p);
}

void gsgl_clientstate_color_append (GLfloat *data, gsglPage *p)
{
	p->client->ColorPointer0 = gsgl_list_append (p->client->ColorPointer0, data);
}

void gsgl_color_data_set (int nx, int ny, int i, int j, float *rgb, gsglPage *p)
{
	GLfloat *data;
	gsgllist *color;
	
	if (nx == 0 && ny == 0 && i == 0 && j == 0) {
		gsgl_clientstate_color_append (NULL, p);
		gsgl_glarray3d_color (0, 0, rgb, p);
		return;
	}
	
	if (i == 0 && j == 0) {
		data = malloc (nx*ny*3*sizeof (GLfloat));
		gsgl_clientstate_color_append (data, p);
	}
	color = gsgl_list_last(p->client->ColorPointer0);
	data = color->data;
	data[i*ny*3+j*3+0] = rgb[0];
	data[i*ny*3+j*3+1] = rgb[1];
	data[i*ny*3+j*3+2] = rgb[2];
	if (i == nx-1 && j == ny-1) {
		gsgl_glarray3d_color (3, nx*ny*3, color->data, p);
	}
}

/*******************************************************/
/*     Client State (等間隔直交データ) を設定する．    */
/*******************************************************/
void gsglsetdatapointer1 (int nmx, int nx, int ny, float *z, 
			  float xmin, float xmax, float ymin, float ymax, gsglPage *p)
{
	gsgl_pointer_set (GSGL_DATA_ORTHO_EQUIV_INTERVAL,
			  nmx, nx, ny, z, NULL, NULL, xmin, xmax, ymin, ymax,
			  NULL, NULL, p);
}

/*******************************************************/
/*     Client State (非等間隔直交データ) を設定する．  */
/*******************************************************/
void gsglsetdatapointer2 (int nmx, int nx, int ny, float *z, float *x, float *y, gsglPage *p)
{
	gsgl_pointer_set (GSGL_DATA_ORTHO_UNEQU_INTERVAL,
			  nmx, nx, ny, z, x, y, 0.0, 0.0, 0.0, 0.0,
			  NULL, NULL, p);
}

/*******************************************************/
/*     Client State (等角度円形データ) を設定する．    */
/*******************************************************/
void gsglsetdatapointer3 (int nmx, int nr, int nt, float *z, float *r, gsglPage *p)
{
	gsgl_pointer_set (GSGL_DATA_CYLIN_EQUIV_INTERVAL,
			  nmx, nr, nt, z, r, NULL, 0.0, 0.0, 0.0, 0.0,
			  NULL, NULL, p);
}

/*********************************************************/
/*     Client State (非等角度円形データ) を設定する．    */
/*********************************************************/
void gsglsetdatapointer4 (int nmx, int nr, int nt, float *z, float *r, float *t, gsglPage *p)
{
	gsgl_pointer_set (GSGL_DATA_CYLIN_UNEQU_INTERVAL,
			  nmx, nr, nt, z, r, t, 0.0, 0.0, 0.0, 0.0,
			  NULL, NULL, p);
}
/*********************************************************/
/*     Client State (X,Y,Zとも2次元データ) を設定する．  */
/*********************************************************/
void gsglsetdatapointer5 (int nmx, int nx, int ny, float *z, float *x, float *y, gsglPage *p)
{
	gsgl_pointer_set (GSGL_DATA_X_Y_Z_2D,
			  nmx, nx, ny, z, x, y, 0.0, 0.0, 0.0, 0.0,
			  NULL, NULL, p);
}

/***************************************************/
/*     Client State (三角形有限要素) を設定する．  */
/***************************************************/
void gsglsetdatapointerf (int nnode, int nelm, int *ielm, float *z, float *x, float *y, gsglPage *p)
{
	gsgl_pointer_set (GSGL_DATA_FEM_TRIANGLE,
			  nnode, nnode, nelm, z, x, y, 0.0, 0.0, 0.0, 0.0, 
			  ielm, NULL, p);
}

/***********************************************/
/*   Client State のうち Color を設定する．    */
/***********************************************/
void gsglsetcolorpointer (int nx, int ny, int i,int j, float *rgb, gsglPage *p)
{
	gsgl_color_data_set (nx, ny, i, j, rgb, p);
}

#define EPS 1.e-3

static gsglboolean gsgl_vector_from_vertex (int ny, int i1, int j1, int i2, int j2,
					    float *vector, float *Vertex)
{
	float len;

	vector[0] = Vertex[i2*ny*3+j2*3+0] - Vertex[i1*ny*3+j1*3+0];
	vector[1] = Vertex[i2*ny*3+j2*3+1] - Vertex[i1*ny*3+j1*3+1];
	vector[2] = Vertex[i2*ny*3+j2*3+2] - Vertex[i1*ny*3+j1*3+2];

	len = sqrt(vector[0]*vector[0]+vector[1]*vector[1]+vector[2]*vector[2]); 

	if (len < EPS) {
		return GSGL_TRUE;
	} else {
		/*vector[0] /= len;
		  vector[1] /= len;
		  vector[2] /= len;*/
		return GSGL_FALSE;
	}
}

static void gsgl_normal_from_vector (float *v1, float *v2, float *n)
{
	n[0] += v1[1]*v2[2]-v2[1]*v1[2];
	n[1] += v1[2]*v2[0]-v2[2]*v1[0];
	n[2] += v1[0]*v2[1]-v2[0]*v1[1];
}

/*******************************************************/
/* Client State のうち Normal (直交データ) を設定する．*/
/*******************************************************/
static void gsgl_pointer12_normal (int i, int j, int nx, int ny, float vertex[], float n[3]) 
{
	float v1[3], v2[3];
	
	n[0]=n[1]=n[2]=0.0;
	
	if (i == 0 && j == 0) { /* 四つ角 */
		if (gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex))
			gsgl_vector_from_vertex (ny, i+1, j+1, i+1, j, v1, vertex);
		if (gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex))
			gsgl_vector_from_vertex (ny, i, j+1, i+1, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == 0 && j == ny-1) { /* 四つ角 */
		if (gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex))
			gsgl_vector_from_vertex (ny, i+1, j-1, i, j-1, v1, vertex);
		if (gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex))
			gsgl_vector_from_vertex (ny, i+1, j, i+1, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		/*printf ("i=0 j=jmax %f %f %f\n", n[0],n[1],n[2]);*/
	} else if (i == nx-1 && j == 0) { /* 四つ角 */
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == nx-1 && j == ny-1) { /* 四つ角 */
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);		
	} else if (i == 0) {		
		if (gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex))
			gsgl_vector_from_vertex (ny, i+1, j-1, i, j-1, v1, vertex);
		if (gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex))
			gsgl_vector_from_vertex (ny, i+1, j, i+1, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		if (gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex))
			gsgl_vector_from_vertex (ny, i+1, j+1, i+1, j, v1, vertex);
		if (gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex))
			gsgl_vector_from_vertex (ny, i, j+1, i+1, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		/*printf ("i=0 %f %f %f\n", n[0],n[1],n[2]);*/
	} else if (i == nx-1) {
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (j == 0) {
		gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (j == ny-1) {
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else {
		gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	}
	/*printf ("%f %f %f\n", n[0],n[1],n[2]);*/
	if (n[0]==0.0 && n[1]==0.0 && n[2]==0.0)
		n[2] = 1.0;
}

/*************************************************************/
/* Client State のうち Normal (等角度円形データ) を設定する．*/
/*************************************************************/
static void gsgl_pointer3_normal (int i, int j, int nx, int ny, float vertex[], float n[3]) 
{
	GLfloat v1[3], v2[3];
	
	n[0]=n[1]=n[2]=0.0;
	
	if (i == 0 && j == 0) { /* 四つ角 */
		gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, ny-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == 0 && j == ny-1) { /* 四つ角 */
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, 0, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == nx-1 && j == 0) { /* 四つ角 */
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, ny-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == nx-1 && j == ny-1) { /* 四つ角 */
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, 0, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == 0) {
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (i == nx-1) {
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (j == 0) {
		gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else if (j == ny-1) {
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	} else {
		gsgl_vector_from_vertex (ny, i, j+1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i+1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i+1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j-1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i-1, j, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i, j+1, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
		gsgl_vector_from_vertex (ny, i, j-1, i, j, v1, vertex);
		gsgl_vector_from_vertex (ny, i, j, i-1, j, v2, vertex);
		gsgl_normal_from_vector (v1, v2, n);
	}
}

/*************************************************************/
/* Client State のうち Normal (等角度円形データ) を設定する．*/
/*************************************************************/
static void gsgl_pointerf_normal (int inode, int nnode, 
				  int nelm, int *ielm,
				  float vertex[], float n[3]) 
{
	GLfloat v1[3], v2[3];
	int i;
	int in0, in1, in2;
	
	n[0]=n[1]=n[2]=0.0;
	
	for (i=0; i<nelm; i++) {
		in0 = ielm [3*i + 0]-1;
		in1 = ielm [3*i + 1]-1;
		in2 = ielm [3*i + 2]-1;
		/*if (inode == 0)
		  printf ("%d (%d %d %d)\n", i, in0, in1, in2);*/
		if (inode == in0) {
			v1[0] = vertex[3*in0+0] - vertex[3*in2+0];
			v1[1] = vertex[3*in0+1] - vertex[3*in2+1];
			v1[2] = vertex[3*in0+2] - vertex[3*in2+2];
			v2[0] = vertex[3*in1+0] - vertex[3*in0+0];
			v2[1] = vertex[3*in1+1] - vertex[3*in0+1];
			v2[2] = vertex[3*in1+2] - vertex[3*in0+2];
			gsgl_normal_from_vector (v1, v2, n);
		}
		if (inode == in1) {
			v1[0] = vertex[3*in1+0] - vertex[3*in0+0];
			v1[1] = vertex[3*in1+1] - vertex[3*in0+1];
			v1[2] = vertex[3*in1+2] - vertex[3*in0+2];
			v2[0] = vertex[3*in2+0] - vertex[3*in1+0];
			v2[1] = vertex[3*in2+1] - vertex[3*in1+1];
			v2[2] = vertex[3*in2+2] - vertex[3*in1+2];
			gsgl_normal_from_vector (v1, v2, n);
		}
		if (inode == in2) {
			v1[0] = vertex[3*in2+0] - vertex[3*in1+0];
			v1[1] = vertex[3*in2+1] - vertex[3*in1+1];
			v1[2] = vertex[3*in2+2] - vertex[3*in1+2];
			v2[0] = vertex[3*in0+0] - vertex[3*in2+0];
			v2[1] = vertex[3*in0+1] - vertex[3*in2+1];
			v2[2] = vertex[3*in0+2] - vertex[3*in2+2];
			gsgl_normal_from_vector (v1, v2, n);
		}
	}
	/*printf ("%d %f %f %f\n", inode, n[0], n[1], n[2]);*/
}
