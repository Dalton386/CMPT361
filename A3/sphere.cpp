#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#define INF 10e10
using namespace std;

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_sphere(Point o, Vector v, Spheres *sph, Point &check) {
  Point ctr = sph->center;
  float r = sph->radius;
  Vector m = get_vec(ctr, o);

  float a = v.x*v.x + v.y*v.y + v.z*v.z;
  float b = 2 * (v.x*m.x + v.y*m.y + v.z*m.z);
  float c = m.x*m.x + m.y*m.y + m.z*m.z - r*r;
  float delta = b*b - 4*a*c;

  if (delta < 0)
    return -1;

  float k = (-b - sqrt(delta))/(2*a);

  check.x = o.x + k * v.x;
  check.y = o.y + k * v.y;
  check.z = o.z + k * v.z;

  return k;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point eye, Vector ray, Spheres *scene, Point &hit) {
//
// do your thing here
//
  float para = INF;
  Point check;
  check.x = check.y = check.z = -1;
  Spheres *ptr = scene;
  Spheres *result = NULL;

  while (ptr != NULL){
    float tmp = intersect_sphere(eye, ray, ptr, check);

    if (tmp > 0 && tmp < para){
      para = tmp;
      result = ptr;
      hit = check;
    }
    ptr = ptr -> next;
  }

	return result;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient)[0] = amb[0];
  (new_sphere->mat_ambient)[1] = amb[1];
  (new_sphere->mat_ambient)[2] = amb[2];
  (new_sphere->mat_diffuse)[0] = dif[0];
  (new_sphere->mat_diffuse)[1] = dif[1];
  (new_sphere->mat_diffuse)[2] = dif[2];
  (new_sphere->mat_specular)[0] = spe[0];
  (new_sphere->mat_specular)[1] = spe[1];
  (new_sphere->mat_specular)[2] = spe[2];
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}
