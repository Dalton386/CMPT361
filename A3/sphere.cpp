#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#define INF 1e10
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
void change_color(Spheres *sph, int opt){
  if (opt == 0) {
    sph->mat_ambient[0] = sph->mat_ambient[1] = sph->mat_ambient[2] = 0;
    sph->mat_diffuse[0] = sph->mat_diffuse[1] = sph->mat_diffuse[2] = 0;
    sph->mat_specular[0] = sph->mat_specular[1] = sph->mat_specular[2] = 0;
  }
  else {
    sph->mat_ambient[0] = sph->mat_ambient[1] = sph->mat_ambient[2] = 1;
    sph->mat_diffuse[0] = sph->mat_diffuse[1] = sph->mat_diffuse[2] = 1;
    sph->mat_specular[0] = sph->mat_specular[1] = sph->mat_specular[2] = 1;
  }

}

float intersect_sphere(Point o, Vector v, Spheres *sph, Point *check, bool inside) {
  Point ctr = sph->center;
  float r = sph->radius;
  float k;

  if (sph->index > 0){
    Vector m = get_vec(ctr, o);

    float a = v.x*v.x + v.y*v.y + v.z*v.z;
    float b = 2 * (v.x*m.x + v.y*m.y + v.z*m.z);
    float c = m.x*m.x + m.y*m.y + m.z*m.z - r*r;
    float delta = b*b - 4*a*c;
    if (delta < 0)
      return -1;

    if (inside)
      k = (-b + sqrt(delta))/(2*a);
    else
      k = (-b - sqrt(delta))/(2*a);
  }
  else {
    float depth = ctr.y;
    k = (depth - o.y) / v.y;
    float nx = (o.x + k * v.x - ctr.x) / r;
    float nz = (o.z + k * v.z - ctr.z) / r;

    Vector norm = {0, 1, 0};
    if (vec_dot(norm, v) >= 0)
      return -1;

    if (nx <= 8 && nx >= 0 && nz <= 8 && nz >= 0){
      if ((int(nx)+int(nz)) % 2 == 0){
        change_color(sph, 0);
      }
      else
        change_color(sph, 1);
    }
    else
      return -1;
  }

  check->x = o.x + k * v.x;
  check->y = o.y + k * v.y;
  check->z = o.z + k * v.z;

  return k;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point eye, Vector ray, Spheres *scene, Point *hit, bool inside) {
//
// do your thing here
//
  float para = INF;
  Point check;
  check.x = check.y = check.z = -1;
  Spheres *ptr = scene;
  Spheres *result = NULL;

  while (ptr != NULL){
    float tmp = intersect_sphere(eye, ray, ptr, &check, inside);

    if (tmp >= 0 && tmp < para){
      para = tmp;
      result = ptr;
      hit->x = check.x;
      hit->y = check.y;
      hit->z = check.z;
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
		    float refl, float trans, int sindex) {
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
  new_sphere->transparency = trans;
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
