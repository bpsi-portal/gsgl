#include "gsaf.h"
#include "gsgl.h"
#include "bsc1.h"

/*c     *****************************
  c     *      set data color       *
  c     *****************************
*/

void gsaf_r2g2b_17 (const float *z, float *rgb)
{
	int iz;

	iz = (*z) * 17.99999;
	if (iz < 0) {
		iz=0;
	} else if (iz >= 18) {
		iz=17;
	}
	if (iz <= 4) {
		rgb[0]=0.0;
		rgb[1]=0.25 * iz;
		rgb[2]=1.0;
	} else if (iz <= 9) {
		rgb[0]=0.0;
		rgb[1]=1.0;
		rgb[2]=1.0-0.25 * (iz-5);
	} else if (iz <= 13) {
		rgb[0]=0.25 * (iz-9);
		rgb[1]=1.0;
		rgb[2]=0.0;
	} else if (iz <= 17) {
		rgb[0]=1.0;
		rgb[1]=1.0-0.25 * (iz-13);
		rgb[2]=0.0;
	} else {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=0.0;
	}
}

void gsaf_r2g2p (const float *r, float *rgb)
{
	int ir;

	ir = 200.0* (*r);
	if (ir < 0) {
		ir=0;
	} else if (ir > 200) {
		ir=200;
	}
	if (ir <= 40) {
		rgb[0]=1.0-ir/40.0;
		rgb[1]=0.0;
		rgb[2]=1.0;
	} else if (ir <= 80) {
		rgb[0]=0.0;
		rgb[1]=(ir-40)/40.0;
		rgb[2]=1.0;
	} else if (ir <= 120) {
		rgb[0]=0.0;
		rgb[1]=1.0;
		rgb[2]=1.0-(ir-80)/40.0;
	} else if (ir <= 160) {
		rgb[0]=(ir-120)/40.0;
		rgb[1]=1.0;
		rgb[2]=0.0;
	} else if (ir <= 200) {
		rgb[0]=1.0;
		rgb[1]=1.0-(ir-160)/40.0;
		rgb[2]=0.0;
	} else {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=0.0;
	}
}

void gsaf_white (const float *r, float *rgb)
{
	rgb[0]=1.0;
	rgb[1]=1.0;
	rgb[2]=1.0;
}


void gsaf_black (const float *r, float *rgb)
{
	rgb[0]=0.0;
	rgb[1]=0.0;
	rgb[2]=0.0;
}

void gsaf_r2g2b (const float *r, float *rgb)
{
	int ir;

	ir = 160.0* (*r);
	if (ir < 0) {
		ir=0;
	} else if (ir > 160) {
		ir=160;
	}
	if (ir <= 40) {
		rgb[0]=0.0;
		rgb[1]=ir/40.0;
		rgb[2]=1.0;
	} else if (ir <= 80) {
		rgb[0]=0.0;
		rgb[1]=1.0;
		rgb[2]=1.0-(ir-40)/40.0;
	} else if (ir <= 120) {
		rgb[0]=(ir-80)/40.0;
		rgb[1]=1.0;
		rgb[2]=0.0;
	} else if (ir <= 160) {
		rgb[0]=1.0;
		rgb[1]=1.0-(ir-120)/40.0;
		rgb[2]=0.0;
	} else {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=0.0;
	}
}

void gsaf_r2w2b (const float *r, float *rgb)
{
	int ir;

	ir = 80.0* (*r);
	if (ir < 0) {
		ir=0;
	} else if (ir > 80) {
		ir=80;
	}
	if (ir <= 40) {
		rgb[0]=ir/40.0;
		rgb[1]=ir/40.0;
		rgb[2]=1.0;
	} else if (ir <= 80) {
		rgb[0]=1.0;
		rgb[1]=1.0-(ir-40)/40.0;
		rgb[2]=1.0-(ir-40)/40.0;
	} else {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=0.0;
	}
}

void gsaf_r2g2bw (const float *r, float *rgb)
{
	int ir;

	ir = 160.0* (*r);
	if (ir < 0) {
		ir=0;
	} else if (ir > 160) {
		ir=160;
	}
	if (ir == 0) {
		rgb[0]=1.0;
		rgb[1]=1.0;
		rgb[2]=1.0;
	} else if (ir >= 0 && ir <= 40) {
		rgb[0]=0.0;
		rgb[1]=ir/40.0;
		rgb[2]=1.0;
	} else if (ir <= 80) {
		rgb[0]=0.0;
		rgb[1]=1.0;
		rgb[2]=1.0-(ir-40)/40.0;
	} else if (ir <= 120) {
		rgb[0]=(ir-80)/40.0;
		rgb[1]=1.0;
		rgb[2]=0.0;
	} else if (ir <= 160) {
		rgb[0]=1.0;
		rgb[1]=1.0-(ir-120)/40.0;
		rgb[2]=0.0;
	} else {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=0.0;
	}
}

void gsaf_r2y2w(const float *r, float *rgb)
{
        int ir;
	
	ir=80.0*(*r);

	if (ir < 0) {
		ir=0;
	} else if (ir > 80) {
		ir=80;
	}
	if (ir <= 40) {
		rgb[0]=1.0;
		rgb[1]=1.0;
		rgb[2]=1.0-ir/40.0;
	} else {
		rgb[0]=1.0;
		rgb[1]=1.0-(ir-40)/40.0;
		rgb[2]=0.0;
	}
	
}

void gsaf_w2g2b(const float *r, float *rgb)
{
	int ir;

	ir=80.0*(*r);

	if (ir < 0) {
		ir=0;
	} else if (ir > 80) {
		ir=80;
	}
	if (ir <= 40) {
		rgb[0]=0.0;
		rgb[1]=0.0+ir/40.0;
		rgb[2]=1.0-ir/40.0;
	} else {
		rgb[0]=0.0+(ir-40)/40.0;
		rgb[1]=1.0;
		rgb[2]=0.0+(ir-40)/40.0;
	}
}   

void gsaf_w2r2k(const float *r, float *rgb)
{
	int ir;

	ir=120.0*(*r);

	if (ir < 0) {
		ir=0;
	} else if (ir > 120) {
		ir=120;
	}
	if (ir <= 40) {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=ir/40.0;
	} else if (ir <= 80) {
		rgb[0]=0.0;
		rgb[1]=(ir-40)/40.0;
		rgb[2]=1.0;
	} else {
		rgb[0]=(ir-80)/40.0;
		rgb[1]=1.0;
		rgb[2]=1.0;
	}
}   

void gsaf_k2b2w(const float *r, float *rgb)
{
	int ir;

	ir=120.0*(*r);

	if (ir < 0) {
		ir=0;
	} else if (ir > 120) {
		ir=120;
	}
	if (ir <= 40) {
		rgb[0]=1.0-ir/40.0;
		rgb[1]=1.0;
		rgb[2]=1.0;
	} else if (ir <= 80) {
		rgb[0]=0.0;
		rgb[1]=1.0-(ir-40)/40.0;
		rgb[2]=1.0;
	} else {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=1.0-(ir-80)/40.0;
	}
}   

void gsaf_wrkbw(const float *r, float *rgb)
{
	int ir;

	ir=120.0*(*r);

	if (ir < 0) {
		ir=0;
	} else if (ir > 120) {
		ir=120;
	}
	if (ir <= 20) {
		rgb[0]=1.0-ir/20.0;
		rgb[1]=1.0;
		rgb[2]=1.0;
	} else if (ir <= 40) {
		rgb[0]=0.0;
		rgb[1]=1.0-(ir-20)/20.0;
		rgb[2]=1.0;
	} else if (ir <= 60) {
		rgb[0]=0.0;
		rgb[1]=0.0;
		rgb[2]=1.0-(ir-40)/20.0;
	} else if (ir <= 80) {
		rgb[0]=(ir-60)/20.0;
		rgb[1]=0.0;
		rgb[2]=0.0;
	} else if (ir <= 100) {
		rgb[0]=1.0;
		rgb[1]=(ir-80)/20.0;
		rgb[2]=0.0;
	} else {
		rgb[0]=1.0;
		rgb[1]=1.0;
		rgb[2]=(ir-100)/20.0;
	}
}   

void gsaf_wrgbw(const float *r, float *rgb)
{
	int ir;

	ir=120.0*(*r);

	if (ir < 0) {
		ir=0;
	} else if (ir > 120) {
		ir=120;
	}
	if (ir <= 20) {
		rgb[0]=1.0-ir/20.0;
		rgb[1]=1.0;
		rgb[2]=1.0;
	} else if (ir <= 40) {
		rgb[0]=0.0;
		rgb[1]=1.0-(ir-20)/20.0;
		rgb[2]=1.0;
	} else if (ir <= 60) {
		rgb[0]=0.0;
		rgb[1]=(ir-40)/20.0;
		rgb[2]=1.0-(ir-40)/20.0;
	} else if (ir <= 80) {
		rgb[0]=(ir-60)/20.0;
		rgb[1]=1.0-(ir-60)/20.0;
		rgb[2]=0.0;
	} else if (ir <= 100) {
		rgb[0]=1.0;
		rgb[1]=(ir-80)/20.0;
		rgb[2]=0.0;
	} else {
		rgb[0]=1.0;
		rgb[1]=1.0;
		rgb[2]=(ir-100)/20.0;
	}
}   

void gsaf_r2g2b_gat0(const float *r, float *rgb)
{
	int ir;
	gsglPage *p;
	gsgl3DInfo *info;
	float zmax, zmin, z, rnew;

	p = gsgl_inq_page ();
	if (!p) {
	  gsaf_r2g2b (r, rgb);
	  return;
	}

	info = p->ThreeDInfo;
	if (!info) {
	  gsaf_r2g2b (r, rgb);
	  return;
	}

	zmin = info->Gzmin;
	zmax = info->Gzmax;

	z = (zmax - zmin) * (*r) + zmin;
	if (zmin * zmax > 0.0) {
	  if (zmin > 0.0) { /* zmin > 0 and zmax > 0, red @ zmax, green @ 0 */ 
	    rnew = (z + zmax) / (2.0 * zmax);
	  } else {  /* zmin < 0 and zmax < 0, green @ 0, blue @ zmin */ 
	    rnew = (z - zmin) / (- 2.0 * zmin);
	  }
	} else if (zmin * zmax < 0.0) {
	  if (fabs(zmin) > fabs(zmax)) { /* green @ 0, blue @ zmin */
	    rnew = (z - zmin) / (- 2.0 * zmin);
	  } else { /* red @ zmax, green @ 0 */
	    rnew = (z + zmax) / (2.0 * zmax);
	  }
	} else {
	  if (fabs(zmin) / (zmax - zmin) < 1.e-5) { /* zmin = 0.0, red @ zmax, green @ zmin */
	    rnew = (z - zmin + zmax - zmin) / (2.0 * (zmax - zmin));
	  } else { /* zmax = 0.0, green @ zmax, blue @ zmin */
	    rnew = (z - zmin) / (2.0 * (zmax - zmin));
	  }
	}
	gsaf_r2g2b (&rnew, rgb);
}

void gsaf_r2w2b_wat0(const float *r, float *rgb)
{
	int ir;
	gsglPage *p;
	gsgl3DInfo *info;
	float zmax, zmin, z, rnew;

	p = gsgl_inq_page ();
	if (!p) {
	  gsaf_r2g2b (r, rgb);
	  return;
	}

	info = p->ThreeDInfo;
	if (!info) {
	  gsaf_r2g2b (r, rgb);
	  return;
	}

	zmin = info->Gzmin;
	zmax = info->Gzmax;

	z = (zmax - zmin) * (*r) + zmin;
	if (zmin * zmax > 0.0) {
	  if (zmin > 0.0) { /* zmin > 0 and zmax > 0, red @ zmax, green @ 0 */ 
	    rnew = (z + zmax) / (2.0 * zmax);
	  } else {  /* zmin < 0 and zmax < 0, green @ 0, blue @ zmin */ 
	    rnew = (z - zmin) / (- 2.0 * zmin);
	  }
	} else if (zmin * zmax < 0.0) {
	  if (fabs(zmin) > fabs(zmax)) { /* green @ 0, blue @ zmin */
	    rnew = (z - zmin) / (- 2.0 * zmin);
	  } else { /* red @ zmax, green @ 0 */
	    rnew = (z + zmax) / (2.0 * zmax);
	  }
	} else {
	  if (fabs(zmin) / (zmax - zmin) < 1.e-5) { /* zmin = 0.0, red @ zmax, green @ zmin */
	    rnew = (z - zmin + zmax - zmin) / (2.0 * (zmax - zmin));
	  } else { /* zmax = 0.0, green @ zmax, blue @ zmin */
	    rnew = (z - zmin) / (2.0 * (zmax - zmin));
	  }
	}
	gsaf_r2w2b (&rnew, rgb);
}
