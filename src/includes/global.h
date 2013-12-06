#ifndef GLOBAL_H
#define GLOBAL_H

/**
 * A structure to capture the global arguments passed into application on the
 * command-line.
 */
struct globalArgs_t {
    long seed; /** random seed */
    int uniform; /** flag to indicate whether to distribute the transmitters uniformly for the initial configuration */
    int iter; /** iteration count for the inner loop */
    int n; /** number of transmitters */
    double temp; /** initial temperature */
    double damping; /** damping factor */
} globalArgs;

#endif /* GLOBAL_H */
