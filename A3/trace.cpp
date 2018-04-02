#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"
#include <algorithm>
using namespace std;

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Spheres *sph) {
//
// do your thing here
//
	RGB_float color;

  Vector n = sphere_normal(q, sph);
  Vector v = get_vec(q, eye_pos);
  normalize(&v);
  Vector l = get_vec(q, light1);
  normalize(&l);
  Vector r = vec_minus(vec_scale(n, 2*vec_dot(n, l)), l);
  normalize(&r);

  RGB_float amb;
  amb.r = sph->mat_ambient[0] * (global_ambient[0] + light1_ambient[0]);
  amb.g = sph->mat_ambient[1] * (global_ambient[1] + light1_ambient[1]);
  amb.b = sph->mat_ambient[2] * (global_ambient[2] + light1_ambient[2]);
  // clr_norm(&amb);

  float dist = vec_len(get_vec(q, light1));
  float fatt = min(float(1.0), 1 / (decay_a + decay_b*dist + decay_c*dist*dist));

  RGB_float dif;
  dif.r = sph->mat_diffuse[0] * light1_diffuse[0];
  dif.g = sph->mat_diffuse[1] * light1_diffuse[1];
  dif.b = sph->mat_diffuse[2] * light1_diffuse[2];
  dif = clr_scale(dif, fatt*vec_dot(n, l));
  // clr_norm(&dif);

  RGB_float spc;
  spc.r = sph->mat_specular[0] * light1_specular[0];
  spc.g = sph->mat_specular[1] * light1_specular[1];
  spc.b = sph->mat_specular[2] * light1_specular[2];
  spc = clr_scale(spc, fatt*pow(vec_dot(r, v), sph->mat_shineness));
  // clr_norm(&spc);

  if (vec_dot(n, l) <= 0){
    color = amb;
  }
  else if (vec_dot(r,v) <= 0){
    color = clr_add(amb, dif);
  }
  else
    color = clr_add(amb, clr_add(dif, spc));

  // clr_norm(&color);

	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Vector ray) {
//
// do your thing here
//
	RGB_float color;
  Point hit;
  hit.x = hit.y = hit.z = -1;
  Spheres *hitsph = intersect_scene(eye_pos, ray, scene, hit);

  if (hitsph == NULL) 
    return background_clr;

  color = phong(hit, hitsph);

	return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(ray);
      // ret_color = background_clr; // just background for now

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

      // Checkboard for testing
      // RGB_float clr = {float(i/32), 0, float(j/32)};
      // ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
