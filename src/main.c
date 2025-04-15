#include "draw_simulation_opengl.h"
#include "n_body_renderer.h"
#include <GL/osmesa.h>
#include <GL/glut.h>
#define __USE_MISC 1 // for math constants (and probably other stuff idk)
#include <math.h>

const double au_conversion = 6.68459e-9;
double log_base_conversion;

current_tick_datum simulation_info;
const int important_planets_len = 10;
double planetary_colors[] = {
    1.0, 1.0, 0.4, 
    0.6392156862745098, 0.6392156862745098, 0.4588235294117647,
    0.8, 0.8, 0.0,
    0.8823529411764706, 0.8823529411764706, 0.9176470588235294,
    0.0, 0.8, 0.0,
    0.8, 0.0, 0.0,
    1.0, 0.4588235294117647, 0.10196078431372549,
    1.0, 0.8, 0.6,
    0.6, 0.0, 1.0,
    0.0, 0.0, 0.7019607843137254
    };
double planetary_rad[] = {
    696340.0,
    2440.0,
    6051.0,
    1737.0,
    6378.0,
    3390.0,
    69911.0,
    58232.0,
    25362.0,
    24622.0
};

int map_sim_to_draw(){
    int i = 0, ind;
    for(i = 0; i < important_planets_len; i++){
        ind = i*7;
        tick_datum.data[ind + xpos] = au_conversion * simulation_info.data[ind + 1];
        tick_datum.data[ind + ypos] = au_conversion * simulation_info.data[ind + 2];
        tick_datum.data[ind + zpos] = au_conversion * simulation_info.data[ind + 3];
        tick_datum.data[ind + red] = planetary_colors[i*3];
        tick_datum.data[ind + green] = planetary_colors[i*3 + 1];
        tick_datum.data[ind + blue] = planetary_colors[i*3 + 2];
        if(i == 3){
            tick_datum.data[ind + radius] =  fabs(log(planetary_rad[i]/5) * log_base_conversion)/3;
        }
        else{
            tick_datum.data[ind + radius] =  fabs(log(planetary_rad[i]/5) * log_base_conversion);
        }
    }
    for(i = important_planets_len; i < simulation_info.num_points; i++){
        ind = i*7;
        tick_datum.data[ind + xpos] = au_conversion * simulation_info.data[ind + 1];
        tick_datum.data[ind + ypos] = au_conversion * simulation_info.data[ind + 2];
        tick_datum.data[ind + zpos] = au_conversion * simulation_info.data[ind + 3];
        tick_datum.data[ind + red] = 0.8;
        tick_datum.data[ind + green] = 0.4;
        tick_datum.data[ind + blue] = 0.0;
        tick_datum.data[ind + radius] =  fabs(log(pow(simulation_info.data[ind] / (4/3 * 2000 * M_PI),1/3)/5) * log_base_conversion);
    }
    return 0;
}

void timer_live_display(int v) {

    simulate_tick(simulation_info.num_points, simulation_info.data, 86400, NULL /*protected against null pointer, so this just prevents output*/,0);
    map_sim_to_draw();
    // printf("timer go!");
    glutPostRedisplay();

    glutTimerFunc(1000/10, timer_live_display, v);
}

int main(int argc, char **argv)
{
    log_base_conversion = 1/log(1e40);
    printf("%lf\n",1/log(1e40));
    FILE* input_file = fopen("./data/in.dat","r");
    simulation_info.num_points = get_num_particles(input_file);
    tick_datum.num_points = simulation_info.num_points;

    if(tick_datum.num_points == -1){
        return 1;
    }
    tick_datum.data = (double*)(malloc(sizeof(double) * tick_datum.num_points * 7));
    simulation_info.data = (double*)(malloc(sizeof(double) * tick_datum.num_points * 7));
    read_data_to_particles(simulation_info.num_points,simulation_info.data,input_file);
    fclose(input_file);

    camera_pos.xpos = 5; camera_pos.ypos = 5; camera_pos.zpos = 5;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(window_size_x, window_size_y);
    glutCreateWindow("N-body simulation");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key_handler);
    glutTimerFunc(100, timer_live_display, 0);
    init();
    glutMainLoop();

    free(tick_datum.data);
    free(simulation_info.data);
    return 0;

}

