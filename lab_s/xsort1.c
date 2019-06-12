/* RubberSort: Rubber flex functions */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>
#include "xsort.h"

#define FRAGSIZ sizeof(XFragment)

/* Graphics parameters */

static Display* dpy;                             /* Display address */
static Window win;                                /* Main Window id */
static GC* gc;                                  /* GC array address */

/* Geometry parameters */

static XFragment ftmp[1];          /* template (temporary) fragment */
static XFragment bak[2];                     /* bak extra fragments */
static XFragment* frag;                   /* fragments' set address */
static int nfrag=0;                        /* all fragments' number */
static int extra[2]={0, 0};                 /* extra fragment index */
static int GGC=FGC;      /* alt (after) ForeGC index (for grp2extr) */

/* Pass graphic parameters from main (xsort2) */

int pass1(Display* d, Window w, GC* g) {
    dpy = d; win = w; gc = g;
    return(0);
} /* pass */

/* Photo flex parametes to pass xsort0 by theare address */

int photo() {
    static void* p[4];  /* passing list: bak, extra, frag, &nfrag */
    p[0] = bak; p[1] = extra, p[2] = frag; p[3] = &nfrag;
    pass0(p);           /* pass p=list to extra module xsort0 */
    return(nfrag);      /* when 1-st fragment has been drawed */
} /* photo */

/* Redraw n fixed fragmentes */

int XFixes(Display* dpy, Window win , GC gc, XFragment* f, int n) {
    int i;  /* fragment index */
    for(i = 0; i < n; i++, f++)
        XFix(dpy, win, gc, f); /* redraw i fixed fragment */
    return(0);
} /* XFixes */

/* Store extra fragments to  bak-copy */

int rebak() {
    if(nfrag > 0)                    /* baking 1-st extra fragment info */
        memcpy(bak, frag+extra[0], FRAGSIZ);
    if(extra[1] > extra[0])          /* baking 2-nd extra fragment info */
        memcpy(bak+1, frag+extra[1], FRAGSIZ);
    return(0);
} /* rebak */

/* Press Button1 new rubber fragment origin */

int rubin(XEvent* ev) {
    XGrabPointer(dpy, win, False, (ButtonReleaseMask | Button1MotionMask),
                 GrabModeAsync, GrabModeAsync, win, None, CurrentTime);
    rebak();                              /* to backing extra fragments */
    if(ev->xbutton.button == Button3)  /* to Erase fragment by Button 2 */
        return(Button3);            /* return to delete pointed fragment */
    frag0(ftmp, ev->xbutton.x, ev->xbutton.y); /* set reform base point */
    return(Button1);                /* return to rubber deform & resize */
} /* rubin */

/* Find fragment near with (x,y) point to delete by Button 2 or 3 */

int near(int x, int y) {
    int i;      /* fragment index */
    for(i = 0; i < nfrag; i++)
        if(fragon(frag+i, x, y, isextra(i)) > 0)
            break;
    return(i); /* nfrag if miss or near fragment index */
} /* near */

/* Erase Button2 or 3 clicked fragment (draw by bg BGC) */

int rubout(XEvent* ev) {
    int i;                                  /* deleted fragment index */
    if(nfrag < 1)
        return(0);               /* no one fragment to rub out (delete) */
    if((i = near(ev->xbutton.x, ev->xbutton.y)) == nfrag)
        return(nfrag);          /* no delete when far from any fragment */
    if((i == extra[0]) || (i == extra[1]))
        XExtra(dpy, win, gc[BGC], frag+i);     /* rubout extra fragment */
    XFix(dpy, win, gc[BGC], frag+i);       /* rubout 1 fixed fragment */
    XFlush(dpy);                                         /* flush out */
    if(--nfrag > i)                    /* shift fragment list to left */
        memmove((frag + i), (frag + i + 1), (nfrag - i)*FRAGSIZ);
    frag = realloc(frag, nfrag*FRAGSIZ);           /* free 1 fragment */
    if(nfrag == 0)
        frag = NULL;                             /* empty fragment list */
    return(nfrag);
} /* rubout */

/* Deformate Rubber fragment (double draw by rg RGC) */

int rerub(XEvent* ev) {
    static int x, y;            /* previous cursor point */
    XContour(dpy, win, gc[RGC], ftmp);
    if(fragvar(ftmp, ev->xmotion.x, ev->xmotion.y) < 0) {
        XContour(dpy, win, gc[RGC], ftmp);
        return(XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y));
    } /* when base centered fragment connect top or left border */
    XContour(dpy, win, gc[RGC], ftmp);
    x = ev->xmotion.x; y =  ev->xmotion.y; /* store cursor point */
    return(0);
} /* rerub */

/* Reset Alter GC index for fragments group (call grp2extr) */

int reggc(int g) {
    return(GGC = g);
} /* regc */

/* Redraw all Fragments in expo or after edit fragment set */

int refrag() {
    int n;                          /* redraw fragments number */
    if((n = extra[0]) > 0)                 /* fix before extra */
        XFixes(dpy, win, gc[FGC], frag, n);
    if((n = (extra[1] - extra[0])) > 1)  /* fix between extras */
        XFixes(dpy, win, gc[FGC], (frag + extra[0] + 1), n - 1);
    if((n = (nfrag - extra[1])) > 1)        /* fix after extra */
        XFixes(dpy, win, gc[GGC], (frag + extra[1] + 1), n - 1);
    if(nfrag > 0)                         /* redraw 1-st extra */
        XExtra(dpy, win, gc[EGC], frag + extra[0]);
    if(extra[1] > extra[0])               /* redraw 2-nd extra */
        XExtra(dpy, win, gc[EGC], frag + extra[1]);
    return(nfrag);
} /* refrag */

/* Extend window by base centered fragment only */
/* No need for base corner fragment or KDE */

int widewin() {
    int w, h; /* window min width & height for temp fragment */
    XWindowAttributes attr;             /* window attributes */
    XGetWindowAttributes(dpy, win, &attr);
    w = fragmaxix(ftmp);
    h = fragmaxiy(ftmp);
    if((w < attr.width) && (h < attr.height))  /* no expand */
        return(0);              /* window for inside template */
    if(w < attr.width)      /* check expand window by width */
        w = attr.width;             /* no expand window width */
    if(h < attr.height)    /* check expand window by height */
        h = attr.height;           /* no expand window height */
    XResizeWindow(dpy, win, w, h);    /* expand window size */
    return(0);
} /* widewin */

/* Cancel template fragment, when overlap or tiny size */

int cancet() {
    int i;   /* fragment index */
    if(tinyfrag(ftmp) > 0)             /* escape tiny template */
        return(1);
    for(i=0; i < nfrag; i++)      /* escape overlaped template */
        if(fragover(ftmp, frag + i) > 0)
            return(1);
    return(0);  /* No cancel fragment */
} /* cancet */

/* Fix rubber fragment  with all redrawing by fg GC */

int fix(XEvent* ev) {
    int w, h;        /* window min width & height for temp fragment */
    XUngrabPointer(dpy, CurrentTime);     /* uncatch window pointer */
    if(ev->xbutton.button != Button1)          /* if erase fragment */
        return(nfrag);                     /* then return with no fix */
    fragvar(ftmp, ev->xbutton.x, ev->xbutton.y); /* frag ending pos */
    XContour(dpy, win, gc[RGC], ftmp);         /* erase last rubber */
    XFlush(dpy);                                /* fragment contour */
    if(cancet() > 0)       /* test size & overlap template fragment */
        return(0);                                   /* to cancel fix */
//widewin(w, h);              /* extend window to right or bottom */
    frag = realloc(frag, (nfrag + 1)*FRAGSIZ);            /* append */
    memcpy((frag+nfrag), ftmp, FRAGSIZ);            /* new fragment */
    if(++nfrag > 1)                  /* quick resort fragments list */
        qsort(frag, nfrag, FRAGSIZ, fragcmp);
    return(nfrag);
} /* fix */

/* Purge bak extra & space for new extra fragment */

int purgextr(XFragment* b, XFragment* e) {
    if(memcmp(b, e, FRAGSIZ) != 0) {
        XExtra(dpy, win, gc[BGC], b); /* purge bak extra space */
        XFix(dpy, win, gc[BGC], e);   /* purge new extra space */
    } /* if */
    return(0);
} /* purgextr */

/* Compare method to 2 fragments by by increase order */
/* typedef int (*FCMP)(const void*, const void*); */

int fragcmp(const void* s1, const void* s2) {
    return(difrag((XFragment*) s1, (XFragment*) s2));
} /* fragcmp */

/* set main window minimum size to WM */

int miniwin() {
    XSizeHints hint;     /* WM geom hints */
    int i=0;             /* fragment index */
    unsigned w=128;      /* window min width */
    unsigned h=128;      /* window min height */
    int xm, ym;          /* max x & y coordinate */
    for(i=0; i < nfrag; i++) {
        if((xm = fragmaxix(frag+i)) > w)
            w = xm;
        if((ym = fragmaxiy(frag+i)) > h)
            h = ym;
    } /* for */
    hint.min_width = w; hint.min_height = h;
    hint.flags = PMinSize;
    XSetNormalHints(dpy, win, &hint);
    return(0);
} /* miniwin */

/* Free all fragmentes */

int allfree() {
    if(nfrag > 0)            /* delete all fragments' */
        free(frag);            /* memory space */
    nfrag = 0;               /* reset fragments' count */
    frag = NULL;             /* reset empty fragments' set */
    extra[0] = extra[1] = 0; /* reset extra index(es) */
    XClearWindow(dpy, win);  /* Clear window  */
    return(0);
} /*  allfree */
