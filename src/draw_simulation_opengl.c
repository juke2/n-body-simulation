#include <GL/osmesa.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

const int window_size_x = 800;
const int window_size_y = 600;
double aspect = (double)window_size_x / (double)window_size_y;

enum POINTER_OFFSET{
    xpos,
    ypos,
    zpos,
    red,
    green,
    blue,
    radius,
    NUM_POINTER_OFFSETS
};

typedef struct
{
    double xpos;
    double ypos;
    double zpos;
} camera_data;


typedef struct
{
double* data;
int num_points;
} current_tick_datum;
current_tick_datum tick_datum;
const int buffer_len = 1000;
int current_point = 0;
char* input_buffer;
FILE* infile;
camera_data camera_pos;

int input_next_line(){
    double* points = tick_datum.data;
    char* buffer = input_buffer;
    char c;
    int index_offset = 0, buffer_ind = 0, i = 0, line_count = 0;
    while(((c = getc(infile)) != EOF)){
        if(c == ' ' || c == '\n'){
            points[index_offset] = atof(buffer);
            index_offset++;
            for(i = 0; i < buffer_ind; i++){
                // atof ignores all whitespace characters before number 
                // (and all characters after it hits a non-numerical char)
                // so this should work
                buffer[i] = ' ';
            }
            buffer_ind = 0;

            if(c == '\n'){
                line_count++;
                if(line_count == tick_datum.num_points){
                    return 0;
                }
            }

        }
        else{
            if(buffer_ind == buffer_len){
                free(buffer);
                printf("BUFFER OVERFLOW ON READ LOOP");
                return -1;
            }
            buffer[buffer_ind] = c;
            buffer_ind++;
        }
    }
    return 0;
}

void display() {
    int ind, i;
    double* dat = tick_datum.data;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION); // change to editing projection matrix
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(90.0, aspect, 0.0, 300.0);

    glMatrixMode(GL_MODELVIEW); // change to model/view stack
    glLoadIdentity();
    gluLookAt(camera_pos.xpos, camera_pos.ypos, camera_pos.zpos, 0, 0, 0, 0, 1, 0);
    for(i = 0; i < tick_datum.num_points; i++){
        ind = i * NUM_POINTER_OFFSETS;
        glPushMatrix();
        glColor3f(dat[ind + red],dat[ind + green],dat[ind + blue]);
        glTranslatef(dat[ind + xpos],dat[ind + ypos],dat[ind + zpos]);
        glutSolidSphere(dat[ind + radius], 30, 30);
        // printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",dat[ind + xpos],dat[ind + ypos],dat[ind + zpos],dat[ind + red],dat[ind + green],dat[ind + blue],dat[ind + radius]);
        glPopMatrix();
    }

    glMatrixMode(GL_PROJECTION); 
    glPopMatrix();
    glutSwapBuffers();
    glFlush();
}

void timer(int v) {
    // printf("TIME TICK: time at call = %d",current_point);
    if(current_point < tick_datum.num_points){
        current_point += 1;
    }
    else{
        current_point = 0;
        rewind(infile);
    }
    input_next_line();
    glutPostRedisplay();

    glutTimerFunc(1000/10, timer, v);
}

void reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    GLfloat aspect = (GLfloat)(w) / (GLfloat)(w);
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
    gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);
    glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
    // glEnable(GL_CULL_FACE);
}



void key_handler(unsigned char key, int x, int y) {
    printf("r = <%lf, %lf, %lf>\n",camera_pos.xpos, camera_pos.ypos, camera_pos.zpos);
    printf("radius: %lf",tick_datum.data[radius]);
    switch (key) {
        case 'w': camera_pos.xpos -= 0.05; break;
        case 'a': camera_pos.zpos -= 0.05; break;
        case 's': camera_pos.xpos += 0.05; break;
        case 'd': camera_pos.zpos += 0.05; break;
        case 'q': camera_pos.ypos -= 0.05; break;
        case 'e': camera_pos.ypos += 0.05; break;
    }
    glutPostRedisplay();
}

int process_args(int argc, char** argv){
    if(argc >= 2){
        tick_datum.num_points = atoi(argv[1]);
        if(argc >= 3){
            infile = fopen(argv[2], "r");
        }
        else{
            infile = fopen("./data/input_opengl.dat", "r");
        }
        return 0;
    }
    else{
        printf("Number of arguments does not match expected. Expected at least 2 arguments, recieved %d", argc);
        return -1;
    }
    
}



int main_draw(int argc, char** argv) {
    int result_args_process = process_args(argc, argv);
    if(result_args_process == -1){
        return 1;
    }
    else{
        input_buffer = (char*)(malloc(sizeof(char) * buffer_len));
        tick_datum.data = (double*)(malloc(sizeof(double) * tick_datum.num_points * NUM_POINTER_OFFSETS));
        camera_pos.xpos = 5; camera_pos.ypos = 5; camera_pos.zpos = 5;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowPosition(80, 80);
        glutInitWindowSize(window_size_x, window_size_y);
        glutCreateWindow("N-body simulation");
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);
        glutKeyboardFunc(key_handler);
        glutTimerFunc(100, timer, 0);
        init();
        glutMainLoop();

        fclose(infile);
        free(input_buffer);
        free(tick_datum.data);
    }
    return 0;
}