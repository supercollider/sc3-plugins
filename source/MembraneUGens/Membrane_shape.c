#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Membrane_shape.h"

#define MEMCHUNK 512
#define MAX_LINES (1024 * 16)
#define MAX_TMP MAX_LINES
#define QUIET

static int inCircle(int size, int x, int y);
static int inHexagon(int size, int x, int y);

typedef int (*inShapePtr)(int, int, int);

inShapePtr inShapes[SHAPE_N] = {inCircle, inHexagon};


static int inCircle(int size, int x, int y) {
  //printf("in? %dx%d\n", x, y);
  int result = 0;
  x = abs(x);
  y = abs(y);
  // converts to euclidean distance
  double y_ratio = sqrt(3);
  double fx = (double) x;
  double fy = (double) y;
  fy *= y_ratio;

  double distance = sqrt((fx * fx) + (fy * fy));
  if ((distance) < (size / 2)) {
    result = 1;
  }
  return(result);
}

//

static int inHexagon(int size, int x, int y) {
  int result = 0;
  x = abs(x);
  y = abs(y);

  if (x < size/2 && y < size/4) {
      if (x < size / 4) {
        result = 1;
      }
      else {
        if (x < ((size / 2) - y)) {
          result = 1;
        }
      }
  }
  return(result);
}

//

static void add(void **list, void *item, int position, int max_n) {
  if (position < (max_n - 1)) {
    list[position] = item;
  }
  else {
    printf("add reached maximum items (%d)\n", max_n);
    exit(1);
  }
}

//

extern t_shape *getShape(int shape_type, int size) {
  size = (size * 4) + 8;
  inShapePtr inShape = inShapes[shape_type % SHAPE_N];
  t_point *look;
  int possible[6][2];
  t_line **lines = calloc(MAX_LINES, sizeof(t_line *));
  int lines_n = 0;
  t_shape *result = (t_shape *) calloc(1, sizeof(t_shape));

  t_point **search = (t_point **) calloc(MAX_TMP, sizeof(t_point *));
  int search_n = 0;

  t_point **points = (t_point **) calloc(MAX_TMP, sizeof(t_point *));
  int points_n = 0;

  int i, j, x, y;
  t_line *line_p;
  t_point *point_p;
  int seen;
  int edge_n = 0;

  look = (t_point *)calloc(1, sizeof(t_point));
  look->x = 0;
  look->y = 0;
  look->id = 0;

  possible[0][0] =  1; possible[0][1] =  1;
  possible[1][0] =  2; possible[1][1] =  0;
  possible[2][0] =  1; possible[2][1] = -1;
  possible[3][0] = -1; possible[3][1] = -1;
  possible[4][0] = -2; possible[4][1] =  0;
  possible[5][0] = -1; possible[5][1] =  1;

  add((void *) points, (void *) look, points_n++, MAX_TMP);

  while(look != NULL) {
    //printf("looking: %dx%d\n", look->x, look->y);
    for (i = 0; i < 6; ++i) {
      x = look->x + possible[i][0];
      y = look->y + possible[i][1];

      if (! (*inShape)(size, x, y)) {
	if (!look->is_edge) {
	  look->is_edge = 1;
	  edge_n++;
	}
      }
      else {
        seen = 0;
        for (j = 0; j < points_n; ++j) {
          point_p = points[j];
          if (point_p->x == x && point_p->y == y) {
            seen = 1;
            break;
          }
        }
        if (!seen) {
          point_p = (t_point *)calloc(1, sizeof(t_point));
	  point_p->x = x;
	  point_p->y = y;
	  point_p->id = points_n;
	  add((void **) search,  (void *) point_p, search_n++,  MAX_TMP);
	  add((void **) points, (void *) point_p, points_n++, MAX_TMP);
        }

        // only save half the lines
        if (i < 3) {
	  line_p = calloc(1, sizeof(t_line));
	  line_p->a = look;
	  line_p->b = point_p;
	  add((void **) lines, (void *) line_p, lines_n++, MAX_LINES);
        }
      }
    }

    if (search_n == 0) {
      look = NULL;
      break;
    }
    else {
      look = search[--search_n];
    }
  }
  free(search);

  result->points   = points;
  result->points_n = points_n;
  result->lines    = lines;
  result->lines_n  = lines_n;
  result->edge_n = edge_n;


#ifndef QUIET
  printf("Made shape with %d lines, %d points and %d edge points.\n",
	 lines_n,
	 points_n,
	 edge_n
	 );
#endif

  return(result);
}

extern void free_shape(t_shape *shape) {
  int i;
  for (i = 0; i < shape->lines_n; ++i) {
    free(shape->lines[i]);
  }
  for (i = 0; i < shape->points_n; ++i) {
    free(shape->points[i]);
  }
  free(shape->lines);
  free(shape->points);
  free(shape);
}
