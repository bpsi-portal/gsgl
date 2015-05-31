#include "gsaf.h"
#include "gsgl.h"
#include "utils.h"
#include "guiitems.h"
#include "gui_common.h"
#include "xwin.h"
#include "drv_gl.h"
#include "ogl.h"
#include "tcpip.h"
#include "tcp_guiitems.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "text.h"

#define GC_NAME_CONFLICT
#include "config.h"
#ifdef HAVE_GCDIR
#  include "gc/gc_cpp.h"
#else
#  include "gc_cpp.h"
#endif

class gsPoint : public gc
{
public:
        gsPoint(float x1=0.0, float y1=0.0) { x = x1; y = y1; };
        ~gsPoint() {};
        float x;
        float y;
};

class gsRectangle : public gc
{
public:
	gsRectangle(float x1, float y1, float w1, float h1) {x = x1; y = y1; w = w1; h = h1;};
        ~gsRectangle() {};
        bool pointIsInside(const gsPoint &p);
        float x;
        float y;
        float w;
        float h;
};

bool gsRectangle::pointIsInside(const gsPoint &p)
{
        if (p.x >= x && p.x <= x + w &&
            p.y >= y && p.y <= y + h)
                return true;
        else
                return false;
}

class gsLine : public gc
{
public:
	gsLine(gsPoint s, gsPoint e) {start = s; end = e;};
	~gsLine() {};
	gsPoint * crossPoint(const gsLine &l2);
	gsPoint start;
	gsPoint end;
};
		
gsPoint * gsLine::crossPoint(const gsLine & l1)
{
	return NULL;
}

static gsglboolean do_clip(const gsPoint &p1, const gsPoint &p2, const gsRectangle &r);

gsglboolean gsglClipLine(float *x, float *y)
{
	float pxs, pxe, pys, pye;
	float gxs, gxe, gys, gye;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye,
			&gxs, &gxe, &gys, &gye);

	gsRectangle rec(gxs, gys, gxe-gxs, gye-gys);
	
	gsPoint p1(x[0], y[0]);
	gsPoint p2(x[1], y[1]);

	if (rec.pointIsInside(p1)) {
		if (rec.pointIsInside(p2)) {
			return GSGL_TRUE;
		} else {
			return do_clip(p1, p2, rec);
		}
	} else {
		if (rec.pointIsInside(p2)) {
			return do_clip(p1, p2, rec);
		} else {
			return GSGL_FALSE;
		}
	}
}

static gsglboolean do_clip(const gsPoint &p1, const gsPoint &p2, const gsRectangle &r)
{
	gsLine l(p1, p2);
	gsPoint pr1(r.x, r.y);
	gsPoint pr2(r.x, r.y+r.h);
	gsPoint pr3(r.x+r.w, r.y+r.h);
	gsPoint pr4(r.x+r.w, r.y);

	gsLine l1(pr1, pr2);
	gsLine l2(pr2, pr3);
	gsLine l3(pr3, pr4);
	gsLine l4(pr4, pr1);

	l.crossPoint(l1);
	l.crossPoint(l2);
	l.crossPoint(l3);
	l.crossPoint(l4);
}
