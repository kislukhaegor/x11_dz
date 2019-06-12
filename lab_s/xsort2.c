/* Sort Rubber resource & dispatch main module */

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <string.h>
#include <stdio.h>
#include "xsort.h"

static Display* dpy;              /* Graphic Display */
static GC gc[NGC];                /* all Graphic Context */
static Window win;                /* root & main windows id */

/* RGB Resource managment */

int resmng(int argc, char* argv[]) {
    int scr;                       /* screen number */
    Window root;                   /* screen root window */
    Colormap cmap;                 /* color paletter */
    XColor rgb, exact;             /* color structure */
    int i;                         /* Resource index */
    XrmDatabase rdb=NULL;          /* Resource data base */
    char restype[64];              /* Resource type space */
    char* rtype = restype;         /* resourse type (String) */
    XrmValue resval;               /* resource value space */
    XrmValue* rval = &resval;      /* resource value (size & addr) */
    static char* rname[] = {       /* resource spec names */
            "xsort.foreground",  /* -fg */
            "xsort.rubbground",  /* -rg */
            "xsort.extrground",  /* -eg */
            "xsort.galtground",  /* -ag */
            "xsort.background"   /* -bg */
    }; /* rname */
    static char* rdef[] = {        /* defaults color resource */
            "orange",       /* foreground all fragments */
            "orange",       /* rubber fragment color */
            "pinr",      /* extra fragment color */
            "orange",      /* group altforeground fragment color */
            "gray"       /* window background */
    }; /* rdef in rname order */
    static XrmOptionDescRec rtab[] = { /* Resource Command Options */
            {"-fg ", ".foreground", XrmoptionSepArg, NULL},
            {"-rg ", ".rubbground", XrmoptionSepArg, NULL},
            {"-eg ", ".extrground", XrmoptionSepArg, NULL},
            {"-ag ", ".galtground", XrmoptionSepArg, NULL},
            {"-bg ", ".background", XrmoptionSepArg, NULL},
            {"-xrm ", NULL, XrmoptionResArg, NULL}
    }; /* rtab[] */

/* Check display defaults */

    dpy = XOpenDisplay(NULL);
    scr = DefaultScreen(dpy);
    cmap = DefaultColormap(dpy, scr);
    root = DefaultRootWindow(dpy);

/* Load resourses data base */

    XrmInitialize();
    rdb = XrmGetFileDatabase(".XSort");                     /* rdb <- .XSort */
    XrmParseCommand(&rdb, rtab, NGC+1, "xsort", &argc, argv); /* argv -> rdb */

/* Extract resourses from data base to GC */

    puts("\nColor resources:");
    for(i=0; i < NGC; i++) {
        if(XrmGetResource(rdb, rname[i], NULL, &rtype, rval) == False)
            rval->addr = rdef[i];        /* set default for missing resource */
/* or append resource to rdb from default */
/* while(XrmGetResource(rdb, rname[i], NULL, &rtype, rval) == False) */
/*   XrmPutStringResource(&rdb, rname[i], rdef[i]); */
/* or rval->addr = rdef[i]; rval->size = strlen(rdef[i]);  */
/*  XrmPutResource(&rdb, rname[i], "String", rval); */
        if(XParseColor(dpy, cmap, rval->addr, &rgb) == 0)
            if(XLookupColor(dpy, cmap, (rval->addr = rdef[i]), &rgb, &exact) == 0)
                memcpy(&rgb, &exact, sizeof(XColor));
        printf("%s(%s): %s\n", rname[i], rtab[i].option, rval->addr); /* Echo */
        fflush(stdout);
        XAllocColor(dpy, cmap, &rgb);
        gc[i] = XCreateGC(dpy, root, 0, 0);
        XSetForeground(dpy, gc[i], rgb.pixel);
    } /* for */
    if(rdb != NULL)            /* Free RDB space */
        XrmDestroyDatabase(rdb);

/* Correct background context */

    for(i=0; i < NGC; i++)  /* Set background is last rgb */
        XSetBackground(dpy, gc[i], rgb.pixel);
    return(0);
} /* resmng */

/* Correct rubber GC */

int gcing() {
    XGCValues gval;       /* Graphic Context values structure */
    unsigned long gmask=GCLineWidth;  /* Graphic Context mask */
    int i;                                        /* GC index */
    gval.line_width = 3;                    /* set bold lines */
    for(i=0; i < NGC; i++)                     /* for all GCs */
        XChangeGC(dpy, gc[i], gmask, &gval);
    gmask = (GCFunction | GCLineWidth | GCLineStyle);
    gval.line_width = 1;              /* set slim, dash & XOR */
    gval.function = GXxor;                   /* for rubber GC */
    gval.line_style = LineOnOffDash; /* worse LineDoubleDash; */
    XChangeGC(dpy, gc[RGC], gmask, &gval);/* Change Rubber GC */
    return(0);
} /* gcing */

/* Create & display main 640x480 window  */

int canvas() {
    XSetWindowAttributes attr;     /* main window attributes structure */
    unsigned long amask;           /* attribute mask */
    unsigned long emask;           /* event mask */
    Window root;                   /* screen root window */
    XGCValues gval;                /* Graphic Context values structure */
    XGetGCValues(dpy, gc[BGC], GCBackground, &gval); /* Get background */
    attr.background_pixel = gval.background;     /* from GC for window */
    attr.override_redirect = False;                 /* with WM support */
    attr.bit_gravity = NorthWestGravity;        /* reconfig Anti-blink */
/* or  attr.bit_gravity = StaticGravity; */
    amask = (CWOverrideRedirect | CWBackPixel | CWBitGravity);
    root = DefaultRootWindow(dpy);
    win = XCreateWindow(dpy, root, 0, 0, 640, 480, 1, CopyFromParent,
                        InputOutput, CopyFromParent, amask, &attr);
    XStoreName(dpy, win, "xsort");                     /* Window title */
    emask = (ButtonPressMask | ButtonReleaseMask | Button1MotionMask |
             ExposureMask | KeyPressMask);
    XSelectInput(dpy, win, emask); /* Select events' types for dispath */
    XMapWindow(dpy, win);                  /* display window on screen */
    return(0);
} /* canvas */

/* Redraw Fragments when Expose with clip window  */

int expo(XEvent* ev) {
    static XRectangle clip[32];  /* clip expo-buffer */
    static int n=0;              /* clib expo-count  */
    clip[n].x = ev->xexpose.x;        /* accumulate exposed */
    clip[n].y = ev->xexpose.y;        /* rectangles in clip */
    clip[n].width = ev->xexpose.width;        /* buffer for */
    clip[n].height = ev->xexpose.height;       /* redrawing */
    n++;                            /* increment clip count */
    if((ev->xexpose.count > 1) && (n < (32-1)))  /* with no */
        return(0);                        /* redrawing return */
    XSetClipRectangles(dpy, gc[FGC], 0, 0, clip, n, Unsorted);
    XSetClipRectangles(dpy, gc[EGC], 0, 0, clip, n, Unsorted);
    refrag();                       /* redraw all fragments */
    XSetClipMask(dpy, gc[FGC], None);   /* restore fore- &  */
    XSetClipMask(dpy, gc[EGC], None);   /* extra-grount GCs */
    return(n=0);          /* return with zeroing clip count */
} /* refrag */

/* Key exit or clean driver */

int rekey(XEvent* ev) {
    KeySym ks = XLookupKeysym((XKeyEvent*) ev, 1);
    if((ks == XK_F) && (ev->xkey.state == ControlMask))
        return('F');                     /* CTRL-F to return exit-code */
    if(ks != XK_Escape)       /* Nothing to do if any key except ESC */
        return(0);
    allfree();                               /* Escape all fragments */
    return(0);
} /* rekey */

/* Dispatch event function */

int dispatch() {
    int done = 0;                  /* event loop done flag (= false) */
    XEvent event;                  /* graphic event structure */
    while(done == 0) {                /* event loop */
        XNextEvent(dpy, &event);        /* Read next event */
        switch(event.type) {            /* check event type */
            case Expose: expo(&event);   /* redraw all exposed fragments */
                break;
            case ButtonPress: /* begin new rubber or delete fragment */
                if(rubin(&event) == Button3) /* new fragment */
                    rubout(&event);     /* delete any fragment */
                break;
            case MotionNotify: rerub(&event);    /* reform rubber fragment */
                break;
            case ButtonRelease: /* end rubber fragment */
                if(fix(&event) > 0)    /* fix new fragment */
                    rextra(photo()); /* reset extra fragment */
                refrag();          /* redraw all fragments */
                miniwin();       /* min window size for WM */
                break;
            case KeyPress: done = rekey(&event);    /* Check quit or clear */
                break;
            default: break;
        } /* swith */
    } /* while */
    XDestroyWindow(dpy, win); /* Close main window */
    XCloseDisplay(dpy);       /* Disconnect X-server */
    return(0);
} /* dispatch */

/* main function */

int main(int argc, char* argv[]) {
    resmng(argc, argv);  /* manage color resources */
    gcing();             /* set all GC */
    canvas();            /* create main window */
    pass1(dpy, win, gc); /* pass graphics to xsort1 */
    dispatch();          /* dispath events */
    return(0);
} /* main */
