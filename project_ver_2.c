#include <GL/freeglut.h>
#include <math.h>
#include "readBMP.h"
#define NIMAGES 2
#define nslices 100
#define nstacks 100

//comment ONLY ONE of the following:
//#define TORUS
#define SPHERE

//clipping area
GLdouble clip_area_x_left, clip_area_x_right, clip_area_y_bottom, clip_area_y_top;
//Object Values
GLfloat object_radius = 0.23f; //object radius
GLfloat object_x = 0.0f; //object center x coordinate
GLfloat object_y = 0.0f;  //object center y coordinate
GLfloat object_x_max, object_x_min, object_y_max, object_y_min; //these will serve as the edge
GLfloat x_speed = 0.05f;    //speed in x axis
GLfloat y_speed = 0.007f;   //speed in y axis
//Fog
GLfloat density = 0.3;
GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
//Refresh
GLfloat refresh_ms = 30; //refresh period in milliseconds
GLint colour = 3;

//Material light properties
GLfloat material_AmbAndDif[] = {0.0, 0.1, 0.9, 1.0};
GLfloat material_Spec[] = {0.5, 0.9, 0.3, 1.0};
GLfloat material_Shininess[] = {2.0};
//Global ambient light
GLfloat globAmb[] = {1.0, 0.0, 1.0, 1.0};
//Light property vectors.
GLfloat lightAmb[] = {0.9, 0.0, 0.0, 1.0};
GLfloat lightDif[] = {0.9, 0.0, 0.0, 1.0};
GLfloat lightSpec[] = {0.0, 1.0, 0.0, 1.0};
GLfloat red[] = {1.0,0.0,0.0,1.0};
GLfloat blue[] = {0.0,0.0,1.0,1.0};
GLfloat green[] = {0.0,1.0,0.0,1.0};


// vertex coordinates
GLfloat vertexCoords[4*3] = {
    -1.0, -1.0, 0.0,
    1.0, -1.0, 0.0,
    -1.0, 1.0, 0.0,
    1.0, 1.0, 0.0
};
//texture coordinates
GLfloat textCoords[4*2] =
{
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
};


// Local storage for bmp image data.
struct BitMapFile *image[NIMAGES];
// file containing the bmp image
char *fileName = "pinball_2.bmp";
//texture ID Objects
static GLenum textureID[1];


//initialization routine
void init(){

    // Load external textures.
    glClearColor(0.1,0.1,0.1, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST); //Enable Depth Test
    glDepthFunc(GL_LESS);

    /*
    //Fog doesn't seem to work..
    glEnable(GL_FOG); //Enable Fog
    glFogfv(GL_FOG_COLOR, fogColor); //set the fog colour to our colour chosen above
    glFogi(GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2
    glFogf(GL_FOG_DENSITY, density); //set the density to the value above
    glHint(GL_FOG_HINT, GL_NICEST); //set the fog to look the nicest, may slow down on older cards
    */

}


void LoadExternalTextures(){

    image[0] = readBMP("pinball_2.bmp");
    if(image[0] == NULL){
        printf("ERROR: Could not load image");
        exit(0);
    }

    // initialize texture ID
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image[0]->sizeX,image[0]->sizeY,0,GL_RGBA,GL_UNSIGNED_BYTE,image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Enable vertex arrays for coordinates and texture mapping
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertexCoords);

    // glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, textCoords);

}


//Callback Handler
void display(){


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // initialize model view transforms
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    LoadExternalTextures();
    glPushMatrix();
        //Activate texture 1 - pinball
        glBindTexture(GL_TEXTURE_2D, textureID[0]);
        glColor3f(1.0,1.0,1.0);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    glPopMatrix();

    //turn on OpenGL Lighting
    glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
    // Global ambient light.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
    // Enable two-sided lighting.
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    // Enable local viewpoint.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    //Light Position
    GLfloat LightPosition[] = {object_x,object_y,0.0};
    GLfloat Light_Center[] = {0.0, 0.0, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Center);
    //let there be light
    glEnable(GL_LIGHT0);
    //Material light properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,material_AmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_Shininess);

    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D,textureID[0]);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
        glBindTexture(GL_TEXTURE_2D,0);
    glPopMatrix();

    glTranslatef(object_x, object_y, 0.0f); //makes the ball move

    switch(colour){
        case 1:
            glColor3f(1.0, 0.0 ,0.0);
            break;
        case 2:
            glColor3f(0.0, 1.0, 0.0);
            break;
        case 3:
            glColor3f(0.0, 0.0, 1.0);
            break;
        case 4:
            glColor3f(0.5, 0.5, 0.5);
            break;
        default:
            break;
    }
    //place a sphere in the scene
    #ifdef TORUS
        glutSolidTorus(object_radius/3,object_radius,nslices,nstacks);
    #endif // TORUS

    #ifdef SPHERE
        glutSolidSphere(object_radius,nslices,nstacks);
    #endif
    glutSwapBuffers(); //swap front and back buffers

    //Animation
    object_x += x_speed;
    object_y += y_speed;

    //if the ball exceeds the edges, it will rebound and move in the opposite direction - the speed constantly gets added to the x and y coordinates of my object
    //the object will then essentially move thanks to the glTranslate, if we comment that line, it will stop moving.
    //if we multiply x_speed by a constant here, we get a funny exponential acceleration and the ball will rebound super fast
    if(object_x > object_x_max){
        object_x = object_x_max;
        x_speed = -x_speed;
    }
    else if (object_x < object_x_min){
        object_x = object_x_min;
        x_speed = -x_speed;

    }
     if(object_y > object_y_max){
        object_y = object_y_max;
        y_speed = -y_speed;
    }
    else if (object_y < object_y_min){
        object_y = object_y_min;
        y_speed = -y_speed;
    }

}

//Call back when the the user tries to resize the window
//constantly refreshes the window
void reshape(GLsizei width, GLsizei height){
    //Compute aspect ratio of the new window
    if(height == 0) height ==1; //to prevent division by 0
    GLfloat aspect_ratio = (GLfloat)width / (GLfloat)height;
    //aspect ratio is w/h
    //set the viewport to cover the new window
    glViewport(0, 0, width, height);

    //Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // to operate on the projection matrix
    glLoadIdentity(); //resets the matrix
    if(width>=height){
        clip_area_x_left = -1.0*aspect_ratio;
        clip_area_x_right = 1.0*aspect_ratio;
        clip_area_y_bottom = -1.0;
        clip_area_y_top = 1.0;
    }
    else {
        clip_area_x_left = -1.0;
        clip_area_x_right = 1.0;
        clip_area_y_bottom = -1.0/aspect_ratio;
        clip_area_y_top = 1.0/aspect_ratio;
    }
    //gluOrtho2D sets up a two-dimensional orthographic viewing region
    //Left, right, bottom, top
    gluOrtho2D(clip_area_x_left, clip_area_x_right, clip_area_y_bottom, clip_area_y_top);
    object_x_min = clip_area_x_left + object_radius; //if we remove the object_radius factor here, the object will bounce only when the center hits the wall
    object_x_max = clip_area_x_right - object_radius;
    object_y_min = clip_area_y_bottom + object_radius;
    object_y_max = clip_area_y_top - object_radius;

}

void timer(int v){

    glutPostRedisplay();
    glutTimerFunc(refresh_ms, timer, 0); // glutTimerFunc registers a timer callback to be triggered in a specified number of milliseconds.

}

void keyInput(unsigned char key, int x, int y)
{
    int index=0;
    switch(key) {
        case 27:
            // ESC
            exit(0);
            break;
        case '1':
            for(index=0; index < 4; index++)
                material_Spec[index]+=0.01;
            break;
        case '2':
            for(index=0; index < 4; index++)
                material_Spec[index]-=0.01;;
            break;
        case '3':
            material_Shininess[1]+=0.1f;
            break;
        case '4':
            material_Shininess[1]-=0.1f;
            break;
        case '5':
            for(index=0; index < 4; index++)
                material_AmbAndDif[index]+=0.01;
            break;
        case '6':
            for(index=0; index < 4; index++)
                material_AmbAndDif[index]-=0.01;
            break;
        default:
            // do nothing
            break;
    }

}

void top_menu(int id){
    if(id==1) exit(0);
}

void color_menu(int id){

    int ind = 0;
    switch(id){
        case 1:
            //change colour of ambient light to red
            for(ind = 0; ind < 4; ind++)
                lightAmb[ind] = red[ind];
            break;
        case 2:
            //change colour of ambient light to green
            for(ind = 0; ind < 4; ind++)
                lightAmb[ind] = green[ind];
            break;
        case 3:
             //change colour of ambient light to blue
            for(ind = 0; ind < 4; ind++)
                lightAmb[ind] = blue[ind];
            break;
        case 4:
            //change colour of diffuse light to red
            for(ind = 0; ind < 4; ind++)
                lightDif[ind] = red[ind];
            break;
        case 5:
            //change colour of diffuse light to green
            for(ind = 0; ind < 4; ind++)
                lightDif[ind] = green[ind];

            break;
        case 6:
            //change colour of diffuse light to blue
            for(ind = 0; ind < 4; ind++);{
                lightDif[ind] = blue[ind];
            }
        case 7:
            //change colour of specular light to red
            for(ind = 0; ind < 4; ind++)
                lightSpec[ind] = red[ind];
            break;
        case 8:
            //change colour of specular light to green
            for(ind = 0; ind < 4; ind++)
                lightSpec[ind] = green[ind];

            break;
        case 9:
            //change colour of specular light to blue
            for(ind = 0; ind < 4; ind++)
                lightSpec[ind] = blue[ind];
            break;
    };
}

void makeMenu(void){

    int sub_menu;
    sub_menu = glutCreateMenu(color_menu);
        glutAddMenuEntry("Ambient-Red",1);
        glutAddMenuEntry("Ambient-Green",2);
        glutAddMenuEntry("Ambient-Blue",3);
        glutAddMenuEntry("Diffuse-Red",4);
        glutAddMenuEntry("Diffuse-Green",5);
        glutAddMenuEntry("Diffuse-Blue",6);
        glutAddMenuEntry("Specular-Red",7);
        glutAddMenuEntry("Specular-Green",8);
        glutAddMenuEntry("Specular-Blue",9);

    glutCreateMenu(top_menu);
        glutAddSubMenu("Colour of light",sub_menu);
        glutAddMenuEntry("Quit",1);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

}

//Main function
int main(int argc, char **argv){

    int window_width = 640;
    int window_height = 640;
    int window_pos_x = 50;
    int window_pos_y = 50;

    glutInit(&argc, argv); //pass potential arguments to glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //Enable Double buffering
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(window_pos_x, window_pos_y);
    glutCreateWindow("OpenGL Window");
    printf("Press ESC to quit\n");
    printf("Press 1 and 2 to increase/decrease the material light specular component.\n");
    printf("Press 3 and 4 to increase/decrease the material shininess.\n");
    printf("Press 5 and 6 to increase/decrease the material light ambient/diffuse component.\n");
    printf("You can resize the window.\n");
    glutKeyboardFunc(keyInput);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0); //  glutTimerFunc registers a timer callback to be triggered in a specified number of milliseconds.
    makeMenu();
    init(); //initiation function
    glutDisplayFunc(display); //set display function
    glutMainLoop(); //initialize glut main loop

    return 0;
}
