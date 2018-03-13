
#include "Angel.h"
#include "math.h"

#define PI 3.14159265

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

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
GLuint ModelView, Projection;

// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
int      Axis = Base;
GLfloat  Theta[NumAngles] = { 0.0 };
bool     topv = false;              //side view
double   db, dl, du;
double   dgrB, dgrL, dgrU;
const int   grn = 1;                   //precision
const double   tgrn = 10.0;
int      stage;
bool     isStick = false;
double      ox, oy, oz, nx, ny, nz, r;

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
    b = sqrt(x*x + (y-BASE_HEIGHT)*(y-BASE_HEIGHT) + z*z);
    c = UPPER_ARM_HEIGHT + r;
    dgrL = calDgr(a,c,b);

    dgrU = 180 - calDgr(a,c,b);
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

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );

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
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
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
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    model_view = mat4( 1.0 );       // An Identity matrix

    if (topv){              // top view
        model_view *= (Translate(0, 5, 0) * RotateX(90));
    }

    // Accumulate ModelView Matrix as we traverse the tree
    model_view *= RotateY(Theta[Base]);
    base();

    model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) *
		    RotateZ(Theta[LowerArm]) );
    lower_arm();

    model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
		    RotateZ(Theta[UpperArm]) );
    upper_arm();

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
init( void )
{
    colorcube();
    
    // Create a vertex array object
    GLuint vao;
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

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat  left = -10.0, right = 10.0;
    GLfloat  bottom = -5.0, top = 15.0;
    GLfloat  zNear = -10.0, zFar = 10.0;

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
    case 'r':
        printf("start rotation\n");
        animate();
        break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    ox = 0; oy = -2; oz = -5;
    nx = 0; ny = 2; nz = 5;
    r = 1;

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

    glutMainLoop();
    return 0;
}
