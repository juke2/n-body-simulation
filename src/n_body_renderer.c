#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


enum POINTER_OFFSET{
    mass,
    posx,
    posy,
    posz,
    velx,
    vely,
    velz
};
int x = 0;
enum ERROR_CODES{
    SUCCESS,
    NOT_IMPLEMENTED,
    DIMENSION_UNDEFINED
};
// data is structured in this format:
// m, x, y, z, vx, vy, vz

const int fout = 1;

const double G = 6.674e-20; // grav constant with kg

double norm_2d(double x_1, double y_1, double x_2, double y_2){
    return sqrt(pow(x_2 - x_1,2) + pow(y_2 - y_1,2));
}
double norm_3d(double x_1, double y_1, double z_1, double x_2, double y_2, double z_2){
    return sqrt(pow(x_2 - x_1,2) + pow(y_2 - y_1,2) + pow(z_2 - z_1,2));
}

int simulate_tick(int num_particles, double*particles, double tick_size, FILE* fout_file, int tick){
    double tick_size_squared = tick_size * tick_size;
    int particle = 0, other_particle = 0, num_removed = 0, ind = 0, oind = 0, pind = 0, previous_particle = 0, invalid_val = 0;
    double mult_factor = 0, ax = 0, ay = 0, az = 0, norm = 0, momentum_x = 0, momentum_y = 0, momentum_z = 0;
    double* f_g_net = (double*)(malloc(sizeof(double) * 3));
    double* new_particle_values = (double *)(malloc(sizeof(double) * num_particles * 7));
    for(particle = 0; particle < num_particles; particle++){

        f_g_net[0] = 0;
        f_g_net[1] = 0;
        f_g_net[2] = 0;
        ind = particle * 7;
        // printf("Particle: %d Ind: %d Mass: %.f\n", particle, ind, particles[ind + mass]);
        if(particles[ind + mass] == -1){
            for(invalid_val = 0; invalid_val < 7; invalid_val++){
                new_particle_values[ind + invalid_val] = -1;
            }
            continue;
        }
        for(other_particle = 0; other_particle < num_particles; other_particle++){
            oind = other_particle * 7;
            if(particles[oind + mass] == -1 || other_particle == particle){
                continue;
            }
            norm = norm_3d(particles[ind+posx],particles[ind+posy],particles[ind + posz],particles[oind+posx],particles[oind+posy],particles[oind + posz]);
            // printf("particle: %d other_particle: %d Norm: %e \n",particle,other_particle,norm);
            if(norm < 1){
                // merge particles
                momentum_x = (particles[ind+velx]*particles[ind+mass] + particles[oind+velx]*particles[oind+mass]);
                momentum_y = (particles[ind+vely]*particles[ind+mass] + particles[oind+vely]*particles[oind+mass]);
                momentum_z = (particles[ind+velz]*particles[ind+mass] + particles[oind+velz]*particles[oind+mass]);
                particles[ind+mass] = particles[ind+mass] + particles[oind+mass];
                particles[ind+velx] = momentum_x / particles[ind+mass];
                particles[ind+vely] = momentum_y / particles[ind+mass];
                particles[ind+vely] = momentum_z / particles[ind+mass];
                particles[ind+posx] = (particles[ind+posx] + particles[oind+posx])/2;
                particles[ind+posy] = (particles[ind+posy] + particles[oind+posy])/2;
                particles[ind+posz] = (particles[ind+posz] + particles[oind+posz])/2;
                f_g_net[0] = 0;
                f_g_net[1] = 0;
                f_g_net[2] = 0;
                for(invalid_val = 0; invalid_val < 7; invalid_val++){
                    particles[oind + invalid_val] = -1;
                }

                // calculate previous interactions (help me)
                for(previous_particle = 0; previous_particle <= other_particle; previous_particle++){
                    pind = previous_particle * 7;
                    norm = norm_3d(particles[ind+posx],particles[ind+posy],particles[ind + posz],particles[oind+posx],particles[oind+posy],particles[oind + posz]);
                    if(particles[oind + mass] == -1 || other_particle == particle){
                        continue;
                    }
                    mult_factor= -G * particles[ind + mass] * particles[pind + mass] / pow(norm,3.0);
                    f_g_net[0] += (particles[ind+posx] - particles[pind+posx]) * mult_factor;
                    f_g_net[1] += (particles[ind+posy] - particles[pind+posy]) * mult_factor;
                    f_g_net[2] += (particles[ind+posz] - particles[pind+posz]) * mult_factor;
                }
            }
            else{
                mult_factor= -G * particles[ind + mass] * particles[oind + mass] / pow(norm,3.0);
                f_g_net[0] += (particles[ind+posx] - particles[oind+posx]) * mult_factor;
                f_g_net[1] += (particles[ind+posy] - particles[oind+posy]) * mult_factor;
                f_g_net[2] += (particles[ind+posz] - particles[oind+posz]) * mult_factor;
            }
        }
        // printf("Fg_net = [%e,%e]\n",f_g_net[0],f_g_net[1]);
        ax = f_g_net[0]/particles[ind + mass]; // I know you could just mult without the mass... its fine don't worry
        ay = f_g_net[1]/particles[ind + mass];
        az = f_g_net[2]/particles[ind + mass];
        new_particle_values[ind + mass] = particles[ind + mass];
        new_particle_values[ind + velx] = particles[ind + velx] + ax * tick_size;
        new_particle_values[ind + vely] = particles[ind + vely] + ay * tick_size;
        new_particle_values[ind + velz] = particles[ind + velz] + az * tick_size;
        new_particle_values[ind + posx] = particles[ind + posx] + particles[ind + velx] * tick_size + 0.5 * ax * tick_size_squared;
        new_particle_values[ind + posy] = particles[ind + posy] + particles[ind + vely] * tick_size + 0.5 * ay * tick_size_squared;
        new_particle_values[ind + posz] = particles[ind + posz] + particles[ind + velz] * tick_size + 0.5 * az * tick_size_squared;
    }
    if(fout && fout_file != NULL){
        for(int i = 0; i < num_particles; i++){
            ind = i * 7;
            fprintf(fout_file, "%d %d %e %e %e %e %e %e %e\n",tick,i,new_particle_values[ind + mass],new_particle_values[ind + posx],new_particle_values[ind + posy],new_particle_values[ind + posz],new_particle_values[ind + velx],new_particle_values[ind + vely],new_particle_values[ind + velz]);
        }
    }
    for(particle = 0; particle < num_particles; particle++){
        ind = particle * 7;
        particles[ind + mass] = new_particle_values[ind + mass];
        particles[ind + posx] = new_particle_values[ind + posx];
        particles[ind + posy] = new_particle_values[ind + posy];
        particles[ind + posz] = new_particle_values[ind + posz];
        particles[ind + velx] = new_particle_values[ind + velx];
        particles[ind + vely] = new_particle_values[ind + vely];
        particles[ind + velz] = new_particle_values[ind + velz];
    }
    free(new_particle_values);
    free(f_g_net);
    return 0;
}

int n_body_simulaton(int num_particles, double* initial_values, int num_ticks, double tick_size){
    int particle = 0, datapoint = 0, tick = 0;
    double * particles = (double*)(malloc(sizeof(double) * num_particles * 7));
    FILE* fout_file = fopen("./data/out.dat","w");
    
    for(particle = 0; particle < num_particles; particle++){
        for(datapoint = 0; datapoint < 7; datapoint++){
            particles[particle*7 + datapoint] = initial_values[particle*7 + datapoint];
        }
    }
    for(tick = 0; tick < num_ticks; tick++){
        simulate_tick(num_particles, particles,tick_size,fout_file,tick);
    }

    free(particles);
    fclose(fout_file);
    return 0;
}

int read_data_to_particles(int num_particles, double* particles, FILE* infile){
    const int buffer_len = 1000;
    char c;
    char* buffer = (char*)(malloc(sizeof(char) * buffer_len));
    int index_offset = 0, buffer_ind = 0, i = 0;
    while((c = getc(infile)) != EOF){
        if(c == ' ' || c == '\n'){
            particles[index_offset] = atof(buffer);
            index_offset++;
            for(i = 0; i < buffer_ind; i++){
                // atof ignores all whitespace characters before number 
                // (and all characters after it hits a non-numerical char)
                // so this should work
                buffer[i] = ' ';
            }
            buffer_ind = 0;

        }
        else{
            if(buffer_ind == buffer_len){
                free(buffer);
                return -1;
            }
            buffer[buffer_ind] = c;
            buffer_ind++;
        }
    }
    free(buffer);
    return 0;
}

int get_num_particles(FILE* infile){
    const int buffer_len = 1000;
    char c;
    char* buffer = (char*)(malloc(sizeof(char) * 1000));
    int i = 0, buffer_ind = 0, result = 0;
    while((c = getc(infile)) != EOF){
        if(c == '\n'){
            result = atoi(buffer);
            free(buffer);
            return result;
        }
        else{
            if(buffer_ind == buffer_len){
                free(buffer);
                return -1;
            }
            buffer[buffer_ind] = c;
            buffer_ind++;
        }
    }
    free(buffer);
    return -1;
}

int main_simulate(int argc, char **argv)
{
    FILE* input_file = fopen("./data/in.dat","r");
    int nparticles = get_num_particles(input_file);
    if(nparticles == -1){
        return -1;
    }
    double* test_datum = (double*)(malloc(sizeof(double) * nparticles * 7));
    read_data_to_particles(nparticles,test_datum,input_file);
    n_body_simulaton(nparticles,test_datum,5000,86400);
    free(test_datum);
    fclose(input_file);
    return 0;
    
}



