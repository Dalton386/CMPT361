
#include "Angel.h"
#include "math.h"
#include "stdlib.h"
#include <iostream>
#include <string>
using namespace std;

#define PI 3.14159265

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
const int NumPoints1 = 40;  // the sphere
const int NumPoints2 = 342;

point4 points[NumVertices];
color4 colors[NumVertices];

point4 points1[NumPoints1];
color4 colors1[NumPoints1];

point4 points2[NumPoints2];
color4 colors2[NumPoints2];

point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 0.5, 0.5, 1.0 ),  // whatever
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};


// Parameters controlling the size of the Robot's arm
const GLfloat BASE_HEIGHT      = 2.0;
const GLfloat BASE_WIDTH       = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 5.0;
const GLfloat LOWER_ARM_WIDTH  = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 5.0;
const GLfloat UPPER_ARM_WIDTH  = 0.5;

// Shader transformation matrices
mat4  model_view;
mat4  fix_view, view;
GLuint ModelView, Projection;

// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
int      Axis = Base;
GLfloat  Theta[NumAngles] = { 0.0 };
GLuint      vao, vao1, vao2;
bool     topv = false;              //side view
double   db, dl, du;
double   dgrB, dgrL, dgrU;
const int   grn = 1;                   //precision
const double   tgrn = 10.0;
int      stage;
bool     isStick = false;
double      ox, oy, oz, nx, ny, nz;
double r = UPPER_ARM_WIDTH / 2;

//----------------------------------------------------------------------------

int Index = 0;

void
quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------
/* Define the animation function*/
double calDgr(double a, double b, double c){
    double d = (a*a + b*b - c*c)/(2*a*b);

    return (acos(d) * 180.0 / PI);
}

void updateBLU(double x, double y, double z, double r){
    double a, b, c;
    db = dl = du = 0;

    a = sqrt(x*x + z*z);
    b = 1;
    c = sqrt((x-1)*(x-1) + z*z);
    dgrB = calDgr(a,b,c);
    if (z > 0) 
        dgrB = 360 - dgrB;

    a = LOWER_ARM_HEIGHT;
    b = UPPER_ARM_HEIGHT + r;
    c = sqrt(x*x + (y-BASE_HEIGHT)*(y-BASE_HEIGHT) + z*z);
    dgrU = 180 - calDgr(a,b,c);

    double tmp = calDgr(a,c,b);
    a = BASE_HEIGHT;
    b = c;
    c = sqrt(x*x + y*y + z*z);
    dgrL = 180 - tmp - calDgr(a,b,c);

    printf("B is %f, L is %f, U is %f\n", dgrB, dgrL, dgrU);
}

void rotate (int){
    bool forward = (db + grn <= dgrB) || (dl + grn <= dgrL) || (du + grn <= dgrU);
    bool backward = (db - grn >= 0) || (dl - grn >= 0) || (du - grn >= 0);
    if (stage%2 == 0 && forward){
        if (db + grn <= dgrB){
            db += grn;
            Theta[Base] = db;    
        }
        if (dl + grn <= dgrL){
            dl += grn;
            Theta[LowerArm] = 360 - dl;
        }
        if (du + grn <= dgrU){
            du += grn;
            Theta[UpperArm] = 360 - du;
        }
        glutPostRedisplay();
        glutTimerFunc(tgrn, rotate, 0);
    }
    else if (stage%2 == 1 && backward){
        if (db - grn >= 0){
            db -= grn;
            Theta[Base] = db;
        }
        if (dl - grn >= 0){
            dl -= grn;
            Theta[LowerArm] = 360 - dl;
        }
        if (du - grn >= 0){
            du -= grn;
            Theta[UpperArm] = 360 - du;
        }
        glutPostRedisplay();
        glutTimerFunc(tgrn, rotate, 0);
    }
    else if (++stage < 2){
        isStick = !isStick;
        printf("isStick is %d\n", isStick);
        glutTimerFunc(tgrn, rotate, 0);
    }
    else if (stage == 2){
        printf("continue rotation\n");
        updateBLU(nx, ny, nz, r);
        glutTimerFunc(tgrn, rotate, 0);
    }
    else if (stage == 3){
        isStick = !isStick;
        printf("isStick is %d\n", isStick);
        glutTimerFunc(tgrn, rotate, 0);        
    }
    else
        stage = 0;
}

void animate(){
    updateBLU(ox, oy, oz, r);
    glutTimerFunc(tgrn, rotate, 0);
}

//----------------------------------------------------------------------------

/* Define the three parts */
/* Note use of push/pop to return modelview matrix
to its state before functions were entered and use
rotation, translation, and scaling to create instances
of symbols (cube and cylinder */

void
base()
{
    mat4 instance = ( Translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ) *
		 Scale( BASE_WIDTH,
			BASE_HEIGHT,
			BASE_WIDTH ) );

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, view * model_view * instance );

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
upper_arm()
{
    mat4 instance = ( Translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) *
		      Scale( UPPER_ARM_WIDTH,
			     UPPER_ARM_HEIGHT,
			     UPPER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, view * model_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
lower_arm()
{
    mat4 instance = ( Translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) *
		      Scale( LOWER_ARM_WIDTH,
			     LOWER_ARM_HEIGHT,
			     LOWER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, view * model_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void sphere(){
    mat4 instance = Scale(r, r, r);

    if (isStick)
        fix_view = model_view;

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, view * fix_view * instance );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, NumPoints2 );
}

void sphere_strip(){
    mat4 instance = Scale(r, r, r);

    if (isStick)
        fix_view = model_view;

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, view * fix_view * instance );
    glDrawArrays( GL_TRIANGLE_FAN, 0, NumPoints1 );
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    model_view = mat4( 1.0 );       // An Identity matrix
    view = mat4(1.0);
    if (topv)
        view *= (Translate(0, 5, 0) * RotateX(90));

    //---------------------------------------
    // draw the robot
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBindVertexArray(vao);

    // Accumulate ModelView Matrix as we traverse the tree
    model_view *= RotateY(Theta[Base]);
    base();

    model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) *
		    RotateZ(Theta[LowerArm]) );
    lower_arm();

    model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
		    RotateZ(Theta[UpperArm]) );
    upper_arm();

    // draw the sphere
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    model_view *= Translate(0.0, UPPER_ARM_HEIGHT + r, 0.0);
    glBindVertexArray(vao1);
    sphere_strip();
    glBindVertexArray(vao2);
    sphere();

    glutSwapBuffers();
}

//----------------------------------------------------------------------------
void initSphere(){
    const float DegreeToRadians = PI / 180.0;

    for (int i = 0; i < NumPoints1; i++)
        colors1[i] = vertex_colors[7];

    for (int i = 0; i < NumPoints2; i++)
        colors2[i] = vertex_colors[7];

    int k = 0;
    points1[k] = point4(0, 0, 1, 1);
    k++;

    float sin80 = sin(80*DegreeToRadians);
    float cos80 = cos(80*DegreeToRadians);

    for (float theta = -180; theta <= 180; theta += 20){
        float thetar = theta*DegreeToRadians;
        points1[k] = point4(sin(thetar)*cos80, cos(thetar)*cos80, sin80, 1);
        k++;
    }

    points1[k] = point4(0, 0, -1, 1);
    k++;

    for (float theta = -180; theta <= 180; theta += 20){
        float thetar = theta;
        points1[k] = point4(sin(thetar)*cos80, cos(thetar)*cos80, sin80, 1);
        k++;
    }

    //*********************************************
    k = 0;
    for (float phi = -80; phi <= 80; phi += 20){
        float phir = phi*DegreeToRadians;
        float phir20 = (phi+20)*DegreeToRadians;

        for (float theta = -180; theta <= 180; theta += 20){
            float thetar = theta*DegreeToRadians;
            points2[k] = point4(sin(thetar)*cos(phir), cos(thetar)*cos(phir), sin(phir), 1);
            k++;
            points2[k] = point4(sin(thetar)*cos(phir20), cos(thetar)*cos(phir20), sin(phir20), 1);
            k++;
        }
    }
}


void
init( void )
{
    colorcube();
    
    // Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader81.glsl", "fshader81.glsl" );
    glUseProgram( program );
    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

    //---------------------------------------------------
    initSphere();

    // Create buffer for sphere strip
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);

    // Create and initialize a buffer object
    GLuint buffer1;
    glGenBuffers(1, &buffer1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points1) + sizeof(colors1), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points1), points1);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points1), sizeof(colors1), colors1);

    // Load shaders and use the resulting shader program
    GLuint program1 = InitShader("vshader81.glsl", "fshader81.glsl");
    glUseProgram(program1);

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition1 = glGetAttribLocation(program1, "vPosition");
    glEnableVertexAttribArray(vPosition1);
    glVertexAttribPointer(vPosition1, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(0));

    GLuint vColor1 = glGetAttribLocation(program1, "vColor");
    glEnableVertexAttribArray(vColor1);
    glVertexAttribPointer(vColor1, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(points1)));
    //**********************************************
    // draw main sphere
    // Create buffer for sphere
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);

    // Create and initialize a buffer object
    GLuint buffer2;
    glGenBuffers(1, &buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, buffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points2) + sizeof(colors2), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points2), points2);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points2), sizeof(colors2), colors2);

    // Load shaders and use the resulting shader program
    GLuint program2 = InitShader("vshader81.glsl", "fshader81.glsl");
    glUseProgram(program2);

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition2 = glGetAttribLocation(program2, "vPosition");
    glEnableVertexAttribArray(vPosition2);
    glVertexAttribPointer(vPosition2, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(0));

    GLuint vColor2 = glGetAttribLocation(program2, "vColor");
    glEnableVertexAttribArray(vColor2);
    glVertexAttribPointer(vColor2, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(points2)));

    //**********************************************
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat  left = -10.0, right = 10.0;
    GLfloat  bottom = -5.0, top = 15.0;
    GLfloat  zNear = -20.0, zFar = 20.0;

    GLfloat aspect = GLfloat(width)/height;

    if ( aspect > 1.0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }

    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

    model_view = mat4( 1.0 );  // An Identity matrix
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
	case 'a':
		Theta[Axis] += 5.0;
		if ( Theta[Axis] > 360.0 ) { Theta[Axis] -= 360.0; }
		glutPostRedisplay();
		break;
	case 'd':
		Theta[Axis] -= 5.0;
		if ( Theta[Axis] < 0.0 ) { Theta[Axis] += 360.0; }
		glutPostRedisplay();
		break;
	case '1':
		Axis = 0; 
		printf("set to base\n");		// set to base
		break;
	case '2':
		Axis = 1; 
		printf("set to lower arm\n");	// set to lower arm
		break;
	case '3':
		Axis = 2; 
		printf("set to upper arm\n");	// set to upper arm
		break;
    case 't':
        topv = true;
        printf("change to top view\n");
        glutPostRedisplay();
        break;
    case 's':
        topv = false;
        printf("change to side view\n");
        glutPostRedisplay();
        break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    //./myrobot 0 0 5 5 2 5 -tv
    string tsv;
    ox = atof(argv[1]); oy = atof(argv[2]); oz = atof(argv[3]);
    nx = atof(argv[4]); ny = atof(argv[5]); nz = atof(argv[6]);
    tsv = string(argv[7]);

    if (tsv == "-tv")
        topv = true;
    else if (tsv == "-sv")
        topv = false;

    cout << ox << oy << oz << nx << ny << nz << tsv << endl;

    fix_view = Translate(ox, oy, oz);

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "robot" );

    // Iff you get a segmentation error at line 34, please uncomment the line below
    glewExperimental = GL_TRUE; 
    glewInit();
    
    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );

    printf("start rotation\n");
    animate();

    glutMainLoop();
    return 0;
}
