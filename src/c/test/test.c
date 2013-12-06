/*
 * Author: Dominik Dahlem
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vector.h"
#include "sphere.h"


#define POINTS 1000
#define SAMPLES 10


int main(int argc, char** argv)
{
    struct vector_t points[POINTS];
    double samples[SAMPLES];
    int i = 0;
    double mean = 0.0;
    double std = 0.0;

    srand48(12345678);

    for (i = 0; i < SAMPLES; i++) {
        sphere_initialiseUniformPoints(&points[0], (int) POINTS);
        samples[i] = sphere_distance(&points[0], (int) POINTS);
    }

    for (i = 0; i < POINTS; i++) {
        printf("%f,%f,%f\n", (points + i)->x, (points + i)->y, (points + i)->z);
    }

    printf("%f\n", sphere_distance(&points[0], (int) POINTS));

    for (i = 0; i < SAMPLES; i++) {
        mean += samples[i];
    }

    mean /= SAMPLES;

    for (i = 0; i < SAMPLES; i++) {
        std += (samples[i] - mean) * (samples[i] - mean);
    }

    std /= SAMPLES;
    std = sqrt(std);

    printf("Mean %f +/- %f\n", mean, std);

    return 0;
}
