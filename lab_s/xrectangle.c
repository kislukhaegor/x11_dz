/* RubberSort & Search: Oval functions overload */

#include <X11/Xlib.h>
#include <math.h>    /* nesessary for library sqrt() */
#include "xsort.h"

/* figure type set {XSegment, XRectangle, XArc} */

typedef XRectangle XFig; /* set figure type for oval */

/* figure type macro converter from (XFragment* ) */

#define REFIG(F) (F->rec) /* address seg, rec or arc */

#define EXTRAFILL 1 /* extra fragment fill (set 1 or 0) */

#define FIXEDFILL 0 /* fix fragment nofill (set 1 or 0) */

/* check fragment fill macros by type */

#define ISFILL(t) ((t) > 0 ? EXTRAFILL : FIXEDFILL)

/* Compute r = sqrt(s) by total r*r inspection */

int radical(int s) {
    int r, R, r2;          /* r, R=r+1, r^2 */
    for(R = r = 0, r2 = 0; r2 < s; r = R++)
        r2 = R*R;
    if((s - r*r) > (r2 - s))
        r = R;
    return(r);
} /* radical */

/* Check (x,y) hited fragment */

int fragon(XFragment* ff, int x, int y, int t) {
    XFig* f = REFIG(ff);
    int dr;                 /* oval error */
//int d[2];   /* hit from focuses distances */
    int d2;     /* hit from focuses distances square */
    int c;                  /* focus distance */
    int c2;                 /* focus distance square */
    int r;                  /* max oval axe size */
    int a = (f->width)/2;   /* oval 1/2 x-axe */
    int b = (f->height)/2;  /* oval 1/2 y-axe */
    XPoint p[2];            /* focuses pos */
    if((x < (f->x - 1)) || (x > (f->x + f->width + 1)) ||
       (y < (f->y - 1)) || (y > (f->y + f->height + 1)))
        return(0);     /* no oval inclusion rectangle zone */
    if((c2 = (a*a - b*b)) < 0)
        c2 = -c2;
    c = radical(c2);
    if(f->width > f->height) { /* x-axe > y-axe */
        p[0].x = f->x + a - c;
        p[1].x = f->x + a + c;
        p[0].y = p[1].y = f->y + b;
        r = f->width;
    } /* if */
    else {                     /* y-axe < x-axe */
        p[0].y = f->y + b - c;
        p[1].y = f->y + b + c;
        p[0].x = p[1].x = f->x + a;
        r = f->height;
    } /* else */
    d2 = (p[0].x - x)*(p[0].x - x) + (p[0].y - y)*(p[0].y - y);
//d[0] = radical(d2);
    d2 = (p[1].x - x)*(p[1].x - x) + (p[1].y - y)*(p[1].y - y);
//d[1] = radical(d2);
//dr = (d[0] + d[1] - r);
//if(dr > 2)         /* check hit outside oval */
    // return(0);       /* hit outside oval */
    if(t < 0)          /* for invoke by fragover */
        return(-t);      /* to overlap control */
    if(ISFILL(t) > 0)  /* check filling oval */
        return(1);       /* hit inside oval */
    if(dr < -4)        /* check hit oval contour */
        return(0);       /* hit not oval contour */
    return(1);         /* hit oval contour */
} /* fragon */

/* Overlap 2 fragmentes */

int fragover(XFragment* ff1, XFragment* ff2) {
    XFig* f1 = REFIG(ff1);
    XFig* f2 = REFIG(ff2);
    XSegment s1, s2;  /* oval f1 & f2 frames */
    XSegment r;       /* overlap oval frames */
    int x, y;         /* internal overlap frame pointer */
    s1.x1 = f1->x; s1.x2 = f1->x + f1->width;
    s1.y1 = f1->y; s1.y2 = f1->y + f1->height;
    s2.x1 = f2->x; s2.x2 = f2->x + f2->width;
    s2.y1 = f2->y; s2.y2 = f2->y + f2->height;
    if((s1.x1 > s2.x2) || (s2.x1 > s1.x2) ||
       (s1.y1 > s2.y2) || (s2.y1 > s1.y2))
        return(0);                    /* No overlap oval frames */
    r.x1 = (s1.x1 > s2.x1) ? s1.x1 : s2.x1;  /* max frames x1 */
    r.y1 = (s1.y1 > s2.y1) ? s1.y1 : s2.y1;  /* max frames y1 */
    r.x2 = (s1.x2 < s2.x2) ? s1.x2 : s2.x2;  /* min frames x2 */
    r.y2 = (s1.y2 < s2.y2) ? s1.y2 : s2.y2;  /* min frames y2 */
    r.x1 -= 2; r.y1 -= 2; r.x2 += 2; r.y2 += 2; /* line width */
    for(y = r.y1; y < r.y2; y++)          /* y-scan overframe */
        for(x = r.x1; x < r.x2; x++)        /* x-scan overframe */
            if(fragon(ff1, x, y, -1) > 0)   /* inner point oval 1 */
                if(fragon(ff2, x, y, -1) > 0) /* inner point oval 2 */
                    return(1);           /* inner point overlap ovals */
    return(0);                            /* No overlap ovals */
} /* fragover */

/* 2 Fragment difference for qsort*/

int difrag(XFragment* ff1, XFragment* ff2) {
    XFig* f1 = REFIG(ff1);
    XFig* f2 = REFIG(ff2);
    return(f1->width * f1->height - f2->width * f2->height);
} /* difrag */

/* Addressed Call fragment extra method by MACRO NUMBER */

int fragextra(int (*fe[])()) {   /* set extra method number */
    return((*fe[GRP2FRAG])()); /* MINI MAXI MEAN DIFF IDEN GRP2 */
} /* fragextra */

/* Fragment gabarit size */

int fragsize(XFragment* ff) {
    XFig* f = REFIG(ff);
    return(f->width * f->height);
} /* fragsize */

/* Tiny Fragment test */

int tinyfrag(XFragment* ff) {
    XFig* f = REFIG(ff);
    if((f->width < 8) || (f->height < 8))
        return(1);
    return(0);
} /* minifrag */

/* Stick fragment base xy-point */

int frag0(XFragment* ff, int x, int y) {
    XFig* f = REFIG(ff);
    f->x = x; f->y = y;
    f->width = f->height = 0;
// f->angle1 = (0*64);
// f->angle2 = (360*64);
    return(0);
} /* frag0 */

/* Variate fragment contour by xy-point */

int fragvar(XFragment* ff, int x, int y) {
    XFig* f = REFIG(ff);
    static int fx0, fy0; /* fragment base point (oval center) */
    int dx, dy;          /* distance from oval center to (xy) */
    int rx, ry; /* circle embrace rectancle shift from center */
    int ax, ay;   /* distance from oval to circle by PI/4 ray */
    if(f->width == 0)
        fx0 = f->x;
    if(f->height == 0)
        fy0 = f->y;
    if((dx = x - fx0) < 0)
        dx = -dx;
    if((dy = y - fy0) < 0)
        dy = -dy;
    ax = (int) (M_SQRT2 * dx);
    ay = (int) (M_SQRT2 * dy);
    rx = fx0 - ax; ry = fy0 - ay;
    if((rx < 0) || (ry < 0))    /* check left-top border */
        return(-1);
    f->x = rx; f->y = ry;
    f->width = 2*ax;
    f->height = 2*ay;
    return(0);
} /* fragvar */

/* Max fragment x-coordinate */

int fragmaxix(XFragment* ff) {
    XFig* f = REFIG(ff);
    return(f->x + f->width);
} /* fragmaxix */

/* Max fragment y-coordinate */

int fragmaxiy(XFragment* ff) {
    XFig* f = REFIG(ff);
    return(f->y + f->height);
} /* fragmaxiy */

/* Draw rubber fragment contour */

int XContour(Display* dpy, Window win, GC gc, XFragment* ff) {
    XFig* f = REFIG(ff);
    XDrawRectangles(dpy, win, gc, f, 1);
    return(0);
} /* XContours */

/* Draw 1 Fixed Fragmentes */

int XFix(Display* dpy, Window win, GC gc, XFragment* ff) {
    XFig* f = REFIG(ff);
    if(FIXEDFILL > 0)
        XFillRectangles(dpy, win, gc, f, 1);
    XDrawRectangles(dpy, win, gc, f, 1);       /* need at any case */
    return(0);
} /* XFix */

/* Draw Extra Fragment(s) */

int XExtra(Display* dpy, Window win, GC gc, XFragment* ff) {
    XFig* f = REFIG(ff);
    if(EXTRAFILL > 0)
        XFillRectangles(dpy, win, gc, f, 1);
    XDrawRectangles(dpy, win, gc, f, 1);
    return(0);
} /* XExtra */