
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned int uint;

typedef struct {
  uint r;
  uint g;
  uint b;
} color;

color color_new(uint r, uint g, uint b) {
  return (color) {
    .r = r,
    .g = g,
    .b = b
  };
}

typedef struct {
  double r;
  double i;
} cpx;

cpx cpx_new(double r, double i) {
  return (cpx) {
    .r = r,
    .i = i,
  };
}

cpx cpx_sum(cpx c0, cpx c1) {
  return (cpx) {
    .r = c0.r + c1.r,
    .i = c0.i + c1.i
  };
}

cpx cpx_mul(cpx c0, cpx c1) {
  return (cpx) {
    .r = (c0.r * c1.r) - (c0.i * c1.i),
    .i = (c0.r * c1.i) + (c0.i * c1.r)
  };
}

cpx cpx_sqr(cpx c) {
  cpx_mul(c, c);
}

double cpx_abs(cpx c) {
  return sqrt(c.r * c.r + c.i * c.i);
}

#define MF_IT 100
double mf(cpx c) {
  cpx z = cpx_new(0, 0);
  cpx s;
  for(int i = 0; i < MF_IT; i++) {
    s = cpx_sqr(z);
    z = cpx_sum(s, c);
  }
  return cpx_abs(z);
}

#define WIDTH 600
#define HEIGHT 800

Display *d;
Window w;
XEvent e;
GC gc;
Colormap cmap;
const char *msg = "Hello, World!";
int s;

void draw_point(uint x, uint y) {
  printf("drawing at: %d, %d\n", x, y);
  if(x >= WIDTH) return;
  if(y >= HEIGHT) return;

  /*
  XColor xcolor;
  xcolor.red = c.r;
  xcolor.green = c.g;
  xcolor.blue = c.b;


  XAllocColor(d, cmap, &xcolor);
  XSetForeground(d, gc, xcolor.pixel);
  XFillRectangle(d, w, DefaultGC(d, s), x, y, 10, 10);
  */
  XDrawPoint(d, w, gc, x, y);
  XFlush(d);
}

void draw_rect(uint x, uint y, uint w, uint h) {
  if(x + w >= WIDTH) return;
  if(y + h >= HEIGHT) return;
  XFillRectangle(d, w, gc, x, y, w, h);
}

void clear_screen() {
  XClearWindow(d, w);
}

#define M_THRESH 10
void draw_mandel() {
  double sec_x = -1;
  double sec_y = 1;
  double sec_w = 2;
  double sec_h = 2;
  double d = 0.001;

  double dw = WIDTH / sec_w;
  double dh = HEIGHT / sec_h;
  for(double x = sec_x; x < sec_x + sec_w; x += d) {
    for(double y = sec_y; y > sec_y - sec_h; y -= d) {
      printf("mandelf(%f, %f): %f\n", x, y, mf(cpx_new(x, y)));
      if(mf(cpx_new(x, y)) <= M_THRESH) {
        double dx = fabs(x - sec_x) / sec_w;
        double dy = fabs(y + sec_y) / sec_h;

        draw_point(
          (int) (dx * WIDTH),
          (int) (dy * HEIGHT));
      }

    }
  }

  return;
}

int main(void) {


  d = XOpenDisplay(NULL);
  if (d == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, WIDTH, HEIGHT, 1,
                          BlackPixel(d, s), WhitePixel(d, s));
  XSelectInput(d, w, ExposureMask | KeyPressMask);
  XMapWindow(d, w);

  cmap = DefaultColormap(d, DefaultScreen(d));

  gc = DefaultGC(d, s);

  while (1) {
    XNextEvent(d, &e);
    if (e.type == Expose) {
      color c = (color) {
        .r = 90000,
        .g = 20000,
        .b = 30000,
      };
      draw_point(200, 10);
      draw_mandel();
    }
    if (e.type == KeyPress)
      break;
  }

  XCloseDisplay(d);
  return 0;
}
