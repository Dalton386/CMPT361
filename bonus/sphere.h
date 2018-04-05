/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#include "vector.h"

typedef struct sphere {
  int index;               // identifies a sphere; must be greater than 0

  Point v1, v2, v3;
  Vector norm;

  Point center;
  float radius;

  float mat_ambient[3];    // material property used in Phong model
  float mat_diffuse[3];
  float mat_specular[3];
  float mat_shineness;

  float reflectance;       
  float transparency;

  struct sphere *next;
} Spheres;   // a list of spheres

// intersect ray with sphere
Spheres *intersect_scene(Point, Vector, Spheres *, Point *, bool);
// return the unit normal at a point on sphere
Vector sphere_normal(Point, Spheres *);
// add a sphere to the sphere list
Spheres *add_sphere(Spheres *, Point, Point, Point, Vector, Point, float, float [], float [], float [], float, float, float, int);

