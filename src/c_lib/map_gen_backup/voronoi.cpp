#include "voronoi.hpp"

priority_queue<point,  vector<point>,  gt> points; // site events
priority_queue<event*, vector<event*>, gt> events; // circle events

//int main()
//{
   //// Read points from input.
   //point p;
   //while (cin >> p.x >> p.y) {
      //points.push(p);

      //// Keep track of bounding box size.
      //if (p.x < X0) X0 = p.x;
      //if (p.y < Y0) Y0 = p.y;
      //if (p.x > X1) X1 = p.x;
      //if (p.y > Y1) Y1 = p.y;
   //}
   //// Add 20% margins to the bounding box.
   //double dx = (X1-X0+1)/5.0; double dy = (Y1-Y0+1)/5.0;
   //X0 -= dx; X1 += dx; Y0 -= dy; Y1 += dy;

   //// Process the queues; select the top element with smaller x coordinate.
   //while (!points.empty())
      //if (!events.empty() && events.top()->x <= points.top().x)
         //process_event();
      //else
         //process_point();

   //// After all points are processed, do the remaining circle events.
   //while (!events.empty())
      //process_event();

   //finish_edges(); // Clean up dangling edges.
   //print_output(); // Output the voronoi diagram.
//}

/* STEVE */
inline int update_bounding_box(point p) {
    // bounding box
    int changed=0;
    if (p.x < X0) {
        X0 = p.x;
        changed=1;
    }
    if (p.y < Y0) {
        Y0 = p.y;
        changed=1;
    }
    if (p.x > X1) {
        X1 = p.x;
        changed=1;
    }
    if (p.y > Y1) {
        Y1 = p.y;
        changed=1;
    }
    return changed;
}

inline void set_margins() {
   // Add 20% margins to the bounding box.
   double dx = (X1-X0+1)/5.0; double dy = (Y1-Y0+1)/5.0;
   X0 -= dx; X1 += dx; Y0 -= dy; Y1 += dy;
}

void add_point(double x, double y) {

    point p;
    p.x = x;
    p.y = y;
    
    if (update_bounding_box(p)) set_margins();
}

void load_points(point *pts, int n_pts) {

    int i;
    for (i=0; i<n_pts; i++) {
        points.push(pts[i]);
        update_bounding_box(pts[i]);
    }

    set_margins();
}

void process_points() {
   // Process the queues; select the top element with smaller x coordinate.
   while (!points.empty())
      if (!events.empty() && events.top()->x <= points.top().x)
         process_event();
      else
         process_point();

   // After all points are processed, do the remaining circle events.
   while (!events.empty())
      process_event();

   finish_edges(); // Clean up dangling edges.
   //print_output(); // Output the voronoi diagram.

}
/* END STEVE */

void process_point()
{
   // Get the next point from the queue.
   point p = points.top();
   points.pop();

   // Add a new arc to the parabolic front.
   front_insert(p);
}

void process_event()
{
   // Get the next event from the queue.
   event *e = events.top();
   events.pop();

   if (e->valid) {
      // Start a new edge.
      seg *s = new seg(e->p);

      // Remove the associated arc from the front.
      arc *a = e->a;
      if (a->prev) {
         a->prev->next = a->next;
         a->prev->s1 = s;
      }
      if (a->next) {
         a->next->prev = a->prev;
         a->next->s0 = s;
      }

      // Finish the edges before and after a.
      if (a->s0) a->s0->finish(e->p);
      if (a->s1) a->s1->finish(e->p);

      // Recheck circle events on either side of p:
      if (a->prev) check_circle_event(a->prev, e->x);
      if (a->next) check_circle_event(a->next, e->x);
   }
   delete e;
}

void front_insert(point p)
{
   if (!root) {
      root = new arc(p);
      return;
   }

   // Find the current arc(s) at height p.y (if there are any).
   for (arc *i = root; i; i = i->next) {
      point z, zz;
      if (intersect(p,i,&z)) {
         // New parabola intersects arc i.  If necessary, duplicate i.
         if (i->next && !intersect(p,i->next, &zz)) {
            i->next->prev = new arc(i->p,i,i->next);
            i->next = i->next->prev;
         }
         else i->next = new arc(i->p,i);
         i->next->s1 = i->s1;

         // Add p between i and i->next.
         i->next->prev = new arc(p,i,i->next);
         i->next = i->next->prev;

         i = i->next; // Now i points to the new arc.

         // Add new half-edges connected to i's endpoints.
         i->prev->s1 = i->s0 = new seg(z);
         i->next->s0 = i->s1 = new seg(z);

         // Check for new circle events around the new arc:
         check_circle_event(i, p.x);
         check_circle_event(i->prev, p.x);
         check_circle_event(i->next, p.x);

         return;
      }
   }

   // Special case: If p never intersects an arc, append it to the list.
   arc *i;
   for (i = root; i->next; i=i->next) ; // Find the last node.

   i->next = new arc(p,i);  
   // Insert segment between p and i
   point start;
   start.x = X0;
   start.y = (i->next->p.y + i->p.y) / 2;
   i->s1 = i->next->s0 = new seg(start);
}

// Look for a new circle event for arc i.
void check_circle_event(arc *i, double x0)
{
   // Invalidate any old event.
   if (i->e && i->e->x != x0)
      i->e->valid = false;
   i->e = NULL;

   if (!i->prev || !i->next)
      return;

   double x;
   point o;

   if (circle(i->prev->p, i->p, i->next->p, &x,&o) && x > x0) {
      // Create new event.
      i->e = new event(x, o, i);
      events.push(i->e);
   }
}

// Find the rightmost point on the circle through a,b,c.
bool circle(point a, point b, point c, double *x, point *o)
{
   // Check that bc is a "right turn" from ab.
   if ((b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) > 0)
      return false;

   // Algorithm from O'Rourke 2ed p. 189.
   double A = b.x - a.x,  B = b.y - a.y,
          C = c.x - a.x,  D = c.y - a.y,
          E = A*(a.x+b.x) + B*(a.y+b.y),
          F = C*(a.x+c.x) + D*(a.y+c.y),
          G = 2*(A*(c.y-b.y) - B*(c.x-b.x));

   if (G == 0) return false;  // Points are co-linear.

   // Point o is the center of the circle.
   o->x = (D*E-B*F)/G;
   o->y = (A*F-C*E)/G;

   // o.x plus radius equals max x coordinate.
   *x = o->x + sqrt( pow(a.x - o->x, 2) + pow(a.y - o->y, 2) );
   return true;
}

// Will a new parabola at point p intersect with arc i?
bool intersect(point p, arc *i, point *result)
{
   if (i->p.x == p.x) return false;

   double a,b;
   if (i->prev) // Get the intersection of i->prev, i.
      a = intersection(i->prev->p, i->p, p.x).y;
   if (i->next) // Get the intersection of i->next, i.
      b = intersection(i->p, i->next->p, p.x).y;

   if ((!i->prev || a <= p.y) && (!i->next || p.y <= b)) {
      result->y = p.y;

      result->x = (i->p.x*i->p.x + (i->p.y-result->y)*(i->p.y-result->y) - p.x*p.x)
                / (2*i->p.x - 2*p.x);

      return true;
   }
   return false;
}

// Where do two parabolas intersect?
point intersection(point p0, point p1, double l)
{
   point res, p = p0;

   double z0 = 2*(p0.x - l);
   double z1 = 2*(p1.x - l);

   if (p0.x == p1.x)
      res.y = (p0.y + p1.y) / 2;
   else if (p1.x == l)
      res.y = p1.y;
   else if (p0.x == l) {
      res.y = p0.y;
      p = p1;
   } else {
      // Use the quadratic formula.
      double a = 1/z0 - 1/z1;
      double b = -2*(p0.y/z0 - p1.y/z1);
      double c = (p0.y*p0.y + p0.x*p0.x - l*l)/z0
               - (p1.y*p1.y + p1.x*p1.x - l*l)/z1;

      res.y = ( -b - sqrt(b*b - 4*a*c) ) / (2*a);
   }
   // Plug back into one of the parabola equations.
   res.x = (p.x*p.x + (p.y-res.y)*(p.y-res.y) - l*l)/(2*p.x-2*l);
   return res;
}

void finish_edges()
{
   // Advance the sweep line so no parabolas can cross the bounding box.
   double l = X1 + (X1-X0) + (Y1-Y0);

   // Extend each remaining segment to the new parabola intersections.
   for (arc *i = root; i->next; i = i->next)
      if (i->s1)
         i->s1->finish(intersection(i->p, i->next->p, l*2));
}

void print_output()
{
   // Bounding box coordinates.
   //cout << "Bounding box:" << endl;
   //cout << X0 << " "<< X1 << " " << Y0 << " " << Y1 << endl;
   cout << "b=[" << X0 << ", " << X1 << ", " << Y0 << ", " << Y1 << "]" << endl;
   cout << "v=[" << endl;

   // Each output segment in four-column format.
   vector<seg*>::iterator i;
   for (i = output.begin(); i != output.end(); i++) {
      point p0 = (*i)->start;
      point p1 = (*i)->end;
      cout << "[" << p0.x << ", " << p0.y << ", " << p1.x << ", " << p1.y << "]," << endl;
   }
   cout << "]" << endl;
}
#include "map.hpp"

#define V_SAMPLES 25*25

//using namespace noise::module;

//struct Point {
    //int x;
    //int y;
//};

//struct Point v_points[V_SAMPLES];

point v_points[V_SAMPLES];


/* Random methods */

inline int randx() {
    return rand() % XMAX;
}
inline int randy() {
    return rand() % YMAX;
}
inline int randz() {
    return rand() % ZMAX;
}

// returns random double in range [0,1]
inline double randd() {
    double d = ((double)rand()/(double)RAND_MAX);
    return d;
}

inline double randdx() {
    double d = randd() * XMAX;
    return d;
}
inline double randdy() {
    double d = randd() * YMAX;
    return d;
}
inline double randdz() {
    double d = randd() * ZMAX;
    return d;
}


void set_seed(int seed) {
    srand((unsigned)seed);
    //Voroni::SetSeed(seed);
}

// fill v_points array with random integer points
//void init_v_points() {
    //int i;
    //struct Point* p;
    //for (i=0; i<V_SAMPLES; i++) {
        //p = &v_points[i];
        //if (p == NULL) {
            //p = (struct Point*) malloc(sizeof(struct Point));
            //v_points[i] = p;
        //}
        //p->x = randx();
        //p->y = randy();
    //}
//}
void init_v_points() {
    int i;
    point p;
    for (i=0; i<V_SAMPLES; i++) {
        p.x = randdx();
        p.y = randdy();
        v_points[i]=p;
        //printf("%Lf %Lf\n", p.x, p.y);
    }
}

// create random points
// generate voroni polygons
// apply Lloyd -- move point to center of polygon
// do this N times, regenerating voroni each time
// see: http://en.wikipedia.org/wiki/Centroid

//double Voronoi::GetValue(x,y,z)  -- can i get the graph from this module?

// need to be able to extract region given a point

// then can apply Lloyd's and do all the map stuff

void load_v_points() {
    load_points(v_points, V_SAMPLES);
}


void start_voronoi() {
    init_v_points();
    load_v_points();
    process_points();
}