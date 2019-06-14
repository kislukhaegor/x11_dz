#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <math.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#define FPS 60
#define SPEED 10

static Display* dpy;              /* Graphic Display */
static GC gc[3];                /* all Graphic Context */
static Window win;                /* root & main windows id */
static int change=0;
static int l1,l2;
XPoint xp[3];

int xpp(){
    XWindowAttributes attrr;
    XGetWindowAttributes(dpy, win, &attrr);
    int l = (attrr.width / 3 < attrr.height / 2) ? attrr.width / 3 : attrr.height / 2;
    xp[0].x = attrr.width / 2 - l / 2;
    xp[0].y = attrr.height / 2;
    xp[1].x = attrr.width / 2 + l / 2;
    xp[1].y = attrr.height / 2;
    xp[2].x = attrr.width / 2;
    xp[2].y = attrr.height / 2;
    l1 = l2 = l/2;
    return 0;
}


int rekey(XEvent* ev) {
    char sym[1];               /* symbol ASCII code */
    KeySym code[1];            /* key pressed symbol X code */
    XLookupString((XKeyEvent* ) ev, NULL, 0, code, NULL);
    switch(code[0]) {
        case XK_h:
        case XK_H:
            if(ev->xkey.state == ControlMask)
                return(4);
        default:
            break;
    }/* switch */
    return(2);
}

int redraw(int jij){
    XWindowAttributes attrr;
    XGetWindowAttributes(dpy, win, &attrr);
    XClearWindow(dpy, win);
    int l = (attrr.width / 3 < attrr.height / 2) ? attrr.width / 3 : attrr.height / 2;
    if (change == 0)
        if ((attrr.height/2!=xp[0].y)||(attrr.width/2-l/2!=xp[0].x))
            xpp();
    if (change == 1)
        XWarpPointer(dpy, win, win,0,0,attrr.width,attrr.height,xp[2].x,attrr.height/2);
    xp[0].x = xp[2].x+l2*sin((jij+90)%360*M_PI/180);
    xp[0].y = attrr.height/2+l2*cos((jij+90)%360*M_PI/180);
    xp[1].x = xp[2].x+l1*sin((jij+270)%360*M_PI/180);
    xp[1].y = attrr.height/2+l1*cos((jij+270)%360*M_PI/180);
    XDrawLine(dpy, win, gc[1], xp[2].x, (attrr.height/2), xp[0].x, xp[0].y);
    XDrawLine(dpy, win, gc[1], xp[2].x, (attrr.height/2), xp[1].x, xp[1].y);
}

int main(int argc, char* argv[]) {
    int scr;
    /* Display Block */
    int jij = 0;
    int kk = 0;
    dpy = XOpenDisplay(NULL);
    scr = DefaultScreen(dpy);
    win = DefaultRootWindow(dpy);
    scr = DefaultScreen(dpy);
    gc[0] = XCreateGC(dpy, win, 0, 0);
    gc[1] = XCreateGC(dpy, win, 0, 0);
    gc[2] = XCreateGC(dpy, win, 0, 0);
    XSetBackground(dpy, gc[0], 0xFFFFFF);
    XSetForeground(dpy, gc[0], 0xFFFFFF);
    XSetBackground(dpy, gc[2], 0xFF0000);
    XSetForeground(dpy, gc[2], 0xFF0000);
    XGCValues gvall;
    gvall.line_style = LineOnOffDash;
    //XChangeGC(dpy, gc[1], GCLineStyle, &gvall);
    /* Display block */

    /* Window block  */
    XSetWindowAttributes attr;     /* main window attributes structure */
    unsigned long amask;           /* attribute mask */
    unsigned long emask;           /* event mask */
    Window root;                   /* screen root window */
    XGCValues gval;                /* Graphic Context values structure */
    XGetGCValues(dpy, gc[0], GCBackground, &gval); /* Get background */
    attr.background_pixel = gval.background;     /* from GC for window */
    attr.override_redirect = False;                 /* with WM support */
    attr.bit_gravity = NorthWestGravity;        /* reconfig Anti-blink */
    amask = (CWOverrideRedirect | CWBackPixel | CWBitGravity);
    root = DefaultRootWindow(dpy);
    win = XCreateWindow(dpy, root, 0, 0, 640, 480, 1, CopyFromParent, InputOutput, CopyFromParent, amask, &attr);
    XStoreName(dpy, win, "Lion");                     /* Window title */
    emask = (ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy, win, emask); /* Select events' types for dispath */
    XMapWindow(dpy, win);                  /* display window on screen */
    /* window block */
    xpp();
    /* Multi Block */
    int multi = 1;
    int press = 0;
//    XWindowAttributes attrr;
    XEvent event;
    while (multi > -1) {
//        XGetWindowAttributes(dpy, win, &attrr);
        event.type = 0;
        XCheckWindowEvent(dpy, win, emask, &event);
        switch (event.type) {
            case Expose:
                redraw(jij);
                break;
            case ConfigureNotify:
                redraw(jij);
                break;
            case KeyPress:
                switch (rekey(&event)) {
                    case 4:
                        multi = -2;
                        break;
                    default:
                        break;
                }/* switch */
                break;
            case ButtonPress:
                xp[2].x = event.xbutton.x;
                xp[2].y = event.xbutton.y;
                if (change == 0) {
                    if (xp[2].x > xp[0].x)
                        xp[2].x = xp[0].x;
                    else if (xp[2].x < xp[1].x)
                        xp[2].x = xp[1].x;
                    l1 = xp[2].x - xp[1].x;
                    l2 = xp[0].x - xp[2].x;
                    change = 1;
                }
                if (event.xbutton.button == Button1)
                    press = 1;
                if (event.xbutton.button == Button3)
                    press = -1;
                if (jij == 360)
                    jij = 0;
                break;
            case ButtonRelease:
                change = 0;
                press = 0;
                xpp();
                jij = 0;
                redraw(jij);
                break;
            default:
                break;
        } /* switch */
        if ((multi > 40) && (press == 1)) {
            multi = 1;
            jij++;
            redraw(jij);
        }
        if ((multi > 40) && (press == -1)) {
            multi = 1;
            jij--;
            redraw(jij);
        }
        multi++;
    } /* while */
    XDestroyWindow(dpy, win); /* Close main window */
    XCloseDisplay(dpy);       /* Disconnect X-server */
    return 0;
}
