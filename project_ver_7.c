#include <stdio.h>
#include <glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include "readBMP.h"
#define NFACES 6
#define NIMAGES 2
#define nslices 100
#define nstacks 100
#define object_radius 0.13f

//Uncomment only ONE of the following:
#define FILL
//#define LINE

// Object Values
GLfloat object_x = 0.0f; //object center x coordinate
GLfloat object_y = 0.0f;  //object center y coordinate
GLfloat object_z = 0.0f; //object z coordinates

// These will serve as the edges
GLfloat object_x_max = 0.5 - object_radius;
GLfloat object_x_min = -0.5 + object_radius;
GLfloat object_y_max = 0.5 - object_radius;
GLfloat object_y_min = -0.5 + object_radius;
GLfloat object_z_max = 0.5 - object_radius;
GLfloat object_z_min = -0.5 + object_radius;

GLfloat x_speed =  0.07f; //speed in x axis
GLfloat y_speed = 0.05f; //speed in y axis
GLfloat z_speed = 0.03f; //speed in z axis
// Refresh
GLfloat refresh_ms = 30; //refresh period in milliseconds
GLint colour = 3;
GLint angle = 45.0; //starting angle of view

//Material light properties
GLfloat material_AmbAndDif[] = {0.0, 0.1, 0.9, 1.0};
GLfloat material_Spec[] = {0.5, 0.9, 0.3, 1.0};
GLfloat material_Shininess[] = {1.0};

//Cube Material light properties
GLfloat cube_material_AmbAndDif[] = {0.3, 0.4, 0.5, 1.0};
GLfloat cube_material_Spec[] = {0.1, 0.7, 0.4, 1.0};
GLfloat cube_material_Shininess[] = {5.0};
//Global ambient light
GLfloat globAmb[] = {1.0, 1.0, 0.5, 1.0};
//Light property vectors.
GLfloat lightAmb[] = {0.7, 0.3, 0.4, 1.0};
GLfloat lightDif[] = {0.9, 0.5, 0.3, 1.0};
GLfloat lightSpec[] = {0.5, 1.0, 0.5, 1.0};
GLfloat red[] = {1.0,0.0,0.0,1.0};
GLfloat blue[] = {0.0,0.0,1.0,1.0};
GLfloat green[] = {0.0,1.0,0.0,1.0};


// Local storage for bmp image data.
struct BitMapFile *images[NFACES];
//Array containing the bpm images
char *fileName[NFACES] = {"right.bmp", "left.bmp", "top.bmp", "bottom.bmp", "front.bmp", "back.bmp"};
//Parameters to bind the textures
GLenum facePos[NFACES] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
//Array of texture index
static GLuint textureID;




// Initialization routine
void init(){

    // Load external textures.
    glClearColor(0.1,0.1,0.1, 1.0);


    #ifdef LINE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    #endif // LINE
    #ifdef FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif // FILL
    LoadExternalTextures();

    glEnable(GL_DEPTH_TEST); //Enable Depth Test
    glDepthFunc(GL_LESS);

}

void init_lighting(){

    // Turn on OpenGL Lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
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
    GLfloat LightPosition[] = {object_x,object_y,object_z};
    GLfloat Light_Center[] = {0.0, 0.0, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Center);
    //let there be light
    glEnable(GL_LIGHT0);
    //Material light properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material_AmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_Shininess);

}

void check(void){

    //If the ball exceeds the edges, it will rebound and move in the opposite direction - the speed constantly gets added to the x and y coordinates of my object
    //The object will then essentially move thanks to the glTranslate, if we comment that line, it will stop moving.
    //If we multiply x_speed by a constant here, we get a funny exponential acceleration and the ball will rebound super fast
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
    if(object_z > object_z_max){
        object_z = object_z_max;
        z_speed = -z_speed;
    }
    else if(object_z < object_z_min){
        object_z = object_z_min;
        z_speed = -z_speed;
    }

    glutPostRedisplay();

}

void LoadExternalTextures(){

    int index;


    //initialize texture ID
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for(index=0; index<NFACES; index++)
    {
        // Load external textures
        images[index] = readBMP(fileName[index]);
        if(images[index] == NULL)
        {
            printf("Could not open file %s\n", fileName[index]);
        }
        else
        {
            printf("Loaded image %s - %d x %d pixels \n", fileName[index], images[index]->sizeX, images[index]->sizeY);
        }
         // generate textures
        glTexImage2D(facePos[index], 0, GL_RGBA, images[index]->sizeX, images[index]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[index]->data);
    }


    //Set parameters to generate s, t, and r texture mapping coordinates
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); //GL_BLEND AND GL_MODULATE seem to be work quite well
    /*
    // Define how RGB components are computed
    // Interpolate RGB with RGB
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
    // GL_CONSTANT refers to the call we make with
    // glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColor)
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);

    // Define how Alpha component is computed
    // Interpolate ALPHA with ALPHA
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE);
    // GL_CONSTANT refers to the call we make with
    // glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColor)
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
    //Specify how texture values combine with current surface color values.
    */

}

//Callback Handler
void display(){

    int index=0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear Color buffer bit and depth buffer bit
    //Initialize model view transforms
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Light
    init_lighting();

    glRotatef(angle, 0.0, 1.0, 0.0);

        glPushMatrix();
            //SPHERE
            glTranslatef(object_x, object_y, object_z); //makes the ball move
            glutSolidSphere(object_radius,nslices,nstacks);
            //Animation
            object_x += x_speed;
            object_y += y_speed;
            object_z += z_speed;

            //Check if the object touches the edges
            check();
            //swap front and back buffers
        glPopMatrix();

        glDepthMask(GL_FALSE);
        glRotatef(30.0, 0.0, 1.0, 0.0);
        glRotatef(25.0, 1.0, 0.0, 0.0);

        glPushMatrix();
            LoadExternalTextures();
            glEnable(GL_TEXTURE_CUBE_MAP);
            //Activate texture object.
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            //Enable automatic generation of s,t and r coordinates
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);

            //glBlendFunc defines the operation of blending for all draw buffers when it is enabled
            //Pixels can be drawn using a function that blends the incoming (source) RGBA values with the RGBA values that are already in the frame buffer (the destination values).
            //Blending is initially disabled. Use glEnable and glDisable with argument GL_BLEND to enable and disable blending.
            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR ); //( 1 1 1 1 )
            glColor4f(1.0, 0.0, 0.0, 0.5);
            glutSolidCube(1.0);
            //Material light properties

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cube_material_AmbAndDif);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cube_material_Spec);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, cube_material_Shininess);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);

            glDisable(GL_BLEND);

            for(index=0; index<NFACES; index++) {
                    if (images[index] != NULL) {
                        // freee allocated memory
                        free(images[index]->data);
                    }
            }
        glPopMatrix();

        glDepthMask(GL_TRUE);

    glutSwapBuffers();

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
        case '+':
            if(angle>1)
                angle+=1;
                glutPostRedisplay();
            break;
        case '-':
            if(angle<180)
                angle-=1;
                glutPostRedisplay();
            break;

        default:
            // Do nothing
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
            //Change colour of ambient light to red
            for(ind = 0; ind < 4; ind++)
                lightAmb[ind] = red[ind];
            break;

        case 2:
            //Change colour of ambient light to green
            for(ind = 0; ind < 4; ind++)
                lightAmb[ind] = green[ind];
            break;

        case 3:
             //Change colour of ambient light to blue
            for(ind = 0; ind < 4; ind++)
                lightAmb[ind] = blue[ind];
            break;

        case 4:
            //Change colour of diffuse light to red
            for(ind = 0; ind < 4; ind++)
                lightDif[ind] = red[ind];
            break;

        case 5:
            //Change colour of diffuse light to green
            for(ind = 0; ind < 4; ind++)
                lightDif[ind] = green[ind];
            break;

        case 6:
            //Change colour of diffuse light to blue
            for(ind = 0; ind < 4; ind++);{
                lightDif[ind] = blue[ind];
            }
            break;

        case 7:
            //Change colour of specular light to red
            for(ind = 0; ind < 4; ind++)
                lightSpec[ind] = red[ind];
            break;

        case 8:
            //Change colour of specular light to green
            for(ind = 0; ind < 4; ind++)
                lightSpec[ind] = green[ind];
            break;

        case 9:
            //Change colour of specular light to blue
            for(ind = 0; ind < 4; ind++)
                lightSpec[ind] = blue[ind];
            break;

        default:
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

// Main function
int main(int argc, char **argv){


    //gladLoadGLLoader((GLADloadproc)GetProcAddress(CONTEXT,));
    int window_width = 800;
    int window_height = 800;
    int window_pos_x = 0;
    int window_pos_y = 0;

    glutInit(&argc, argv); //pass potential arguments to glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //Enable Double buffering
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(window_pos_x, window_pos_y);
    glutCreateWindow("OpenGL Window");
    printf("Press ESC to quit\n");
    printf("Press 1 and 2 to increase/decrease the materials light specular component.\n");
    printf("Press 3 and 4 to increase/decrease the materials shininess.\n");
    printf("Press 5 and 6 to increase/decrease the materials light ambient/diffuse component.\n");
    printf("Press + and - to transform the scene\n");
    glutKeyboardFunc(keyInput);
    makeMenu();
    init(); //initiation function
    glutDisplayFunc(display); //set display function
    glutMainLoop(); //initialize glut main loop

    return 0;
}
