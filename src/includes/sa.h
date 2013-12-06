#ifndef SA_H
#define SA_H

#include "global.h"
#include "sphere.h"
#include "vector.h"
#include "logging.h"


/**
 * @name Temperature Settings
 */
//@{
/**
 * initial temperature
 */
#define T_INITIAL 30.0

/**
 * minimum temperature
 */
#define T_MIN 0.1

/**
 * Number of iterations for the inner loop to explore a given temperature.
 */
#define T_ITERATION 100

/**
 * Damping factor for the annealing process.
 */
#define T_DAMPING 0.99

/**
 * Boltzmann constant
 */
#define BOLTZMANN_CONSTANT 1.0
//@}



void sa_energy(struct vector_t *transmitters, const struct globalArgs_t const* globalArgs);
void sa_distance(struct vector_t *transmitters, const struct globalArgs_t const* globalArgs);
void sa_closeness(struct vector_t *transmitters, const struct globalArgs_t const* globalArgs);

#endif /* SA_H */
