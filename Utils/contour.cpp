#include "contour.h"

void ApproxContour(double *data, int dim_x, int dim_y, double *px, double *py, int n_levels, double *levels,
             void (*polyCB)(int, ISO_POINT *, double , int ))
  {

  GRID    grid;
  grid.data   = data;
  grid.dim_x  = dim_x;
  grid.dim_y  = dim_y;
  grid.px = px;
  grid.py = py;
  grid.polyCallBack = polyCB;

  /* Allocate buffers used to contain contour information */
  if ((grid.map = (char *)malloc ((dim_x + 1) * dim_y)) == NULL)
    {
    fprintf (stderr, "Contour(): unable to allocate buffer! (%d bytes)\n", (dim_x + 1) * dim_y);
    free ((char *) grid.map);
    return;
    }
  grid.list = (ISO_POINT *) malloc (2 * dim_x * dim_y * sizeof (ISO_POINT));
  if (grid.list == (ISO_POINT  *) NULL)
    {
    fprintf (stderr, "Contour(): unable to allocate buffer! (%d bytes)\n", 2 * dim_x * dim_y * sizeof (ISO_POINT));
    free ((char *) grid.map);
    return;
    }

  /* Generate contours, if not a uniform field. */
  double   sum, sum2, count;
  double   *u, *v, r;
  char    *s;
  int     first;

  sum = sum2 = count = 0.0;

  first = 1;
  s = grid.map;
  u = grid.data;
  v = u + grid.dim_x * grid.dim_y;
  for (int i = 0; i < grid.dim_x * grid.dim_y; i++, u++, v++, s++)
    {
    r = *u;
    sum   += r;
    sum2  += r * r;
    count += 1.0;

    if (first)
      {
      grid.max_value = grid.min_value = r;
      first = 0;
      }
    else if (grid.max_value < r)
      grid.max_value = r;
    else if (grid.min_value > r)
      grid.min_value = r;
    }

  grid.mean = sum / count;

  grid.max_value += 1.;
  grid.min_value -= 1.;

  grid.n_levels = n_levels;
  grid.levels = levels;

  if (grid.min_value != grid.max_value)
    startLine (&grid);

  /* Release data structures. */
  free ((char *) grid.map);
  free ((char *) grid.list);
  }

/* startLine -- Locate first point of contour lines by checking edges of
   gridded data set, then interior points, for each contour level. */
static void startLine(GRID *grid)
  {
  int  idx, edge;
  for (idx = 0; idx < grid->n_levels; idx++)
    {
    /* Clear flags */
    grid->contour_mode = (grid->levels[idx] >= grid->mean);
    memset (grid->map, 0, grid->dim_x * grid->dim_y);

    /* Check edges */
    for (edge = 0; edge < 4; edge++)
      startEdge (grid, idx, (uchar)edge);
    /* Check interior points */
    startInterior (grid, idx);
    }
  }

/* startEdge -- For a specified contour level and edge of gridded data set,
 *  check for (properly directed) contour line. */
static void startEdge(GRID *grid, int ilevel, uchar bearing)
  {
  IS_POINT   point1, point2;
  double   last, next;
  int   i, ds;

  switch (point1.bearing = bearing)
    {
    case EAST:
      point1.x = 0;
      point1.y = 0;
      ds = 1;
      break;
    case NORTH:
      point1.x = 0;
      point1.y = grid->dim_y - 2;
      ds = 1;
      break;
    case WEST:
      point1.x = grid->dim_x - 2;
      point1.y = grid->dim_y - 2;
      ds = -1;
      break;
    case SOUTH:
      point1.x = grid->dim_x - 2;
      point1.y = 0;
      ds = -1;
      break;
    }

  switch (point1.bearing)
    {
    /* Find first point with valid data. */
    case EAST:
    case WEST:
      next = getDataPoint (grid, &point1, SAME);
      memcpy ((char *) &point2, (char *) &point1, sizeof (IS_POINT));
      point2.x -= ds;

      for (i = 1; i < grid->dim_y; i++, point1.y = point2.y += ds)
		{
		last = next;
		next = getDataPoint (grid, &point1, NEXT);
		if (last >= grid->levels[ilevel] && grid->levels[ilevel] > next)
		  {
		  drawLine (grid, &point1, ilevel);
		  memcpy ((char *) &point1, (char *) &point2, sizeof (IS_POINT));
		  point1.x = point2.x + ds;
		  }
		}
      break;
      /* Find first point with valid data. */
    case SOUTH:
    case NORTH:
      next = getDataPoint (grid, &point1, SAME);
      memcpy ((char *) &point2, (char *) &point1, sizeof (IS_POINT));
      point2.y += ds;

      for (i = 1; i < grid->dim_x; i++, point1.x = point2.x += ds)
		{
		last = next;
		next = getDataPoint (grid, &point1, NEXT);
		if (last >= grid->levels[ilevel] && grid->levels[ilevel] > next)
		  {
		  drawLine (grid, &point1, ilevel);

		  memcpy ((char *) &point1, (char *) &point2, sizeof (IS_POINT));
		  point1.y = point2.y - ds;
		  }
		}
      break;
    }
  }

/* startInterior -- For a specified contour level, check for (properly
   directed) contour line for all interior data points. Do _not_ follow contour
   lines detected by the 'startEdge' routine. */
static void startInterior(GRID *grid, int ilevel)
  {
  IS_POINT   point;
  int  x, y;
  double   next, last;
  for (x = 1; x < grid->dim_x - 1; x++)
    {
    point.x = x;
    point.y = 0;
    point.bearing = EAST;
    next = getDataPoint (grid, &point, SAME);
    for (y = point.y; y < grid->dim_y; y++, point.y++)     
      {
      last = next;
      next = getDataPoint (grid, &point, NEXT);
	  
      if (last >= grid->levels[ilevel] && grid->levels[ilevel] > next)
		{
		if (!faceInUse (grid, &point, WEST))
		  {
		  drawLine (grid, &point, ilevel);
		  point.x = x;
		  point.y = y;
		  point.bearing = EAST;
		  }
		}
      }
    }
  }

/* drawLine -- Given in initial contour point by either 'startEdge' or 
   'startInterior', follow contour line until it encounters either an edge 
   or previously contoured cell. */
static void drawLine(GRID *grid, IS_POINT *point, int ilevel)
  {
  uchar   exit_bearing;
  uchar   adj, opp;
  double   fadj, fopp;
  
  initPoint (grid);
  
  for ( ;; )
    {
    /* Add current point to vector list. If either of the points is missing, return immediately (open contour). */
    if (!savePoint (grid, point, ilevel))
      {
      lastPoint (grid, ilevel);
      return;
      }
      /* Has this face of this cell been marked in use? If so, then this is a closed contour. */
    if (faceInUse (grid, point, WEST))
      {
      lastPoint (grid, ilevel);
      return;
      }
      /* Examine adjacent and opposite corners of cell; determine appropriate action. */
    markInUse (grid, point, WEST);
    
    fadj = getDataPoint (grid, point, ADJACENT);
    fopp = getDataPoint (grid, point, OPPOSITE);
    
    /* If either point is missing, return immediately (open contour). */
    if (isnanf (fadj) || isnanf (fopp))
      {
      lastPoint (grid, ilevel);
      return;
      }

    adj = (fadj <= grid->levels[ilevel]) ? 2 : 0;
    opp = (fopp >= grid->levels[ilevel]) ? 1 : 0;
    switch (adj + opp)
      {
      /* Exit EAST face. */
      case 0: 
		markInUse (grid, point, NORTH);
		markInUse (grid, point, SOUTH);
		exit_bearing = EAST;
		break;
		/* Exit SOUTH face. */
      case 1:
		markInUse (grid, point, NORTH);
		markInUse (grid, point, EAST);
		exit_bearing = SOUTH;
		break;
		/* Exit NORTH face. */
      case 2:
		markInUse (grid, point, EAST);
		markInUse (grid, point, SOUTH);
		exit_bearing = NORTH;
		break;
		/* Exit NORTH or SOUTH face, depending upon contour level. */
      case 3:
		exit_bearing = (grid->contour_mode) ? NORTH : SOUTH;
		break;
      }
	
    /* Update face number, coordinate of defining corner. */
    point->bearing = (point->bearing + exit_bearing) % 4;
    switch (point->bearing)
      {
      case EAST : point->x++; break;
      case NORTH: point->y--; break;
      case WEST : point->x--; break;
      case SOUTH: point->y++; break;
      }
    }
  }

/* markInUse -- Mark the specified cell face as contoured. This is necessary to
 *  prevent infinite processing of closed contours. see also:  faceInUse */
static void markInUse(GRID *grid, IS_POINT *point, uchar face)
  {
  face = (point->bearing + face) % 4;
  switch (face)
    {
    case NORTH:
    case SOUTH:
      grid->map[MXY_to_L (grid, point->x, point->y + (face == SOUTH ? 1 : 0))] |= NS_MAP;
      break;
    case EAST:
    case WEST:
      grid->map[MXY_to_L (grid, point->x + (face == EAST ? 1 : 0), point->y)] |= EW_MAP;    
      break;
    }
  }

/* faceInUse -- Determine if the specified cell face has been marked as 
 * contoured. This is necessary to prevent infinite processing of closed 
 * contours. see also: markInUse */
static uchar faceInUse(GRID *grid, IS_POINT *point, uchar face)
  {
  uchar   r;
  face = (point->bearing + face) % 4;
  switch (face)
    {
    case NORTH:
    case SOUTH:
      r = grid->map[MXY_to_L (grid, point->x, point->y + (face == SOUTH ? 1 : 0))] & NS_MAP;
      break;
    case EAST:
    case WEST:
      r = grid->map[MXY_to_L (grid, point->x + (face == EAST ? 1 : 0), point->y)] & EW_MAP;
      break;
    }
  return r;
  }

/* initPoint -- Initialize the contour point list. 
 * see also: savePoint, lastPoint  */
static void initPoint(GRID *grid)
  {
  grid->count = 0;
  }

/* lastPoint -- Generate the actual contour line from the contour point list.
 *  see also:  savePoint, initPoint  */
static void lastPoint(GRID *grid, int ilevel)
  {
  if (grid->count)
    grid->polyCallBack(grid->count, grid->list, grid->levels[ilevel], ilevel);
  }

/* savePoints -- Add specified point to the contour point list.
 *  see also:  initPoint, lastPoint */
static uchar savePoint(GRID *grid, IS_POINT *point, int ilevel)
  {
  double   last, next;
  double   x, y, ds;

  last = getDataPoint (grid, point, SAME);
  next = getDataPoint (grid, point, NEXT);
  
  /* Are the points the same value? */
  if (last == next)
    {
    fprintf (stderr, "(%2d, %2d, %d)  ", point->x, point->y, point->bearing);
    fprintf (stderr, "%8g  %8g  ", last, next);
    fprintf (stderr, "potential divide-by-zero!\n");
    return 0;
    }
  
  x = (double) point->x;
  y = (double) point->y;
  
  ds = (double) ((last - grid->levels[ilevel]) / (last - next));
  
  switch (point->bearing)
    {
    case EAST :                 y += ds;        break;
    case NORTH: x += ds;        y += 1.0;       break;
    case WEST : x += 1.0;       y += 1.0 - ds;  break;
    case SOUTH: x += 1.0 - ds;                  break;
    }
  
  /* Update to contour point list */
  int ix1, ix2;
  if ((double)point->x < x)
    {
    ix1 = point->x;
    ix2 = ix1 + 1;
    }
  else
    {
    ix2 = point->x;
    ix1 = ix2 - 1;
    }

  int iy1, iy2;
  if ((double)point->y < y)
    {
    iy1 = point->y;
    iy2 = iy1 + 1;
    }
  else
    {
    iy2 = point->y;
    iy1 = iy2 - 1;
    }

  grid->list[grid->count].x = grid->px[ix2] - ((x - (double)(ix2)) * (grid->px[ix1] - grid->px[ix2]));
  grid->list[grid->count].y = grid->py[iy2] - ((y - (double)(iy2)) * (grid->py[iy1] - grid->py[iy2]));

  /* Update counter */
  grid->count++;
  
  return 1;
  }

/* getDataPoint -- Return the value of the data point in specified corner of 
 * specified cell (the 'point' parameter contains the address of the
 *  top-left corner of this cell). */
static double getDataPoint(GRID *grid, IS_POINT *point, uchar corner)
  {
  int  dx, dy;
  int  offset;
  
  switch ((point->bearing + corner) % 4)
    {
    case SAME    :  dx = 0; dy = 0; break;
    case NEXT    :  dx = 0; dy = 1; break;
    case OPPOSITE:  dx = 1; dy = 1; break;
    case ADJACENT:  dx = 1; dy = 0; break;   
    }
  
  offset = XY_to_L (grid, point->x + dx, point->y + dy);
  if ((int) (point->x + dx) >= grid->dim_x || 
    (int) (point->y + dy) >= grid->dim_y || 
    (int) (point->x + dx) < 0 ||
    (int) (point->y + dy) < 0)
    {
    return  NaN;
    }
  else
    return grid->data[offset];
  }


