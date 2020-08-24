
#include <glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#define NFACES 6
#define NVERTICES 4
#define nslices 100
#define nstacks 100
#define object_radius 0.15f


//Object Values
GLfloat object_x = 0.0f; //object center x coordinate
GLfloat object_y = 0.0f;  //object center y coordinate
GLfloat object_z = 0.0f; //object z coordinates


GLfloat x_speed = 0.05f;    //speed in x axis
GLfloat y_speed = 0.007f;   //speed in y axis
GLfloat z_speed = 0.03f; //speed in z axis
//Refresh
GLfloat refresh_ms = 30; //refresh period in milliseconds
GLfloat angle = 10;


GLfloat object_x_min = -0.5 + object_radius; //if we remove the object_radius factor here, the object will bounce only when the center hits the wall
GLfloat object_x_max = 0.5 - object_radius;
GLfloat object_y_min = -0.5 + object_radius;
GLfloat object_y_max = 0.5 - object_radius;
GLfloat object_z_min = -0.5 + object_radius;
GLfloat object_z_max = 0.5 - object_radius;


//Material light properties
GLfloat material_AmbAndDif[] = {0.0, 0.1, 0.9, 1.0};
GLfloat material_Spec[] = {0.5, 0.9, 0.3, 1.0};
GLfloat material_Shininess[] = {2.0};


//Cube Material light properties
GLfloat cube_material_AmbAndDif[] = {0.3, 0.4, 0.5, 1.0};
GLfloat cube_material_Spec[] = {0.1, 0.7, 0.4, 1.0};
GLfloat cube_material_Shininess[] = {5.0};
//Global ambient light
GLfloat globAmb[] = {1.0, 0.0, 1.0, 1.0};
//Light property vectors.
GLfloat lightAmb[] = {0.9, 0.0, 0.0, 1.0};
GLfloat lightDif[] = {0.9, 0.0, 0.0, 1.0};
GLfloat lightSpec[] = {0.0, 1.0, 0.0, 1.0};
GLfloat red[] = {1.0,0.0,0.0,1.0};
GLfloat blue[] = {0.0,0.0,1.0,1.0};
GLfloat green[] = {0.0,1.0,0.0,1.0};


GLfloat vertexArray[NFACES*NVERTICES][3] = {
    // first face - v0,v1,v3,v2
    {0.5, -0.5, 0.5}, // v0
    {0.5, 0.5, 0.5}, // v1
    {-0.5, -0.5, 0.5}, // v3
    {-0.5, 0.5, 0.5}, // v2

    // second face - v3,v2,v7,v6
    {-0.5, -0.5, 0.5}, // v3
    {-0.5, 0.5, 0.5}, // v2
    {-0.5, -0.5, -0.5}, // v7
    {-0.5, 0.5, -0.5}, // v6

    // third face - v7,v6,v4,v5
    {-0.5, -0.5, -0.5}, // v7
    {-0.5, 0.5, -0.5}, // v6
    {0.5, -0.5, -0.5}, // v4
    {0.5, 0.5, -0.5}, // v5

    // forth face - v4, v5, v0, v1
    {0.5, -0.5, -0.5}, // v4
    {0.5, 0.5, -0.5}, // v5
    {0.5, -0.5, 0.5}, // v0
    {0.5, 0.5, 0.5} // v1


};


GLfloat colorArray[NFACES*NVERTICES][3] = {
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},

};


void init_lighting(){

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
    GLfloat LightPosition[] = {object_x,object_y,object_z};
    GLfloat Light_Center[] = {0.0, 0.0, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Center);
    //let there be light
    glEnable(GL_LIGHT0);
    //Material light properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,material_AmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_Shininess);

}


//initialization routine
void init(){

    // Load external textures.
    glClearColor(0.1,0.1,0.1, 1.0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST); //Enable Depth Test
    glDepthFunc(GL_LESS);

}


void check(void){

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

    if(object_z > object_z_max){
        object_z = object_z_max;
        z_speed = -z_speed;
    }
    else if(object_z < object_z_min){
        object_z = object_z_min;
        z_speed = -z_speed;
    }

}


//Callback Handler
void display(){

    GLshort indFace, indVertex;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    init_lighting();
    //Initialize model view transforms
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(angle, 0.0, 1.0, 0.0);
    glRotatef(30.0, 0.0, 1.0, 0.0);
    glRotatef(25.0, 1.0, 0.0, 0.0);

    //CUBE
    //Specifies whether the depth buffer is enabled for writing
    glDepthMask(GL_FALSE);
    glPushMatrix();
    // here we draw the triangle stripss
    for (indFace=0; indFace<NFACES; indFace++) {
        glColor4f(1.0, 0.0, 0.0, 0.5);
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA ); //( 1 1 1 1 )
        glBegin(GL_TRIANGLE_STRIP);
        for(indVertex=0; indVertex<NVERTICES; indVertex++) {
            // extract color
            glColor3fv(colorArray[indFace*NVERTICES + indVertex]);
            // extract vertices
            glVertex3fv(vertexArray[indFace*NVERTICES + indVertex]);
        };
        glEnd();
    };
    //Set cube material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cube_material_AmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cube_material_Spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, cube_material_Shininess);
    //Disable Blend
    glDisable(GL_BLEND);
    glPopMatrix();
    glDepthMask(GL_TRUE);
    //Enable Depth Mask so I can draw my sphere
    //DREW CUBE

    //SPHERE
    glPushMatrix();
    glTranslatef(object_x, object_y, object_z); //makes the ball move
    //place a sphere in the scene
    glColor3f(1.0,0.0,0.0);
    glutSolidSphere(object_radius,nslices,nstacks);
    //Material light properties

    //Animation
    object_x += x_speed;
    object_y += y_speed;
    object_z += z_speed;

    //check if the object touches the edges
    check();
    glPopMatrix();
    //DREW MY CUBE

    glutSwapBuffers(); //swap front and back buffers


}


void timer_callback(int v){

    glutPostRedisplay();
    glutTimerFunc(refresh_ms, timer_callback, 0); // glutTimerFunc registers a timer callback to be triggered in a specified number of milliseconds.

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
            break;
        case '-':
            if(angle<180)
                angle-=1;
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
            break;

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


//Main function
int main(int argc, char **argv){

    int window_width = 800;
    int window_height = 800;
    int window_pos_x = 0;
    int window_pos_y = 0;

    glutInit(&argc, argv); //pass potential arguments to glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //Enable Double buffering
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(window_pos_x, window_pos_y);
    glutCreateWindow("OpenGL Window");
    printf("Press ESC to quit.\n");
    printf("Press 1 and 2 to increase/decrease the material light specular component.\n");
    printf("Press 3 and 4 to increase/decrease the material shininess.\n");
    printf("Press 5 and 6 to increase/decrease the material light ambient/diffuse component.\n");
    glutKeyboardFunc(keyInput);
    glutTimerFunc(0, timer_callback, 0); //  glutTimerFunc registers a timer callback to be triggered in a specified number of milliseconds.
    makeMenu();
    init(); //initiation function
    glutDisplayFunc(display); //set display function
    glutMainLoop(); //initialize glut main loop

    return 0;
}

