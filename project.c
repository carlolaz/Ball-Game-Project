#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define NIMAGES 2
#define nslices 100
#define nstacks 100
#include "readBMP.h"

//Ball Values
GLfloat ballRadius = 0.3f; //radius of the bouncing ball
GLfloat ballX = 0.0f; //Ball's Center
GLfloat ballY = 0.0f;
GLfloat ballXMax, ballXMin, ballYMax, ballYMin;
GLfloat xSpeed = 0.02f;
GLfloat ySpeed = 0.007f;
//Fog
GLfloat density = 0.3;
GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};

float refreshMilliseconds = 30; //refresh period in milliseconds
int colour = 1;
int mouseX, mouseY = 0;

//Material light properties
float material_AmbAndDif[] = {1.0, 1.0, 0.0, 1.0};
float material_Spec[] = {0.0, 1.0, 0.0, 1.0};
float material_Shininess[] = {2.0};
// global ambient light
float globAmb[] = {0.0, 0.0, 1.0, 1.0};

// Light property vectors.
float lightAmb[] = {0.9, 0.0, 0.0, 1.0};
float lightDif[] = {0.9, 0.0, 0.0, 1.0};
float lightSpec[] = {0.0, 1.0, 0.0, 1.0};

GLfloat red[]={1.0f,0.0f,0.0f,1.0f};
GLfloat blue[]={0.0f,0.0f,0.9f,1.0f};
GLfloat green[]={0.0f,0.9f,0.0f,1.0f};

//texture coordinates
GLfloat textCoords[4*2] =
{
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
};

//Projection clipping areas
GLdouble clipAreaXLeft, clipAreaXRight, clipAreaYBottom, clipAreaYTop;
// Local storage for bmp image data.
struct BitMapFile *image[NIMAGES];
//texture ID Objects
static GLenum textureID[1];
//Routine di inizializzazione
void init(){

    glClearColor(0.1,0.1,0.1, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // initialize texture ID
    glGenTextures(2, textureID);
    // Load external textures.
    LoadExternalTextures();
    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // Turn on OpenGL texturing.
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST); //Enable Depth Test
    glDepthFunc(GL_LESS);

    //Ho provato ad implementare la nebbia ma non sembra avere effetto
    glTranslatef(0.0,0.0,2.0);
    glEnable(GL_FOG); //Enable Fog
    glFogfv(GL_FOG_COLOR, fogColor); //set the fog colour to our colour chosen above
    glFogi(GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2
    glFogf(GL_FOG_DENSITY, density); //set the density to the value above
    glHint(GL_FOG_HINT, GL_NICEST);

    // set the fog to look the nicest, may slow down on older cards
    //turn on OpenGL Lighting
    glEnable(GL_LIGHTING);
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

}

void LoadExternalTextures(){

    image[0] = readBMP("pinball.bmp");
    if(image[0] == NULL){
        printf("ERROR: Could not load image");
        exit(0);
    }

    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image[0]->sizeX,image[0]->sizeY,0,GL_RGBA,GL_UNSIGNED_BYTE,image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

}
//Callback Handler
void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); //per operare sulla matrice modelview
    glLoadIdentity();

    //Activate texture 1 - pinball
    glBindTexture(GL_TEXTURE_2D, textureID[0]);

    // position light
    GLfloat LightPosition[] = {ballX,ballY,0.0};
    GLfloat Light_Center[] = {0.0, 0.0, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Center);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,material_AmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_Shininess);

    glPushMatrix();
        /*
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,material_AmbAndDif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Spec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_Shininess);
        */
        //set material light properties
    glPopMatrix();
    glEnable(GL_LIGHT0); // let there be light

    glTranslatef(ballX, ballY, 0.0f); //fa muovere la palla
    /*
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        writeStrokeString(GLUT_STROKE_ROMAN, "Ball!");
    glPopMatrix();
    */

    //glBegin(GL_TRIANGLE_FAN);
    //scelgo il colore
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
    glutSolidSphere(ballRadius,nslices,nstacks);
    /*
    glVertex2f(0.0, 0.0); //center of the circle
    int numSegments = 100;
    GLfloat angle;
    int i = 0;
    for(i=0; i<= numSegments; i++){
        angle = i* 2.0f * PI/numSegments;
        glVertex2f(cos(angle)*ballRadius, sin(angle)*ballRadius);
    }
    glEnd();
    */
    glutSwapBuffers(); //swap front and back buffers

    //Animation Control - compute the location for the next refresh
    ballX += xSpeed;
    ballY += ySpeed;

    //Check if the ball exceeds the edges
    if(ballX > ballXMax){
        ballX = ballXMax;
        xSpeed = -xSpeed;
    }
    else if (ballX < ballXMin){
        ballX = ballXMin;
        xSpeed = -xSpeed;

    }
     if(ballY > ballYMax){
        ballY = ballYMax;
        ySpeed = -ySpeed;
    }
    else if (ballY < ballYMin){
        ballY = ballYMin;
        ySpeed = -ySpeed;
    }

}

//Call back when the window is re-sized
void reshape(GLsizei width, GLsizei height){
    //Compute aspect ratio of the new window
    if(height == 0) height ==1; //to prevent division by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    //set the viewport to cover the new indows
    glViewport(0, 0, width, height);

    //Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // to operate on the projection matrix
    glLoadIdentity();
    if(width>=height){
        clipAreaXLeft = -1.0*aspect;
        clipAreaXRight = 1.0*aspect;
        clipAreaYBottom = -1.0;
        clipAreaYTop = 1.0;
    }
    else {
        clipAreaXLeft = -1.0;
        clipAreaXRight = 1.0;
        clipAreaYBottom = -1.0/aspect;
        clipAreaYTop = 1.0/aspect;
    }
    //gluOrtho2D sets up a two-dimensional orthographic viewing region
    //Left, right, bottom, top
    gluOrtho2D(clipAreaXLeft, clipAreaXRight, clipAreaYBottom, clipAreaYTop);
    ballXMin = clipAreaXLeft + ballRadius;
    ballXMax = clipAreaXRight - ballRadius;
    ballYMin = clipAreaYBottom + ballRadius;
    ballYMax = clipAreaYTop - ballRadius;

}

void Timer(int value){

    glutPostRedisplay(); //Post a paint request to activate display();
    glutTimerFunc(refreshMilliseconds, Timer, 0);

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
    //se scelgo quit, esco.
}

void color_menu(int id){

    switch(id){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
    };
}

void makeMenu(void){

    int sub_menu;
    sub_menu = glutCreateMenu(color_menu);
        glutAddMenuEntry("Red",1);
        glutAddMenuEntry("Green",2);
        glutAddMenuEntry("Blue",3);

    glutCreateMenu(top_menu);
        glutAddSubMenu("Colour of light",sub_menu);
        glutAddMenuEntry("Quit",1);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

}

//Main function
int main(int argc, char **argv){

    int windowWidth = 640;
    int windowHeight = 640;
    int windowPosX = 50;
    int windowPosY = 50;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //Enable Double buffering
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosX, windowPosY);
    glutCreateWindow("Bouncing Ball");
    printf("Press ESC to quit\n");
    printf("Press 1 and 2 to increase/decrease the material light specular component.\n");
    printf("Press 3 and 4 to increase/decrease the material shininess.\n");
    printf("Press 5 and 6 to increase/decrease the material light ambient/diffuse component.\n");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyInput);
    makeMenu();
    glutReshapeFunc(reshape);
    glutTimerFunc(0, Timer, 0);
    init();
    glutMainLoop();

    return 0;
}

