int size = 12;
int maxsize = 12;
int scale = 4;
int ssize = maxsize * scale;
ArrayList[] edges = new ArrayList[2];

void setup() {
  size(ssize, ssize);
  edges[0] = findEdges(0);
  edges[1] = findEdges(1);
  background(0);
  frameRate(8);
}

ArrayList findEdges(int shape) {
  int[] start = {0, 0};
  int[] look = start;
  int[][] possible = new int[6][2];
  possible[0][0] =  1; possible[0][1] =  1;
  possible[1][0] =  2; possible[1][1] =  0;
  possible[2][0] =  1; possible[2][1] = -1;
  possible[3][0] = -1; possible[3][1] = -1;
  possible[4][0] = -2; possible[4][1] =  0;
  possible[5][0] = -1; possible[5][1] =  1;

  ArrayList result = new ArrayList();
  ArrayList search = new ArrayList();
  ArrayList visited = new ArrayList();

  visited.add(look);

  while(look != null) {
    int foo[] = {look[0], look[1]};
    for (int i = 0; i < 6; ++i) {
      int x = look[0] + possible[i][0];
      int y = look[1] + possible[i][1];
      boolean inShape;
      if (shape == 0) {
        inShape = inCircle(x, y);
      }
      else {
        inShape = inHexagon(x, y);
      }

      if (inShape) {
        // only save half the lines
        if (i < 3) {
          int[] line = {look[0], look[1], x, y};
          result.add(line);
        }
        boolean seen = false;
        for (int j = 0; j < visited.size(); ++j) {
          int[] test = (int[]) visited.get(j);
          if (test[0] == x && test[1] == y) {
            seen = true;
            break;
          }
        }
        if (!seen) {
          int[] addme = {x, y};
          search.add(addme);
          visited.add(addme);
        }
      }
    }
    if (search.isEmpty()) {
      look = null;
      break;
    }
    else {
      look = (int[]) search.remove(0);
    }
  }
  return(result);
}

boolean inCircle(int x, int y) {
  boolean result = false;
  x = abs(x);
  y = abs(y);
  // converts to euclidean distance
  float y_ratio = sqrt(3);
  float fx = (float) x;
  float fy = (float) y;
  fy *= y_ratio;

  double distance = sqrt((fx * fx) + (fy * fy));
  if ((distance) < (size / 2)) {
    result = true;
  }
  return(result);
}

boolean inHexagon(int x, int y) {
  boolean result = false;
  x = abs(x);
  y = abs(y);

  if (x < size/2 && y < size/4) {
      if (x < size / 4) {
        result = true;
      }
      else {
        if (x < ((size / 2) - y)) {
          result = true;
        }
      }
  }
  return(result);
}


void draw() {
  background(0);
  translate(ssize/2, ssize/2);
  noFill();
  stroke(#c0c0c0);
  //ellipse(0, 0, size * scale, size * scale);
  stroke(#d0d0d0);

  drawShape(edges[0], color(255, 0, 0));
  drawShape(edges[1], color(0, 255, 0));

  if (size < maxsize) {
    size++;
    edges[0] = findEdges(0);
    edges[1] = findEdges(1);
  }
}

void drawShape(ArrayList edges, color c) {
  float y_ratio = sqrt(3);
  stroke(c);
  for (Iterator i = edges.iterator(); i.hasNext();) {
    int[] edge = (int[]) i.next();
    int o = ssize / 2;
    line(edge[0] * scale,
         y_ratio * edge[1] * scale,
         edge[2] * scale,
         y_ratio * edge[3] * scale
         );
  }
}
