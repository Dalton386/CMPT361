#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"
#include <algorithm>
#include <iostream>
#include <time.h>
#include <stdlib.h>
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

extern int step_max;
extern int shadow_on;
extern int reflec_on;
extern int refrac_on;
extern int chessb_on;
extern int stocha_on;
extern int supers_on;
extern float refractance;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Vector n, Vector l, Vector v, Vector r, Point q, Spheres *sph, bool inside) {
	RGB_float color;

  RGB_float amb;
  amb.r = sph->mat_ambient[0] * (global_ambient[0] + light1_ambient[0]);
  amb.g = sph->mat_ambient[1] * (global_ambient[1] + light1_ambient[1]);
  amb.b = sph->mat_ambient[2] * (global_ambient[2] + light1_ambient[2]);

  if (shadow_on == 1){
    Point hit;
    hit.x = hit.y = hit.z = -10;
    Vector ray = get_vec(q, light1);
    Spheres *hitsph = intersect_scene(q, ray, scene, &hit, false);
    float len = vec_len(get_vec(q, hit));
    if (hitsph != NULL && vec_len(ray) >= len){
      return amb;
    }

    if (inside)
      return amb;
  }

  float dist = vec_len(get_vec(q, light1));
  float fatt = min( float(1), 1 / (decay_a + decay_b*dist + decay_c*dist*dist));

  RGB_float dif;
  dif.r = sph->mat_diffuse[0] * light1_diffuse[0];
  dif.g = sph->mat_diffuse[1] * light1_diffuse[1];
  dif.b = sph->mat_diffuse[2] * light1_diffuse[2];
  dif = clr_scale(dif, fatt*vec_dot(n, l));

  RGB_float spc;
  spc.r = sph->mat_specular[0] * light1_specular[0];
  spc.g = sph->mat_specular[1] * light1_specular[1];
  spc.b = sph->mat_specular[2] * light1_specular[2];
  spc = clr_scale(spc, fatt*pow(vec_dot(r, v), sph->mat_shineness));

  if (vec_dot(n, l) <= 0){
    color = amb;
  }
  else if (vec_dot(r, v) <= 0){
    color = clr_add(amb, dif);
  }
  else
    color = clr_add(amb, clr_add(dif, spc));

	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point eye, Vector ray, int step, bool inside) {
  if (step > step_max)
    return background_clr;

  Point hit;
  hit.x = hit.y = hit.z = -1;
  Spheres *hitsph = intersect_scene(eye, ray, scene, &hit, inside);

  if (hitsph == NULL) 
    return background_clr;

  Vector n = {0, 1, 0};
  if (hitsph->index > 0){
    if (!inside){
      n = hitsph->norm;
      normalize(&n);
    }
    else{
      n = vec_scale(hitsph->norm, -1);
      normalize(&n);
    }
  }
  // else
  //   inside = false;

  Vector v = get_vec(hit, eye);
  normalize(&v);
  Vector l = get_vec(hit, light1);
  normalize(&l);
  Vector r = vec_minus(vec_scale(n, 2*vec_dot(n, l)), l);
  normalize(&r);
  Vector vr = vec_minus(vec_scale(n, 2*vec_dot(n, v)), v);
  normalize(&vr);
  RGB_float phclr = phong(n, l, v, r, hit, hitsph, inside);

  RGB_float rfclr, rrclr;
  rfclr.r = rfclr.g = rfclr.b = 0;
  rrclr.r = rrclr.g = rrclr.b = 0;
  if (reflec_on == 1){
    rfclr = clr_scale(recursive_ray_trace(hit, vr, step+1, inside), hitsph->reflectance);
  }

  // consider refraction
  if (refrac_on == 1 && hitsph->index > 0){
    float ratio = 1 / hitsph->transparency;
    Vector nv = n;
    Vector i = ray;
    normalize(&i);

    if (inside){
      ratio = 1 / ratio;
    }
    float cosi = -1 * vec_dot(i, nv);
    float sint2 = pow(ratio, 2) * (1 - pow(cosi, 2));
    float para = ratio*cosi - sqrt(1 - sint2);

    Vector t = vec_plus(vec_scale(i, ratio), vec_scale(nv, para));
    rrclr = clr_scale(recursive_ray_trace(hit, t, step+1, !(inside)), refractance);
  }

  RGB_float stclr;
  stclr.r = stclr.g = stclr.b = 0;
  if (stocha_on == 1) {
    int count = RAY_NUM;
    Vector nnv;
    while (count-- > 0) {
      nnv = {rand()%10-5, rand()%10-5, rand()%10-5};
      while (vec_dot(nnv, n) <= 0){
        nnv = {rand()%10-5, rand()%10-5, rand()%10-5};
      }

      normalize(&nnv);
      stclr = clr_add(stclr, clr_scale(recursive_ray_trace(hit, nnv, step+1, inside), hitsph->reflectance));
    }

    stclr = clr_scale (stclr, 1.0/RAY_NUM);
  }

  RGB_float color = clr_add(clr_add(phclr, stclr) , clr_add(rfclr, rrclr));

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
  bool inside = false;
  srand (time(NULL));

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
        ray = get_vec(eye_pos, cur_pixel_pos);
        ret_color = recursive_ray_trace(eye_pos, ray, 0, inside);

        if (supers_on == 1){
          RGB_float clr = {0, 0, 0};
          Point cur = cur_pixel_pos;
          float dx = 0.25 * x_grid_size;
          float dy = 0.25 * y_grid_size;
          Point use;

          use = {cur.x - dx, cur.y - dy, cur.z};
          ray = get_vec(eye_pos, use);
          clr = clr_add (clr, recursive_ray_trace(eye_pos, ray, 0, inside));

          use = {cur.x + dx, cur.y + dy, cur.z};
          ray = get_vec(eye_pos, use);
          clr = clr_add (clr, recursive_ray_trace(eye_pos, ray, 0, inside));

          use = {cur.x + dx, cur.y - dy, cur.z};
          ray = get_vec(eye_pos, use);
          clr = clr_add (clr, recursive_ray_trace(eye_pos, ray, 0, inside));     

          use = {cur.x - dx, cur.y + dy, cur.z};
          ray = get_vec(eye_pos, use);
          clr = clr_add (clr, recursive_ray_trace(eye_pos, ray, 0, inside));    

          ret_color = clr_add(clr, ret_color);
        }


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
