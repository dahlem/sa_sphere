/**
 * Contains the simulated annealing algorithms. The default one is sa_distance, while the other ones
 * were mainly used for experimental purposes.
 *
 * @author Dominik Dahlem
 */
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "sa.h"
#include "vector.h"
#include "sphere.h"
#include "logging.h"
#include "global.h"


/**
 * Select a point uniformly random.
 *
 * @return Index into the point array
 */
int selectPoint(int number)
{
    return lrand48() % number;
}

/**
 * Anneal the temperature.
 *
 * @param double* pointer to the temperature to be annealed.
 * @param double* pointer to the damping factor
 */
void anneal(double *temperature, double damping)
{
    *temperature = damping * (*temperature);
}

/**
 * This is the heart of the simulation using simulated annealing.
 *
 * @param struct vector* the points to be distributed across a sphere
 */
void sa_distance(struct vector_t *points, const struct globalArgs_t const* globalArgs)
{
    double temperature = globalArgs->temp;
    double distance_old, distance_new, distance_best, distance_cur, distance_delta, expo, variance;
    struct vector_t new_points[globalArgs->n];
    struct vector_t best_points[globalArgs->n];
    struct vector_t v_new;
    int index = 0;
    int k = 0;
    int accepted = 0;
    long iteration = 0;

    vector_arrayCopy(&new_points[0], &points[0], globalArgs->n);

    distance_best = 0.0;

    do {
        /* select a random walker */
        index = selectPoint(globalArgs->n);
        variance = 0.5 * (1 - exp(-0.5 * temperature));

        for (k = 0; k < globalArgs->iter; k++) {
            /* perform the random walk */
            v_new = sphere_walk(&points[index], variance * variance);

            /* copy the new location of the walker */
            vector_copy(&new_points[index], &v_new);

            /* calcuate the old and the new distance */
            distance_old = sphere_distance(&points[0], globalArgs->n);
            distance_new = sphere_distance(&new_points[0], globalArgs->n);
            distance_delta = distance_new - distance_old;
            accepted = 0;

            expo = exp(-abs(distance_delta) /
                    ((double) BOLTZMANN_CONSTANT * temperature));

            if (distance_new > distance_old) {
                /* accept the new distance, because it is bigger */
                vector_copy(&points[index], &v_new);
                distance_cur = distance_new;
                accepted = 1;

                /*
                 * if the new distance is higher than the best distance,
                 * then keep the best configuration.
                 */
                if (distance_best < distance_new) {
                    vector_arrayCopy(
                            &best_points[0],
                            &points[0],
                            (int) globalArgs->n);

                    distance_best = distance_new;
                    distance_cur = distance_best;
                  }
            } else if (drand48() < expo) {
                /*
                 * if the new distance is not higher than the old one
                 * then accept with a given probability anyway to be able to escape
                 * local minima.
                 */
                vector_copy(&points[index], &v_new);
                distance_cur = distance_old;
                accepted = 1;
            }

            logging_logSim(iteration, distance_cur, distance_delta, temperature, variance, accepted);
            iteration++;
        }

        anneal(&temperature, globalArgs->damping);
    } while (temperature > T_MIN);

    for (k = 0; k < globalArgs->n; k++) {
        logging_logBest((best_points + k)->x, (best_points + k)->y, (best_points + k)->z);
    }
}

/**
 * This is the heart of the simulation using simulated annealing.
 *
 * @param struct vector* the points to be distributed across a sphere
 */
void sa_closeness(struct vector_t *points, const struct globalArgs_t const* globalArgs)
{
    double temperature = globalArgs->temp;
    double distance_old, distance_new, distance_best, distance_cur, distance_delta, expo, variance;
    struct vector_t new_points[globalArgs->n];
    struct vector_t best_points[globalArgs->n];
    struct vector_t v_new[2];
    int index_min[2];
    int k = 0;
    int accepted = 0;
    long iteration = 0;

    vector_arrayCopy(&new_points[0], &points[0], globalArgs->n);

    distance_best = 0.0;

    do {
        /* select a random walker */
        sphere_selectClosest(&points[0], globalArgs->n, index_min);

        // the variance should be between 0.9 and 0
//        variance = 0.9 * (1 - exp(-0.5 * temperature));
        variance = 0.01;


        for (k = 0; k < globalArgs->iter; k++) {
            vector_arrayCopy(
                &new_points[0],
                &points[0],
                (int) globalArgs->n);

            /* perform the random walk */
            sphere_moveApart(&new_points[index_min[0]], &new_points[index_min[1]], variance);

            /* calcuate the old and the new distance */
            distance_old = sphere_distance(&points[0], globalArgs->n);
            distance_new = sphere_distance(&new_points[0], globalArgs->n);
            distance_delta = distance_new - distance_old;
            accepted = 0;

            expo = exp(-abs(distance_delta) /
                    ((double) BOLTZMANN_CONSTANT * temperature));

            if (distance_new > distance_old) {
                /* accept the new distance, because it is bigger */
                vector_copy(&points[index_min[0]], &v_new[0]);
                vector_copy(&points[index_min[1]], &v_new[1]);
                distance_cur = distance_new;
                accepted = 1;

                /*
                 * if the new distance is higher than the best distance,
                 * then keep the best configuration.
                 */
                if (distance_best < distance_new) {
                    vector_arrayCopy(
                            &best_points[0],
                            &points[0],
                            (int) globalArgs->n);

                    distance_best = distance_new;
                    distance_cur = distance_best;
                  }
            } else if (drand48() < expo) {
                /*
                 * if the new distance is not higher than the old one
                 * then accept with a given probability anyway to be able to escape
                 * local minima.
                 */
                vector_copy(&points[index_min[0]], &v_new[0]);
                vector_copy(&points[index_min[1]], &v_new[1]);
                distance_cur = distance_old;
                accepted = 1;
            }

            logging_logSim(iteration, distance_cur, distance_delta, temperature, variance, accepted);
            iteration++;
        }

        anneal(&temperature, globalArgs->damping);
    } while (temperature > T_MIN);

    for (k = 0; k < globalArgs->n; k++) {
        logging_logBest((best_points + k)->x, (best_points + k)->y, (best_points + k)->z);
    }
}

/**
 * This is the heart of the simulation using simulated annealing.
 *
 * @param struct vector* the points to be distributed across a sphere
 */
void sa_energy(struct vector_t *points, const struct globalArgs_t const* globalArgs)
{
    double temperature = globalArgs->temp;
    double energy_old, energy_new, energy_best, energy_cur, energy_delta, expo, variance;
    struct vector_t new_points[globalArgs->n];
    struct vector_t best_points[globalArgs->n];
    struct vector_t v_new;
    int index = 0;
    int k = 0;
    int accepted = 0;
    long iteration = 0;

    vector_arrayCopy(&new_points[0], &points[0], globalArgs->n);

    energy_best = DBL_MAX;

    do {
        /* select a random walker */
        index = selectPoint(globalArgs->n);
        variance = 1 - exp(-0.5 * temperature);

        for (k = 0; k < globalArgs->iter; k++) {
            /* perform the random walk */
            v_new = sphere_walk(&points[index], variance);

            /* copy the new location of the walker */
            vector_copy(&new_points[index], &v_new);

            /* calcuate the old and the new energy */
            energy_old = sphere_rieszEnergy(&points[0], globalArgs->n);
            energy_new = sphere_rieszEnergy(&new_points[0], globalArgs->n);
            energy_delta = energy_new - energy_old;
            accepted = 0;

            expo = exp(-abs(energy_delta) /
                    ((double) BOLTZMANN_CONSTANT * temperature));

            if (energy_new < energy_old) {
                /* accept the new energy, because it is bigger */
                vector_copy(&points[index], &v_new);
                energy_cur = energy_new;
                accepted = 1;

                /*
                 * if the new energy is higher than the best energy,
                 * then keep the best configuration.
                 */
                if (energy_best > energy_new) {
                    vector_arrayCopy(
                            &best_points[0],
                            &points[0],
                            (int) globalArgs->n);

                    energy_best = energy_new;
                    energy_cur = energy_best;
                  }
            } else if (drand48() < expo) {
                /*
                 * if the new energy is not higher than the old one
                 * then accept with a given probability anyway to be able to escape
                 * local minima.
                 */
                vector_copy(&points[index], &v_new);
                energy_cur = energy_old;
                accepted = 1;
            }

            logging_logSim(iteration, energy_cur, energy_delta, temperature, variance, accepted);
            iteration++;
        }

        anneal(&temperature, globalArgs->damping);
    } while (temperature > T_MIN);

    for (k = 0; k < globalArgs->n; k++) {
        logging_logBest((best_points + k)->x, (best_points + k)->y, (best_points + k)->z);
    }
}
