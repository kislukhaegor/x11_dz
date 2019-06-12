/* Rubber Sort header file  */

/* Union Fragment types structure */

typedef union {
    // XSegment seg[1];   /* line segment */
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

#define MINIFRAG 0   /* min (Left sort) fragment */
#define MAXIFRAG 1   /* max (Right sort) fragment */
#define MEANFRAG 2   /* mean (Medium sort) fragment */
#define DIFFFRAG 3   /* 2 max differ (Left & Right) fragments */
#define IDENFRAG 4   /* 2 max identical (beside sort) fragments */
#define GRP2FRAG 5   /* devide to 2 group fragments */

/* Rubber functions xsort1 */

int pass1(Display*, Window, GC*);  /* pass graphic parameters */
int photo();          /* photo flex parameters to pass xsort0 */
int rebak();                     /* to baking extra fragments */
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
int purgextr(XFragment*, XFragment*);   /* purge extras space */
int reggc(int);             /* reset alter GC for group extra */

/* Extra types xsort0 */

int pass0(void**);   /* pass flex parameters list from xsort1 */
int miniextra();                       /* mini extra fragment */
int maxiextra();                       /* maxi extra fragment */
int meanextra();                       /* mean extra fragment */
int diffextra();              /* 2 max differ extra fragments */
int idenextra();             /* 2 max similar extra fragments */
int grp2extra();            /* devide 2 group extra fragments */
int rextra(int);                      /* reset extra fragment */
int isextra(int);               /* check extra fragment index */

/* Fragment functions */

int fragon(XFragment*, int, int, int);  /* check hit fragment */
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