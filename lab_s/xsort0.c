/* RubberSort: Extra module */

#include <limits.h>
#include <X11/Xlib.h>
#include "xsort.h"

/* Rubber parameter passed from rubber module xsort1 */ 

static int* nfrag;       /* all fragments' number */
static XFragment* frag;  /* fragments' array address */
static int* extra;       /* extra fragment index array */
static XFragment* bak;   /* bak extra fragment */

/* Pass flex parameters from xsort1 */

int pass0(void* p[]) {
bak = (XFragment* ) p[0];
extra = (int* ) p[1];
frag = (XFragment* ) p[2];
nfrag = (int*) p[3];
return(0);
} /* pass0 */

/* min (left) fragment extra */

int miniextra() {
extra[0] = extra[1] = 0;
return(MINIFRAG);
} /* miniextra */

/* max (right) fragment extra */

int maxiextra() {
extra[0] = extra[1] = (*nfrag - 1);
return(MAXIFRAG);
} /* maxiextra */

/* mean (medium) fragment extra in size order */

int meanextra() {
extra[0] = extra[1] = ((*nfrag)/2);
return(MEANFRAG);
} /* meanextra */

/* 2 max differ fragments pair extra */

int diffextra() {
extra[0] = 0; extra[1] = *nfrag - 1;
return(DIFFFRAG);
} /* diffextra */

/* 2 max identical (beside) fragments pair extra */

int idenextra() {
int i, j;              /* fragment & fragment++ indexes */
int d;                 /* 2 beside fragments difference */
unsigned dmin=INT_MAX;  /* min difference or (1<<16)-1 */
int e[2];                            /* work extra pair */
extra[0] = extra[1] = 0;               /* init extra pair */
for(i = 0, j = 1; j < nfrag[0]; i++, j++) { /* check by 2 */
  if((d = difrag(frag + j, frag + i)) < 0)   /* check abs */
    d -= d;              /* 2 beside fragments difference */
  if(d < dmin) {              /* check current difference */
    extra[0] = i; extra[1] = j; /* fix max ident pair now */
    dmin = d;                   /* fix min difference now */
  } /* if */
} /* for */
return(IDENFRAG);
} /* idenextra */

/* Devide fragments to 2 group by medium */

int grp2extra() {
extra[0] = extra[1] = (nfrag[0] / 2); /* extra medium fragment */
reggc(AGC);            /* fragment (after) group alt foreground */
return(GRP2FRAG);
} /* grp2extra */

/* Reset Extra fragment(s) with clear space for redraw */

int rextra(int n /* to cosmetic repass return */ ) {
static int (*extrafunc[])()  = { /* extra functions array */
             miniextra, maxiextra, meanextra, 
             diffextra, idenextra, grp2extra
}; /* extrafunc */
fragextra(extrafunc);         /* reset extra method */
purgextr(bak, frag+extra[0]); /* purge 1st bak & new extra space */
if(extra[0] != extra[1])      /* purge 2nd bak & new extra space */
  purgextr(bak+1, frag+extra[1]);
return(0);
} /* rextra */

/* Check extra fragment by index i */

int isextra(int i) {
return((i == extra[0]) || (i == extra[1]));
} /* isextra */
