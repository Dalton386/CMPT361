//
// this provide functions to set up the scene
//
#include "sphere.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];

extern float global_ambient[3];
extern Spheres *scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;
extern float refractance;

extern double scale;
extern double trans_x;
extern double trans_y; 
extern double trans_z;

float sphere_ambient[] = {0.6, 0.6, 0.6};
float sphere_diffuse[] = {1.0, 0.0, 0.25};
float sphere_specular[] = {1.0, 1.0, 1.0};
float sphere_shineness = 6;
float sphere_reflectance = 0.3;
float sphere_transparency = 1.5;

//////////////////////////////////////////////////////////////////////////



void build_chess(double trans_x, double trans_y, double trans_z){
  // string fileName = 

  ifstream infile("chess_pieces/chess_piece.smf");
  char vf;
  int vnum, fnum;
  vector<Point> allpts;

  Point orig = {0, 0, 0};     // occupy 0
  allpts.push_back(orig);

  infile >> vf >> vnum >> fnum;
  for (int i = 0; i < vnum; i++){
    // cout << "here" << endl;
    double x, y, z;
    infile >> vf >> x >> y >> z;

    x = x + trans_x;
    y = y + trans_y;
    z = z + trans_z;

    Point pt = {x, y, z};
    allpts.push_back(pt);
  }

  for (int i = 0; i < fnum; i++){
    // cout << "there" << endl;
    int x, y, z;
    infile >> vf >> x >> y >> z;
    Point v1, v2, v3;
    v1 = allpts[x];
    v2 = allpts[y];
    v3 = allpts[z];

    Vector norm = vec_crs(get_vec(v1, v2), get_vec(v1, v3));
    normalize(&norm);

    scene = add_sphere(scene, v1, v2, v3, norm, v1, 1, sphere_ambient,
             sphere_diffuse, sphere_specular, sphere_shineness,
         sphere_reflectance, sphere_transparency, i+1);
  }

  // cout << "hello" << endl;
}

void set_up_default_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // setup global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // spheres are opaque in default scene
  refractance = 0;

  // setup light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_ambient[0] = light1_ambient[1] = light1_ambient[2] = 0.1;
  light1_diffuse[0] = light1_diffuse[1] = light1_diffuse[2] = 1.0;
  light1_specular[0] = light1_specular[1] = light1_specular[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  double trans_x = 0.2;
  double trans_y = -1;
  double trans_z = -1.6;

  build_chess(trans_x, trans_y, trans_z);

  build_chess(-0.7, -1.2, -1.6);
}

/***************************************
 * You can create your own scene here
 ***************************************/
void set_up_user_scene() {

  set_up_default_scene();

  // spheres are transparent in default scene
  refractance = 0.5;
}
