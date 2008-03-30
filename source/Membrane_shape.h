
#ifdef __cplusplus
extern "C" {
#endif

#define SHAPE_N 2

typedef struct {
  int id;
  int x;
  int y;
  int is_edge;
} t_point;

typedef struct {
  t_point *a;
  t_point *b;
} t_line;

typedef struct {
  int shape_type;
  t_line **lines;
  int lines_n;
  t_point **points;
  int points_n;
  int edge_n;
} t_shape;

extern t_shape *getShape(int shape_type, int size);
extern void free_shape(t_shape *shape);

#ifdef __cplusplus
}
#endif
