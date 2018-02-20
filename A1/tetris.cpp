// Two-Dimensional Sierpinski Gasket       
// Generated using randomly selected vertices and bisection

#include "Angel.h"
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

using namespace std;

//It is nice to define and use a color pallete on your software. You can put this information on another utility file.
static vec3 base_colors[] = {
        vec3(0.0, 0.0, 0.0),        //1~6 is available for tile
        vec3(1.0, 0.0, 0.0),        //0 is for bg
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(1.0, 1.0, 0.0),
        vec3(1.0, 0.0, 1.0),
        vec3(0.0, 1.0, 1.0),
};

static vec3 grid_color = vec3(0.2, 0.2, 0.2);

//three triangles
const int NumPoints = 1200;

//and two lines
const int NumPoints1 = 64;

//vertex array for the triangles and for the lines, respectively
GLuint vao, vao1;

//triangle points and colors arrays
vec2 points[NumPoints];
vec3 colors[NumPoints];

//lines points and colors arrays
vec2 points1[NumPoints1];
vec3 colors1[NumPoints1];

//the image
int row = 20, col = 10;
int canvas[20][10] = {0};
int image[20][10] = {0};
int xoff, yoff;
array<array<int, 4>, 4> falling;
vector<array<array < int, 4>, 4>> tilab;

//----------------------------------------------------------------------------
void draw_image(void);

void init_tilab();

bool isCollision(int x, int y);

void rotate(); // rotate the entire matrix
void reverse_rotate();

void init_falling();

void move_right();

void move_left();

void move_rotate();

void move_down();

//----------------------------------------------------------------------------
void
init(void) {
    //Just some hard coded data
    //Vertex positions 
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            double cx = -0.9 + 0.2 * j;
            double cy = 0.95 - 0.1 * i;
            int cidx = 6 * (i * col + j);

            points[cidx] = vec2(cx - 0.1, cy + 0.05);
            points[cidx + 1] = vec2(cx - 0.1, cy - 0.05);
            points[cidx + 2] = vec2(cx + 0.1, cy + 0.05);
            points[cidx + 3] = vec2(cx - 0.1, cy - 0.05);
            points[cidx + 4] = vec2(cx + 0.1, cy + 0.05);
            points[cidx + 5] = vec2(cx + 0.1, cy - 0.05);
        }
    }

    //color stuff for each vertex of each of the triangles
    for (int i = 0; i < NumPoints; i++)
        colors[i] = base_colors[0];

    //Draw the grids.
    int i = 0;
    for (; i < 21; i++) {
        points1[2 * i] = vec2(-1, (i - 10) * 0.1);
        points1[2 * i + 1] = vec2(1, (i - 10) * 0.1);
        colors1[2 * i] = colors1[2 * i + 1] = grid_color;
    }

    for (; i < 32; i++) {
        points1[2 * i] = vec2((i - 26) * 0.2, 1);
        points1[2 * i + 1] = vec2((i - 26) * 0.2, -1);
        colors1[2 * i] = colors1[2 * i + 1] = grid_color;
    }

    //Here we create another vertexArrayObject to render some lines. This is intended to be your grid, so since the
    //grid positions never change, you can leave this vertex array object on the initialization.
    // Create and bind a vertex array object
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);

    // Create and initialize a buffer object
    GLuint buffer1;
    glGenBuffers(1, &buffer1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer1);

    //glBufferData( GL_ARRAY_BUFFER, sizeof(points1), points1, GL_STATIC_DRAW );
    glBufferData(GL_ARRAY_BUFFER, sizeof(points1) + sizeof(colors1), points1, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points1), points1);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points1), sizeof(colors1), colors1);

    // Load shaders and use the resulting shader program
    GLuint program1 = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program1);

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc1 = glGetAttribLocation(program1, "vPosition");
    glEnableVertexAttribArray(loc1);
    glVertexAttribPointer(loc1, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(0));

    GLuint vColor1 = glGetAttribLocation(program1, "vColor");
    glEnableVertexAttribArray(vColor1);
    glVertexAttribPointer(vColor1, 3, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(points1)));

    //****************************

    glClearColor(0, 0, 0, 0); // black background
}

//----------------------------------------------------------------------------

void
display(void) {

    // Create and bind a vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and bind a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);


    //glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), points, GL_STATIC_DRAW);


    // Next, we load the real data in parts.  We need to specify the
    //   correct byte offset for placing the color data after the point
    //   data in the buffer.  Conveniently, the byte offset we need is
    //   the same as the size (in bytes) of the points array, which is
    //   returned from "sizeof(points)".
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);


    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Initialize the vertex position attribute from the vertex shader
    // Keep in mind that the second parameter is associated with the length of the array sent to opengl (1D,2D,3D or 4D).
    // The last parameter is the offset where the data is stored on the buffer.
    GLuint loc = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(0));


    // Likewise, initialize the vertex color attribute.  Once again, we
    //    need to specify the starting offset (in bytes) for the color
    //    data.  Just like loading the array, we use "sizeof(points)"
    //    to determine the correct value.
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(points)));


    glClear(GL_COLOR_BUFFER_BIT);     // clear the window


    //Draw triangles
    //Here we are binding back the first vertex array object. Now we can acess all the buffers associated to it and render accordingly
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, NumPoints);

    //Draw lines using the second vertex array object. On your tetris code, you probabily want to draw the lines first, then the triangles.
    //If you want to change the thickness of the lines, this is how:  glLineWidth(5.0);    
    glBindVertexArray(vao1);
    glDrawArrays(GL_LINES, 0, NumPoints1);

    //Causes all issued commands to be executed as quickly as they are accepted by the actual rendering engine
    glFlush();
}

//----------------------------------------------------------------------------
void draw_image(void) {
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 10; j++)
            image[i][j] = canvas[i][j];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        	if (i + yoff - 1 >= 0)
            	image[i + yoff - 1][j + xoff - 1] += falling[i][j];

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int cidx = 6 * (i * col + j);

            colors[cidx] = base_colors[image[i][j] % 7];
            colors[cidx + 1] = base_colors[image[i][j] % 7];
            colors[cidx + 2] = base_colors[image[i][j] % 7];
            colors[cidx + 3] = base_colors[image[i][j] % 7];
            colors[cidx + 4] = base_colors[image[i][j] % 7];
            colors[cidx + 5] = base_colors[image[i][j] % 7];
        }
    }

    glutPostRedisplay();
}

bool isCollision(int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (falling[i][j] > 0) {
                if (i + y > 20 || j + x < 1 || j + x > 10)
                    return true;
                else if (i + y - 1 >= 0 && canvas[i + y - 1][j + x - 1] > 0)
                    return true;
            }
        }
    }

    return false;
}

void init_tilab() {
    array<array<int, 4>, 4> tiles_O{};
    array<array<int, 4>, 4> tiles_I{};
    array<array<int, 4>, 4> tiles_S{};
    array<array<int, 4>, 4> tiles_Z{};
    array<array<int, 4>, 4> tiles_L{};
    array<array<int, 4>, 4> tiles_J{};
    array<array<int, 4>, 4> tiles_T{};

    // construct tiles_O
    tiles_O[1][1] = tiles_O[1][2] = tiles_O[2][1] = tiles_O[2][2] = 1;

    // construct tiles_I
    tiles_I[1][0] = tiles_I[1][1] = tiles_I[1][2] = tiles_I[1][3] = 1;

    // construct tiles_S
    tiles_S[1][2] = tiles_S[1][3] = tiles_S[2][1] = tiles_S[2][2] = 1;

    // construct tiles_Z
    tiles_Z[1][1] = tiles_Z[1][2] = tiles_Z[2][2] = tiles_Z[2][3] = 1;

    // construct tiles_L
    tiles_L[1][1] = tiles_L[1][2] = tiles_L[1][3] = tiles_L[2][1] = 1;

    // construct tiles_J
    tiles_J[1][1] = tiles_J[1][2] = tiles_J[1][3] = tiles_J[2][3] = 1;

    // construct tiles_T
    tiles_T[1][1] = tiles_T[1][2] = tiles_T[1][3] = tiles_T[2][2] = 1;

    tilab.push_back(tiles_O);
    tilab.push_back(tiles_I);
    tilab.push_back(tiles_S);
    tilab.push_back(tiles_Z);
    tilab.push_back(tiles_L);
    tilab.push_back(tiles_J);
    tilab.push_back(tiles_T);
}

void init_falling() {
    yoff = -2;
    xoff = rand() % 7 + 1;
    falling = tilab[xoff - 1];

    int color = rand() % 6 + 1;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (falling[i][j] == 1)
                falling[i][j] = color;

    int rot = rand() % 4 + 1;
    for (int i = 0; i < rot; i++)
        rotate();

    if (!isCollision(xoff, yoff))
        draw_image();
    else
        exit(EXIT_SUCCESS);
}

void rotate() {
    for (int i = 0; i < 4; i++)
        for (int j = i; j < 4; j++)
            swap(falling[i][j], falling[j][i]);

    reverse(falling.begin(), falling.end());
}

void reverse_rotate() {
    reverse(falling.begin(), falling.end());

    for (int i = 0; i < 4; i++)
        for (int j = i; j < 4; j++)
            swap(falling[i][j], falling[j][i]);
}

void move_right() {
    if (!isCollision(xoff + 1, yoff)) {
        xoff++;
        draw_image();
    }
}

void move_left() {
    if (!isCollision(xoff - 1, yoff)) {
        xoff--;
        draw_image();
    }
}

void move_rotate() {
    rotate();
    if (!isCollision(xoff, yoff))
        draw_image();
    else
        reverse_rotate();
}

void move_down() {
    if (!isCollision(xoff, yoff + 1)) {
        yoff++;
        draw_image();
    } else {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            	if (i + yoff - 1 >= 0)
                	canvas[i + yoff - 1][j + xoff - 1] += falling[i][j];
                else if (falling[i][j] > 0)
                	exit(EXIT_SUCCESS);

        for (int i = 19; i >= 0; i--) {
            int isFilled = 1;
            while (isFilled) {
                for (int j = 0; j < 10; j++)
                    if (canvas[i][j] == 0)
                        isFilled = 0;
                if (isFilled) {
                    for (int j = i; j > 0; j--)
                        for (int k = 0; k < 10; k++)
                            canvas[j][k] = canvas[j - 1][k];
                    for (int j = 0; j < 10; j++)
                        canvas[0][j] = 0;
                }
            }
        }

        init_falling();
    }
}

//timed function. We intended to execute this every one second.
void dropDelay(int) {
    move_down();

    //then we can set another identical event in 1000 miliseconds in the future, that is how we keep the triangle rotating
    glutTimerFunc(1000.0, dropDelay, 0);
}

//If you want to use arrows, take a look at this function
//It works similarly, but you have no acess to the ASCII keys using it. Please see the link below for details.
//https://www.opengl.org/resources/libraries/glut/spec3/node54.html
void keyboardSpecial(int key, int x, int y) {
    switch (key) {
        //similar to the stuff inside the keyboard class, but applying the translaton only to the third triangle.
        case GLUT_KEY_DOWN:
            printf("down arrow pressed!\n");
            move_down();
            break;
        case GLUT_KEY_UP:
            printf("down arrow pressed!\n");
            move_rotate();
            break;
        case GLUT_KEY_LEFT:
            printf("down arrow pressed!\n");
            move_left();
            break;
        case GLUT_KEY_RIGHT:
            printf("down arrow pressed!\n");
            move_right();
            break;
    }

}

//bind most keys on your keyboard to this function
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 113:
            printf("q pressed!\n");
            exit(EXIT_SUCCESS);
            break;

        case 114:
            printf("r pressed!\n");
            for (int i = 0; i < 20; i++)
                for (int j = 0; j < 10; j++)
                    image[i][j] = canvas[i][j] = 0;
            init_falling();
            break;
    }

}

//----------------------------------------------------------------------------

int
main(int argc, char **argv) {
    glutInit(&argc, argv);
    //We explicitly tell OpenGL that we are using the RGBA mode. When we start working with 3D resources there will be changes here.
    glutInitDisplayMode(GLUT_RGBA);

    //Change the size of the screen here. Keep in mind that the coordinates will also change accordingly
    //so, if you want to keep your points on the rendering canvas undisturbed you need a square screen.
    glutInitWindowSize(400, 800);

    // If you are using freeglut, the next two lines will check if 
    // the code is truly 3.2. Otherwise, comment them out
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    //Title of your screen, change accordingly
    glutCreateWindow("Simplified Tetris");

    // Iff you get a segmentation error at line 34, please uncomment the line below
    glewExperimental = GL_TRUE;
    glewInit();

    //this is the initialization function for your data.
    init();

    //Here we bind the function "display" to the OpenGL handler that will render your data
    glutDisplayFunc(display);

    //Finally, we bind the keyboard events inside the class "keyboard" using the line below
    glutKeyboardFunc(keyboard);

    //If you really want to use F1 ~ F9 keys, or the arrow keys, then use the line of code below.
    glutSpecialFunc(keyboardSpecial);

    //Start the game
    srand(static_cast<unsigned int>(time(nullptr)));
    init_tilab();
    init_falling();

    glutTimerFunc(1000.0, dropDelay, 0);

    glutMainLoop();
    return 0;
}
