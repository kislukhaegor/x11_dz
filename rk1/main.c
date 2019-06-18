#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

typedef struct {
    Window window;
    int value;
} MatrixCell;

typedef struct Matrix {
    size_t rows;
    size_t cols;
    MatrixCell** mass;
} Matrix;

Matrix* create_matrix(size_t rows, size_t cols) {
    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->mass = (MatrixCell**)malloc(sizeof(MatrixCell*) * rows);
    for (size_t i = 0; i < rows; ++i) {
        matrix->mass[i] = (MatrixCell*)calloc(cols, sizeof(MatrixCell));
    }
    return matrix;
}

static void free_mass(MatrixCell** mass, size_t n_rows) {
    for (size_t i = 0; i < n_rows; ++i) {
        free(mass[i]);
    }
    free(mass);
}

int free_matrix(Matrix* matrix) {
    for (size_t i = 0; i < matrix->rows; ++i) {
        free(matrix->mass[i]);
    }
    free(matrix->mass);
    free(matrix);
    return 0;
}

int fill_adj_matrix(Matrix* adj_matrix, const Matrix* matrix, size_t i_coor, size_t j_coor) {
    size_t i_adj = 0;
    size_t j_adj = 0;
    for (size_t i = 0; i < i_coor; ++i) {
        j_adj = 0;
        for (size_t j = 0; j < j_coor; ++j) {
            adj_matrix->mass[i_adj][j_adj] = matrix->mass[i][j];
            ++j_adj;
        }
        for (size_t j = j_coor + 1; j < matrix->cols; ++j) {
            adj_matrix->mass[i_adj][j_adj] = matrix->mass[i][j];
            ++j_adj;
        }
        ++i_adj;
    }
    for (size_t i = i_coor + 1; i < matrix->rows; ++i) {
        j_adj = 0;
        for (size_t j = 0; j < j_coor; ++j) {
            adj_matrix->mass[i_adj][j_adj] = matrix->mass[i][j];
            ++j_adj;
        }
        for (size_t j = j_coor + 1; j < matrix->cols; ++j) {
            adj_matrix->mass[i_adj][j_adj] = matrix->mass[i][j];
            ++j_adj;
        }
        ++i_adj;
    }
    return 0;
}

int det(const Matrix* matrix) {
    if (matrix->rows == 1) {
        return matrix->mass[0][0].value;
    }

    if (matrix->rows == 2) {
        return matrix->mass[0][0].value * matrix->mass[1][1].value - matrix->mass[1][0].value * matrix->mass[0][1].value;
    }

    Matrix* minor = create_matrix(matrix->rows - 1, matrix->cols - 1);
    int value = 0;
    for (size_t i = 0; i < matrix->cols; ++i) {
        fill_adj_matrix(minor, matrix, 0, i);
        double val = det(minor);
        value += matrix->mass[0][i].value * val * ((i%2) ? -1 : 1);
    }
    free_matrix(minor);
    return value;
}

typedef struct {
    Window window;
    unsigned width;
    int determinant;
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
    Matrix* matrix;
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
    XFillRectangle(context->display, context->matrix->mass[i][j].window, context->gc, 0, 0, size, size);
}

void draw_value(MatrixWindowContext* context, unsigned i, unsigned j) {
    XSetForeground(context->display, context->gc, context->background);
    clear_cell(context, i, j);
    static const char* syms[] = {"0", "1"};
    XSetForeground(context->display, context->gc, 0x0);
    int value = context->matrix->mass[i][j].value;
    XDrawString(context->display, context->matrix->mass[i][j].window, context->gc, context->let_x, context->let_y, syms[value], 1);
}

void clear_info_window(MatrixWindowContext* context) {
    unsigned width = context->info_window.width;
    unsigned height = context->height;
    XFillRectangle(context->display, context->info_window.window, context->gc, 0, 0, width, height);
}

void draw_determinant(MatrixWindowContext* context) {
    XSetForeground(context->display, context->gc, context->background);
    clear_info_window(context);

    char* str;
    size_t len = asprintf(&str, "Determinant: %d", context->info_window.determinant);
    XSetForeground(context->display, context->gc, 0x0);
    XDrawString(context->display, context->info_window.window, context->gc, context->let_x, context->let_y, str, len);
}

void update_determinant(MatrixWindowContext *context) {
    context->info_window.determinant = det(context->matrix);
    draw_determinant(context);
}

void draw_all(MatrixWindowContext* context) {
    for (int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            draw_value(context, i, j);
        }
    }
    draw_determinant(context);
}

void invert_cell(MatrixWindowContext* context, int i, int j) {
    int value = context->matrix->mass[i][j].value;
    context->matrix->mass[i][j].value = (value == 0) ? 1 : 0;
    draw_value(context, i, j);
}

void invert_matrix(MatrixWindowContext* context) {
    for (int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            invert_cell(context, i, j);
        }
    }
}

void clear_matrix(MatrixWindowContext* context) {
    for (int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            context->matrix->mass[i][j].value = 0;
        }
    }
    draw_all(context);
}

int handle_key(MatrixWindowContext* context, XEvent *ev) {
    KeySym sym;
    XLookupString((XKeyEvent*)ev, NULL, 0, &sym, NULL);
    switch (sym) {
        case XK_Alt_L:
        case XK_Alt_R: {
            invert_matrix(context);
            update_determinant(context);
            break;
        }
        case XK_Escape: {
            clear_matrix(context);
            update_determinant(context);
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

void handle_button(MatrixWindowContext* context, XEvent* ev) {
    for (int i = 0; i < context->dimension; ++i) {
        for (int j = 0; j < context->dimension; ++j) {
            if (context->matrix->mass[i][j].window == ev->xbutton.subwindow) {
                invert_cell(context, i, j);
            }
        }
    }
    update_determinant(context);
}

int dispatch(MatrixWindowContext* context) {
    XEvent event;
    while (True) {
        XNextEvent(context->display, &event);
        switch (event.type) {
            case Expose: {
                draw_all(context);
                break;
            }
            case ButtonPress: {
                handle_button(context, &event);
                break;
            }
            case KeyPress: {
                int ret = handle_key(context, &event);
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
    context->matrix = create_matrix(dimension, dimension);
    context->display = XOpenDisplay(NULL);
    return context;
}

void open_windows(MatrixWindowContext* context, MatrixWindowParams params) {
    context->width = params.main_width;
    context->height = params.main_height;
    context->cell_size = params.cell_size;
    context->background = params.background;
    context->info_window.width = params.info_width;

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
            context->matrix->mass[i][j].window = XCreateWindow(context->display, context->root, j * context->cell_size, i * context->cell_size, context->cell_size, context->cell_size, params.cell_border,
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
            XMapSubwindows(context->display, context->matrix->mass[i][j].window);
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
    free_matrix(context->matrix);
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
    dispatch(context);
    close_windows(context);
    free_context(context);
}