#ifndef __apl1_h_
#define __apl1_h_

struct _gsglpattern
{
	float Pat[11];
	float Xt;
	float Yt;
	float Zt;
	float Vt;
	int Nt;
	int Npatt;
	int Npats[15];
	float Pats[15][10];
	gsglboolean Hold_xy;
	int ipat_exception;
};

#define PATTERN(p) (p->pattern)

int findex2 (int i, int j, int nxa);
void gsgl_init_hold_text_position (gsglPage *);
gsglboolean gsgl_inq_hold_text_position (gsglPage *);

#endif
