#include <stdio.h>
#include <GL/osmesa.h>
#include <GL/osmesa.h>
#include <GL/glut.h>

#ifndef DRAW_SIMULATION_OPENGL_H
#define DRAW_SIMULATION_OPENGL_H


typedef struct
{
double* data;
int num_points;
} current_tick_datum;


typedef struct
{
    double xpos;
    double ypos;
    double zpos;
} camera_data;

extern const int window_size_x;
extern const int window_size_y;
extern double aspect;
extern current_tick_datum tick_datum;
extern const int buffer_len;
extern int current_point;
extern char* input_buffer;
extern FILE* infile;
extern camera_data camera_pos;

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



int input_next_line();
void display();
void timer(int v);
void reshape(GLint w, GLint h);
void init();
void key_handler(unsigned char key, int x, int y);
int process_args(int argc, char** argv);
int main_draw(int argc, char** argv);

#endif