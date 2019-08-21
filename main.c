
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned int uint;

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
  return cpx_mul(c, c);
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

#define WIDTH 800
#define HEIGHT 800

Display *d;
Window w;
XEvent e;
GC gc;
Colormap cmap;
int s;

void draw_point(uint x, uint y) {
  if(x >= WIDTH) return;
  if(y >= HEIGHT) return;

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
void draw_mandel(double sec_x, double sec_y, double sec_w, double sec_h) {
  double sx = 0;
  double sy = 0;
  for(int x = 0; x < WIDTH; x++) {
    for(int y = 0; y < HEIGHT; y++) {
      double dx = ((double) x) / WIDTH;
      double dy = ((double) y) / HEIGHT;
      sx = sec_x + (dx * sec_w);
      sy = sec_y - (dy * sec_h);
      if(mf(cpx_new(sx, sy)) <= M_THRESH) {
        draw_point(x, y);
      }
    }
  }

  return;
}

int main(int argc, char **argv) {
  if(argc <= 4) {
    printf("usage: ./mandel x y w h\n");
    return 0;
  }

  double sec_x;
  double sec_y;
  double sec_w;
  double sec_h;
  sscanf(argv[1], "%lf", &sec_x);
  sscanf(argv[2], "%lf", &sec_y);
  sscanf(argv[3], "%lf", &sec_w);
  sscanf(argv[4], "%lf", &sec_h);

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

  for(;;) {
    XNextEvent(d, &e);
    if (e.type == Expose) {
      draw_mandel(sec_x, sec_y, sec_w, sec_h);
    }
    if (e.type == KeyPress)
      break;
  }

  XCloseDisplay(d);
  return 0;
}
