#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <math.h>
#include <stlib.h>

static Display* dpy;
static GC gc[2];
static Window root;
static Window** box;
static unsigned BW=40;
static unsigned BH=40;
static unsigned w=400;
static unsigned h=400;
static unsigned NX,NY;
static XRectangle cell;             /* Box cell */
static char* mark[] = { "0", "1" }; /* Marker symbols */
static int copy[1000][1000];
static int busy[1000][1000];

static unsigned NX;
static unsigned NY;

/* dynamic memory allocation for all game desk array */
int alloc(unsigned _nx, unsigned _ny)
{
    void** b;
    int i;
    NX=_nx; NY=_ny;
    b=calloc(NY,sizeof(void*));
    for(i=0;i<NY;i++)
        b[i] = calloc(NX,sizeof(unsigned long));
    relink(b);
    return(0);
}

/* free allocated memory */
int dealloc( void** b)
{
    int i;
    for(i=0; i < NY; i++)
        free(b[i]);
    free(b);
    return(0);
}

int relink(void** b)
{
    box = (Window**) b;
    return(0);
}

void ones(int j,int i)
{
    XSetForeground(dpy, gc[0], 0xFF00FF);
    XDrawString(dpy, box[i][j], gc[0], cell.x, cell.y, mark[1], 1);
    XSetForeground(dpy, gc[0], 0xFFFFFF);
    busy[i][j]=1;
}

void zeros(int j,int i)
{
    XSetForeground(dpy, gc[0], 0xFF00FF);
    XDrawString(dpy, box[i][j], gc[0], cell.x, cell.y, mark[0], 1);
    XSetForeground(dpy, gc[0], 0xFFFFFF);
    busy[i][j]=0;
}
void onech(int j,int i)
{
    XSetForeground(dpy, gc[0], 0x00FFFF);
    XDrawString(dpy, box[i][j], gc[0], cell.x, cell.y, mark[0], 1);
    XSetForeground(dpy, gc[0], 0x000000);
    XDrawString(dpy, box[i][j], gc[0], cell.x, cell.y, mark[1], 1);
    busy[i][j]=1;
}
void zeroch(int j,int i)
{
    XSetForeground(dpy, gc[0], 0x00FFFF);
    XDrawString(dpy, box[i][j], gc[0], cell.x, cell.y, mark[1], 1);
    XSetForeground(dpy, gc[0], 0x000000);
    XDrawString(dpy, box[i][j], gc[0], cell.x, cell.y, mark[0], 1);
    busy[i][j]=0;
}
int rebox(XEvent* ev)
{
    int i, j;
    for(i=0;i<NX;i++)
        for(j=0;j<NY;j++)
            if(i==j)
                onech(i,j);
            else
                zeroch(i,j);
    return(0);
}

int kraska(int i, int j)
{
    XPoint points[4];
    points[0].y=0;
    points[0].x=0;
    points[1].x=40;
    points[1].y=0;
    points[2].x=40;
    points[2].y=40;
    points[3].x=0;
    points[3].y=40;
    XFillPolygon(dpy,box[i][j],gc[0], points, 4,Convex, CoordModeOrigin);
}

int rebeg()
{
    int i,j;
    for(i=0;i<NY;i++)
        for(j=0;j<NX;j++)
            if(copy[i][j]==1)
                if(busy[i][j]==1)
                {
                    XSetForeground(dpy, gc[0], 0x00FFFF);
                    kraska(i,j);
                    onech(j,i);
                    copy[i][j]=0;
                }
                else
                {
                    XSetForeground(dpy, gc[0], 0x00FFFF);
                    kraska(i,j);
                    zeroch(j,i);
                    copy[i][j]=0;
                }
}
int reset(XEvent* ev)
{
    rebeg();
    int i,j,x;
    for(i=0;i<NY;i++)
    {
        for(j=0;j<NX;j++)
            if(ev->xbutton.subwindow == box[i][j])
                break;
        if(ev->xbutton.subwindow == box[i][j])
            break;
    }


    XSetForeground(dpy, gc[0], 0xFFFFFF);
    for(x=0;x<NY;x++)
        if(busy[x][j]==1)
        {
            kraska(x,j);
            zeros(j,x);
            copy[x][j]=1;
        }

    for(x=0;x<NX;x++)
        if(busy[i][x]==1)
        {
            kraska(i,x);
            zeros(x,i);
            copy[i][x]=1;
        }
    kraska(i,j);
    ones(j,i);
    copy[i][j]=1;

    return(0);

}

int back()
{
    int i,j;
    for(i=0;i<NY;i++)
        for(j=0;j<NX;j++)
            if(copy[i][j]==1)
                if(busy[i][j]==1)
                {
                    XSetForeground(dpy, gc[0], 0x00FFFF);
                    kraska(i,j);
                    zeroch(j,i);
                    copy[i][j]=0;
                }
                else
                {
                    XSetForeground(dpy, gc[0], 0x00FFFF);
                    kraska(i,j);
                    onech(j,i);
                    copy[i][j]=0;
                }
}

int xcustom()
{
    int x,y;
    int dx,dy;
    int src;
    int depth;
    XSetWindowAttributes attr;
    XSizeHints hint;
    XFontStruct* fn;           /* Font parameters structure */
    char* fontname = "9x15";   /* default font name */
    int i,j;

    src = DefaultScreen(dpy);
    depth = DefaultDepth(dpy, src);
    gc[0] = DefaultGC(dpy, src);

/* Font custom */

    if((fn = XLoadQueryFont(dpy, fontname)) == NULL)
        return(puts("Incorrect FontStruct id"));
    XSetFont(dpy, gc[0], fn->fid);

    cell.width = fn->max_bounds.width;
    cell.height = fn->max_bounds.ascent + fn->max_bounds.descent;
    cell.x = (BW -  fn->max_bounds.width)/2;
    cell.y = BH/2 + (fn->max_bounds.ascent - fn->max_bounds.descent)/2;

/* Main root window */

    w = NX*BW;
    h = NY*BH;
    attr.override_redirect = False;
    attr.background_pixel = 0xFFFFFF;
    attr.event_mask = (ButtonPressMask | ButtonReleaseMask | KeyPressMask);
    x=0; y=0;
    root = XCreateWindow(dpy, DefaultRootWindow(dpy), x, y, w, h,
                         1, depth, InputOutput, CopyFromParent,
                         (CWOverrideRedirect | CWBackPixel | CWEventMask),&attr);
    hint.flags = (PMinSize | PMaxSize | PPosition);
    hint.min_width = hint.max_width = w;
    hint.min_height = hint.max_height = h;
    hint.x=x; hint.y=y;
    XSetNormalHints(dpy, root, &hint);

    attr.override_redirect = True;
    attr.background_pixel = 0x00FFFF;
    attr.event_mask = (KeyPressMask | ExposureMask );
    w = BW; h = BH;
    for(i=0, y=0; i < NY; i++,y+=h)
    {
        for(j=0, x=0 ; j < NX; j++, x+=w)
            box[i][j] = XCreateWindow(dpy, root, x, y, w, h, 1,
                                      depth, InputOutput, CopyFromParent,
                                      (CWOverrideRedirect | CWBackPixel | CWEventMask), &attr);
    }

/* Display windows */

    XMapWindow(dpy, root);
    XMapSubwindows(dpy, root);
    for(i=0; i<NY; i++)
        for(j=0; j<NY; j++)
            XMapSubwindows(dpy, box[i][j]);
    XStoreName(dpy, root, "pictogramm");

/* Create clear GC */

    gc[1] = XCreateGC(dpy, root, 0, 0);
    XCopyGC(dpy, gc[0], GCFont, gc[1]);
    XSetForeground(dpy, gc[1], 0x00FFFF);

    return(0);
}

/* KeyBoard Driver */

int kbdrive(XEvent* ev)
{
    KeySym sym;
    XLookupString((XKeyEvent*) ev, NULL, 0, &sym, NULL);
    switch(sym)
    {
        case XK_Escape: back();
            break;
        case XK_d:
        case XK_D: if(ev->xkey.state & ControlMask)
                return(1);
            break;
        default:        break;
    }
    return(0);
}

/* Event dispatcher */

int dispatch()
{
    XEvent event;
    int flag = 0;
    while(flag == 0)
    {
        XNextEvent(dpy, &event);
        switch(event.type)
        {
            case Expose:      rebox(&event);
                break;
            case ButtonPress: reset(&event);
                break;
            case KeyPress:    flag = kbdrive(&event);
                break;
            default:          break;
        }
    }
    return(0);
}

/* Main function */

int main(int argc, char* argv[])
{
    if(argc < 2)
        fprintf(stderr,"Default: pictogramm 400x400+40+40\n");
    XParseGeometry(argv[1], &BW, &BH, &w, &h);
    NX=w/BW;
    NY=h/BH;
    alloc(NX, NY);
    dpy = XOpenDisplay(NULL);
    if(xcustom() > 0)
        return(1);
    dispatch();
    XDestroySubwindows(dpy, root);
    XDestroyWindow(dpy, root);
    XCloseDisplay(dpy);
    dealloc( box);
    return(0);
}
