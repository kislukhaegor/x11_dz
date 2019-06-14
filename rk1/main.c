#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

typedef struct {
    Window window;
    int value;
} MatrixCell;

typedef struct {
    Window window;
} InfoWindow;

typedef struct {
    Window root;
    Display* display;
    GC gc;
    unsigned background;
    unsigned width;
    unsigned height;
    unsigned cell_size;
    unsigned let_x;
    unsigned let_y;
    unsigned dimension;
    MatrixCell** sub_windows;
    InfoWindow info_window;
} MatrixWindowContext;

typedef struct {
    unsigned main_width;
    unsigned main_height;
    unsigned cell_size;
    unsigned cell_border;
    unsigned info_width;
    unsigned background;
} MatrixWindowParams;

MatrixWindowParams default_params() {
    MatrixWindowParams params = {
        .main_width = 600,
        .main_height = 400,
        .cell_size = 40,
        .cell_border = 2,
        .info_width = 200,
        .background = 0xFF80FD,
    };
    return params;
}

void load_font(MatrixWindowContext* context, const char* fontname) {
    XRectangle cell;
    XFontStruct* fn = XLoadQueryFont(context->display, fontname);
    if (fn == NULL) {
        puts("Incorrect FontStruct id");
        return;
    }
    XSetFont(context->display, context->gc, fn->fid);

    context->let_x = (context->cell_size -  fn->max_bounds.width) / 2;
    context->let_y = context->cell_size / 2 + (fn->max_bounds.ascent - fn->max_bounds.descent) / 2;
}

void clear_cell(MatrixWindowContext* context, unsigned i, unsigned j) {
    unsigned size = context->cell_size;
    XFillRectangle(context->display, context->sub_windows[i][j].window, context->gc, 0, 0, size, size);
}

void draw_value(MatrixWindowContext* context, unsigned i, unsigned j) {
    XSetForeground(context->display, context->gc, context->background);
    clear_cell(context, i, j);
    static const char* syms[] = {"0", "1"};
    XSetForeground(context->display, context->gc, 0xFFFFFF);
    int value = context->sub_windows[i][j].value;
    XDrawString(context->display, context->sub_windows[i][j].window, context->gc, context->let_x, context->let_y, syms[value], 1);
}

void draw_all(MatrixWindowContext* context) {
    for (int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            draw_value(context, i, j);
        }
    }
}

int kbdrive(XEvent *ev) {
    KeySym sym;
    XLookupString((XKeyEvent *) ev, NULL, 0, &sym, NULL);
    switch (sym) {
        case XK_Escape: {
            break;
        }
        case XK_q:
        case XK_Q: {
            return 1;
        }
        default: {
            break;
        }
    }
    return 0;
}

void handle_buttion(MatrixWindowContext* context, XEvent* ev) {
    for (int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            if (context->sub_windows[i][j].window == ev->xbutton.subwindow) {
                int value = context->sub_windows[i][j].value;
                context->sub_windows[i][j].value = value == 0 ? 1 : 0;
                draw_value(context, i, j);
            }
        }
    }
}

int dispatch(MatrixWindowContext* context) {
    XEvent event;
    while (true) {
        XNextEvent(context->display, &event);
        switch (event.type) {
//            case Expose: {
//                rebox(&event);
//                break;
//            }
            case ButtonPress: {
                handle_buttion(context, &event);
                break;
            }
            case KeyPress: {
                int ret = kbdrive(&event);
                if (ret) {
                    return ret;
                }
            }
        }
    }
    return 0;
}

MatrixWindowContext* create_matrix_window_context(int dimension) {
    MatrixWindowContext* context = (MatrixWindowContext*)calloc(1, sizeof(MatrixWindowContext));
    if (context == NULL) {
        return NULL;
    }
    context->dimension = dimension;
    context->sub_windows = (MatrixCell**)calloc(dimension, sizeof(MatrixCell*));
    for (int i = 0; i < dimension; ++i) {
        context->sub_windows[i] = (MatrixCell*)calloc(context->dimension, sizeof(MatrixCell));
    }
    context->display = XOpenDisplay(NULL);
    return context;
}

void open_windows(MatrixWindowContext* context, MatrixWindowParams params) {
    context->width = params.main_width;
    context->height = params.main_height;
    context->cell_size = params.cell_size;
    context->background = params.background;

    int scr = DefaultScreen(context->display);
    int depth = DefaultDepth(context->display, scr);
    context->gc = DefaultGC(context->display, scr);

    XSetWindowAttributes attr;
    attr.override_redirect = False;
    attr.background_pixel = params.background;
    attr.event_mask = (KeyPressMask | ButtonPressMask | ButtonReleaseMask);

    context->root = XCreateWindow(context->display, DefaultRootWindow(context->display), 0, 0, context->width, context->height,
                                1, depth, InputOutput, CopyFromParent,
                                (CWOverrideRedirect | CWBackPixel | CWEventMask), &attr);

    attr.override_redirect = True;
    attr.background_pixel = params.background;
    attr.event_mask = (KeyPressMask | ExposureMask);
    attr.win_gravity=NorthWestGravity;
    for(int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            context->sub_windows[i][j].window = XCreateWindow(context->display, context->root, j * context->cell_size, i * context->cell_size, context->cell_size, context->cell_size, params.cell_border,
                                              depth, InputOutput, CopyFromParent,
                                              (CWOverrideRedirect | CWBackPixel | CWEventMask | CWWinGravity), &attr);
        }
    }

    context->info_window.window = XCreateWindow(context->display, context->root, context->dimension * context->cell_size, 0, params.info_width, context->height, params.cell_border,
                                       depth, InputOutput, CopyFromParent,
                                       (CWOverrideRedirect | CWBackPixel | CWEventMask | CWWinGravity), &attr);

    XMapWindow(context->display, context->root);
    XMapSubwindows(context->display, context->root);
    for (int i = 0; i < context->dimension; i++) {
        for (int j = 0; j < context->dimension; j++) {
            XMapSubwindows(context->display, context->sub_windows[i][j].window);
        }
    }
    XMapSubwindows(context->display, context->info_window.window);
}

void close_windows(MatrixWindowContext* context) {
    XDestroySubwindows(context->display, context->root);
    XDestroyWindow(context->display, context->root);
    XCloseDisplay(context->display);
}

void free_context(MatrixWindowContext* context) {
    for (int i = 0; i < context->dimension; ++i) {
        free(context->sub_windows[i]);
    }
    free(context->sub_windows);
    free(context);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s dimension", argv[0]);
        return 0;
    }
    int dimension = atoi(argv[1]);

    MatrixWindowContext* context = create_matrix_window_context(dimension);
    open_windows(context, default_params());
    load_font(context, "9x15");
    draw_all(context);
    dispatch(context);
    close_windows(context);
    free_context(context);
}