#include <stdio.h>

#ifndef N_BODY_RENDERER_H
#define N_BODY_RENDERER_H

enum ERROR_CODES{
    SUCCESS,
    NOT_IMPLEMENTED,
    DIMENSION_UNDEFINED
};

extern int x;
extern const int fout;
extern const double G; 
double norm_2d(double x_1, double y_1, double x_2, double y_2);
double norm_3d(double x_1, double y_1, double z_1, double x_2, double y_2, double z_2);
int simulate_tick(int num_particles, double*particles, double tick_size, FILE* fout_file, int tick);
int n_body_simulaton(int num_particles, double* initial_values, int num_ticks, double tick_size);
int read_data_to_particles(int num_particles, double* particles, FILE* infile);
int get_num_particles(FILE* infile);
int main_simulate(int argc, char **argv);

#endif