/* Rubber Sort header file  */

/* Union Fragment types structure */

typedef union {
  XSegment seg[1];   /* line segment */
  XRectangle rec[1]; /* (fill) rectangle */
  XArc arc[1];       /* (fill) ellipse */
} XFragment; 

/* Color GC ground index */

#define NGC 5  /* GCs' number */
#define FGC 0  /* fore-ground */
#define RGC 1  /* rubber-ground */
#define EGC 2  /* extr-ground */
#define AGC 3  /* groupalt-ground */ 
#define BGC 4  /* back-ground color */

/* Extra function index */

#define LFRAG 0  /* min (Left sort) fragment */
#define RFRAG 1  /* max (Right sort) fragment */
#define MFRAG 2  /* mean (Medium sort) fragment */
#define DFRAG 3  /* 2 max differ (Left & Right) fragments */
#define IFRAG 4  /* 2 max identical (beside sort) fragments */
#define GFRAG 1  /* devide to 2 group fragments */

/* Rubber functions xsort1 */

int pass(Display*, Window, GC*);   /* pass graphic parameters */
int pass1(Display*, Window, GC*);   /* pass graphic parameters */
int rubin(XEvent*);             /* new rubber or del fragment */ 
int rubout(XEvent*);                  /* rubout(del) fragment */
int rerub(XEvent*);              /* deformate rubber fragment */
int fix(XEvent*);                      /* fix rubber fragment */
int widewin();                               /* extend window */
int miniwin();                  /* hint min window size to WM */
int fragcmp(const void*, const void*); /* compare 2 fragments */
int refrag();                         /* redraw all fragments */
int near(int, int);         /* find near fragment to xy point */
int cancet();                     /* cancel template fragment */
int rextra();                         /* reset extra fragment */
int purgextr(XFragment*, XFragment*);   /* purge extras space */

/* Extra types xsort1 */

int miniextra();                       /* mini extra fragment */
int maxiextra();                       /* maxi extra fragment */
int meanextra();                       /* mean extra fragment */
int diffextra();              /* 2 max differ extra fragments */
int idenextra();             /* 2 max similar extra fragments */
int grp2extra();            /* devide 2 group extra fragments */

/* Fragment functions */

int fragon(XFragment*, int, int);  /* check fragment hited on */
int difrag(XFragment*, XFragment*);  /* 2 fragment difference */
int fragsize(XFragment*);              /* check fragment size */
int frag0(XFragment*, int, int);    /* set fragment xy-origin */
int fragvar(XFragment*, int, int);   /* variate fragment size */
int fragmaxix(XFragment*);          /* fragment's max x point */
int fragmaxiy(XFragment*);          /* fragment's max y point */
int radical(int);                              /* root square */
int fragover(XFragment*, XFragment*); /* overlap 2 fragmentes */
int tinyfrag(XFragment*);               /* tiny fragment test */
int fragextra(int (*[])());     /* call fragment extra method */
int altfrag();          /* alt fragment group GC (group extra */

/* Draw Fragment Functions */

int XFixes(Display*, Window, GC, XFragment*, int);
int XContour(Display*, Window, GC, XFragment*);
int XExtra(Display*, Window, GC, XFragment*);
int XFix(Display*, Window, GC, XFragment*);

/* Resource & dispatch functions xsort2 */

int resmng(int, char*[]);           /* rgb resource managment */
int canvas();                                  /* main window */
int gcing();                             /* custom GC drawing */
int dispatch();                             /* dispatch event */
int expo(XEvent*);                 /* expose fragments window */
int rekey(XEvent*);                     /* key press reaction */
