/**
 * The location of N radio transmitters around the world are to determined, such
 * that the masts are as far apart as Write a code that uses the simulated
 * annealing algorithm find the maximum of the function f = min(d_ij),
 * where d_ij the distance (measured on the sphere) between points i and j the
 * unit sphere. Compute the value of f when N = 50, 100.
 *
 * @author Dominik Dahlem
 */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "global.h"
#include "sphere.h"
#include "logging.h"
#include "sa.h"


/**
 * @name Application settings
 */
//@{
/**
 * Number of points to be distributed
 */
#define POINTS 50

/**
 * Coefficient for the variance of the random walk. Set to 1 to cover the whole
 * circumference of a sphere. <1 to cover only a fraction of the circumference.
 */
#define DELTA_MOVE 1.0

/**
 * Default random seed.
 */
#define RANDOM_SEED 751339078

/**
 * Boolean: True.
 */
#define TRUE 1

/**
 * Boolean: False.
 */
#define FALSE 0
//@}

/**
 * getopt configuration of the command-line parameters. All command-line arguments are optional.
 */
static const char *cl_arguments = "uh?r:t:i:d:n:";


/**
 * Display the help message for this application.
 */
void displayHelp()
{
    printf("annealPoints - Uniformly distribute points on a sphere.\n");
    printf(" -d : Damping factor for the annealing process.\n");
    printf(" -i : Number of iterations.\n");
    printf(" -n : Number of Points.\n");
    printf(" -r : Seed for the random number generator.\n");
    printf(" -t : Initial value for the temperature.\n");
    printf(" -u : Flag to indicate uniform initial configuration.\n");
    printf(" -? : This help message.\n");
    printf(" -h : This help message.\n");

    exit(EXIT_SUCCESS);
}

/**
 * Initialise the global parameters.
 */
void init()
{
    globalArgs.seed = (long) RANDOM_SEED;
    globalArgs.uniform = FALSE;
    globalArgs.temp = T_INITIAL;
    globalArgs.iter = T_ITERATION;
    globalArgs.damping = T_DAMPING;
    globalArgs.n = POINTS;
}

/**
 * Process the command-line arguments passed into the application.
 *
 * @param int number of arguments
 * @param char** pointer to the character array representing the command-line parameters
 */
void process_cl(int argc, char **argv)
{
    int opt = 0;
    opt = getopt(argc, argv, cl_arguments);
    while (opt != -1) {
        switch (opt) {
            case 'd':
                globalArgs.damping = atof(optarg);
                break;
            case 'i':
                globalArgs.iter = atoi(optarg);
                break;
            case 'n':
                globalArgs.n = atoi(optarg);
                break;
            case 'r':
                globalArgs.seed = atol(optarg);
                break;
            case 't':
                globalArgs.temp = atof(optarg);
                break;
            case 'u':
                globalArgs.uniform = TRUE;
                break;
            case 'h':
            case '?':
                 displayHelp();
                 break;
            default:
                 break;
        }
        opt = getopt(argc, argv, cl_arguments);
    }
}

/**
 * The main function.
 *
 * @param int number of arguments
 * @param char** pointer to the character array representing the command-line parameters
 * @return the return code of the application.
 */
int main(int argc, char** argv)
{
    struct vector_t *points;
    int k = 0;

    /* initialise the command line parameters */
    init();
    process_cl(argc, argv);

    /* allocate memory for the points on the sphere */
    points = (struct vector_t *) malloc(globalArgs.n * sizeof(struct vector_t));
    srand48(globalArgs.seed);

    /* select the method to set up the initial configuration */
    if (globalArgs.uniform == TRUE) {
        sphere_initialiseUniformPoints(&points[0], globalArgs.n);
    } else {
        sphere_initialiseCluster(&points[0], globalArgs.n);
    }

    /* open the log files */
    if (logging_open() == FAIL) {
        exit(EXIT_FAILURE);
    }

    logging_logParam(globalArgs.seed, globalArgs.iter, globalArgs.n, globalArgs.temp,
                     globalArgs.damping, globalArgs.uniform);

    /* log the initial configuration */
    for (k = 0; k < globalArgs.n; k++) {
        logging_logInitial((points + k)->x,
                           (points + k)->y,
                           (points + k)->z);
    }

    /* start the simulation */
    sa_distance(&points[0], &globalArgs);

    /* clean up everything */
    logging_close();
    free(points);

    return 0;
}
