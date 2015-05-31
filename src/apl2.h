#ifndef __apl2_h_
#define __apl2_h_

struct _gsgl2dinfo
{
	float Dx;
	float Dy;
	float Pxs;
	float Pys;
	float Pxe;
	float Pye;
	float Gxs;
	float Gys;
	float Gxe;
	float Gye;
	gsglboolean Lgf;
};

gsglboolean gsgl_inq_gdefin_is_called (gsglPage *);

#endif
