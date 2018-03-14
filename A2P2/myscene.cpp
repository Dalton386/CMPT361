#ifdef __APPLE_CC__
#include "Angel.h"
#include <GLUT/glut.h>
#else
#include "Angel.h"
#include <GL/glut.h>
#endif


GLfloat blue[] = { 0.25, 0.5, 0.9, 1.0 };
GLfloat ding[] = { 0.45, 0.25, 0.9, 1};
GLfloat green[] = { 0.3, 0.15, 0.8, 1.0 };
GLfloat purple[] = { 0.9, 0.2, 0.9, 1.0 };
GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
GLfloat cyan[] = { 0.2, 0.8, 0.8, 1.0 };

GLfloat light_red[] = { 0.5, 0.0, 0.0, 1.0 };
GLfloat virus[] = {0.074, 0.517, 0.318, 1};
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat direction[] = { 1.0, 1.0, 1.0, 0 };

double gnu = 0.02;
int len = 30;
double density = 8;
GLfloat aspect;
int displayListId;

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

void draw_virus(){
	GLfloat* colors[6] = {cyan, yellow, ding, green, blue, purple}; 
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
					set_material(colors[i], white, 30);
					glPushMatrix();
					glRotatef(i*60, 0, 0, 1);
				  	glTranslatef(gnu, 0, 0);
				  	glutSolidSphere(gnu, 6, 6);
				  	glPopMatrix();
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}

	set_material(virus, purple, 10);
	glutSolidSphere(gnu*len/2, 20, 20);

}


void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  // mat4 mt = Perspective(170.0, aspect, 1.0, 150.0);
  // GLdouble m[16] = {mt[0][0], mt[0][1], mt[0][2], mt[0][3],
  // 					mt[1][0], mt[1][1], mt[1][2], mt[1][3],
  // 					mt[2][0], mt[2][1], mt[2][2], mt[2][3],
  // 					mt[3][0], mt[3][1], mt[3][2], mt[3][3]};
  // glLoadMatrixd(m);

  glPushMatrix();
  glRotatef(-20.0, 1.0, 0.0, 0.0);

  // glPushMatrix();
  // glTranslatef(-2, 2, -2);
  // glRotatef(30, 0, 1, 0);
  // glScalef(0.3, 0.3, 0.3);
  // draw_virus();
  // glPopMatrix();

  glPushMatrix();
  glTranslatef(-1.7, -0.3, -1);
  glRotatef(50, 0, 1, 0);
  glScalef(0.8, 0.8, 0.8);
  draw_virus();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-0.5, -1.5, -0.5);
  glRotatef(10, 0, 1, 0);
  glScalef(1, 1, 1);
  draw_virus();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(1, -1, 1);
  glRotatef(70, 0, 1, 0);
  glScalef(1.5, 1.5, 1.5);
  draw_virus();
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
  set_light(cyan, green, yellow, direction);

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