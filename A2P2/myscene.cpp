#ifdef __APPLE_CC__
#include "Angel.h"
#include <GLUT/glut.h>
#else
#include "Angel.h"
#include <GL/glut.h>
#endif


GLfloat blue[] = { 0, 0, 0.9, 1.0 };
GLfloat light_blue[] = { 0.1, 0.1, 0.9, 1.0 };

GLfloat green[] = { 0, 0.9, 0, 1.0 };
GLfloat light_green[] = { 0.3, 0.8, 0.3, 1.0 };

GLfloat yellow[] = { 0.9, 0.9, 0.25, 1.0 };
GLfloat light_yellow[] = { 0.6, 0.5, 0.08, 1 };

GLfloat red[] = { 0.83, 0.26, 0.15, 1.0 };
GLfloat light_red[] = {  0.78, 0.45, 0.14, 1.0  };


GLfloat ding[] = { 0.45, 0.25, 0.9, 1};
GLfloat purple[] = { 0.9, 0.2, 0.9, 1.0 };
GLfloat cyan[] = { 0.2, 0.8, 0.8, 1.0 };
GLfloat virus[] = {0.074, 0.517, 0.318, 1};
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat grey[] = { 0.5, 0.5, 0.5, 1.0 };

GLfloat deep_grey[] = { 0.4, 0.4, 0.4, 1.0 };

GLfloat direction[] = { 1.0, 1.0, 0.0, 0 };

double gnu;
int len;
double density;
double scale;
double shnn;
int opt;
GLfloat aspect;
GLfloat* colors[2];

void set_material(GLfloat ambient[], GLfloat specular[], GLint shininess){
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambient);
  	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void set_light(GLfloat ambient[], GLfloat diffuse[], GLfloat specular[], GLfloat direct[] ){
  	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  	glLightfv(GL_LIGHT0, GL_POSITION, direct);
}

void set_par(double g, double l, double d, double s, double sh, double o){
    gnu = g;
    len = l;
    density = d;
    scale = s;
    shnn = sh;
    opt = o;
}

void draw_primitive(int op){
	GLdouble size = 0.1;
	switch (op){
		case 0:
			glutSolidSphere(gnu, 6, 6);break;
		case 1:
			glutSolidTeapot(size);break;
		case 2:
			glutSolidCube(size);break;
	}
}

void draw_virus(){
	double den = 360 / density;

	for (int l = 0; l < density; l++){
		glPushMatrix();
		glRotatef(l*den, 1, 0, 0);	
		for (int k = 0; k < density; k++){
			glPushMatrix();
			glRotatef(k*den, 0, 1, 0);
			for (int j = 0; j < len; j++){
				glPushMatrix();
				glTranslatef(0, 0, gnu*j);
				glRotatef((j*60)%360, 0, 0, 1);
				for (int i = 0; i < 6; i++){
					set_material(colors[i%2], white, shnn*i);
					glPushMatrix();
					glRotatef(i*60, 0, 0, 1);
				  	glTranslatef(gnu, 0, 0);
				  	draw_primitive(opt);
				  	glPopMatrix();
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
}

void draw_layer(){
    glPushMatrix();
    colors[0] = yellow;
    colors[1] = light_yellow;
    glRotatef(10, 0, 1, 0);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();

    glPushMatrix();
    colors[0] = red;
    colors[1] = light_red;
    glRotatef(20, 1, 0, 0);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();

    glPushMatrix();
    colors[0] = green;
    colors[1] = light_green;
    glRotatef(15, 0, 0, 1);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();
}

void draw_base(){
    colors[0] = virus;
    colors[1] = deep_grey;
	glPushMatrix();
    glRotatef(10, 0, 0, 1);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();

    glPushMatrix();
    glRotatef(10, 0, 1, 0);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();

    glPushMatrix();
    glRotatef(20, 1, 0, 0);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();

    glPushMatrix();
    glRotatef(15, 1, 1, 0);
    glScalef(scale, scale, scale);
    draw_virus();
    glPopMatrix();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();

  glPushMatrix();
  glRotatef(20, 0, 0, 1);
  set_par(0.1, 30, 6, 0.6, 2, 2);
  draw_layer();
  glPopMatrix();

  glPushMatrix();
  glRotatef(20, 0, 1, 0);
  set_par(0.05, 50, 6, 0.6, 0, 1);
  draw_layer();
  glPopMatrix();

  glPushMatrix();
  glRotatef(20, 1, 0, 0);
  set_par(0.1, 20, 15, 0.6, 0, 0);
  draw_base();
  glPopMatrix();

  //-----------------------------------------------------
  // the end
  glPopMatrix();
  glFlush();
}

void keyboard( unsigned char key, int x, int y ) {
    switch( key ) {
	     case 033: // Escape Key
	     case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}

void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);

  aspect = GLfloat(w) / GLfloat(h);
  glLoadIdentity();
  if (w <= h) {
    // width is smaller, so stretch out the height
    glOrtho(-2.5, 2.5, -2.5/aspect, 2.5/aspect, -10.0, 10.0);
  } else {
    // height is smaller, so stretch out the width
    glOrtho(-2.5*aspect, 2.5*aspect, -2.5, 2.5, -10.0, 10.0);
  }
}

void init() {
  set_light(deep_grey, light_yellow, blue, direction);

  glEnable(GL_LIGHTING);                // so the renderer considers light
  glEnable(GL_LIGHT0);                  // turn LIGHT0 on
  glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(80, 80);
  glutInitWindowSize(800, 600);
  glutCreateWindow("myscene");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();
}