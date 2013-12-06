/**
 * Logging facility for the simulated annealing.
 *
 * @author Dominik Dahlem
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "logging.h"


#define DATE_MAX_SIZE 15


/**
 * log files for the simulation results, the best configuration, the initial configuration,
 * the parameters for a given simulation.
 */
FILE *all, *best, *initial, *param;


/**
 * private method to log vector information.
 *
 * @param FILE* the file to log to
 * @param double the x-coordinate
 * @param double the y-coordinate
 * @param double the z-coordinate
 */
void logVector(FILE *file, double x, double y, double z)
{
    fprintf(file, "%f,%f,%f\n", x, y, z);
}

/**
 * Open the log files. This function is responsible of opening the three log files,
 * sim.log, best.log, and initial.log. Those log files are put into a time-stapmed
 * directory with the format YYYYMMDDhhmmss, so that we can run multiple simulations
 * without having to append to files and being able to correlate the parameters to
 * initial and best configurations.
 *
 * @return int returns the status of the log files: 0 for failure, 1 for success.
 */
int logging_open()
{
    time_t time_now;
    struct tm *time_ptr;
    int status;
    char time_str[DATE_MAX_SIZE];
    const char *log_base_dir = "./log/";
    const char *log_sim_name = "/sim.log";
    const char *log_best_name = "/best.log";
    const char *log_initial_name = "/initial.log";
    const char *log_param_name = "/param.log";
    char *log_dir, *log_sim, *log_best, *log_initial, *log_param;
    int logdirSize;


    /* we go through a bit of labour to create a time-stamped directory */
    /* get the current time */
    time(&time_now);
    time_ptr = gmtime(&time_now);
    strftime(time_str, DATE_MAX_SIZE, "%Y%m%d%H%M%S", time_ptr);

    /* allocate memory for the file names */
    logdirSize = (strlen(log_base_dir) + strlen(time_str) + 1);
    log_dir = (char *) malloc(logdirSize * sizeof(char));
    log_sim = (char *) malloc((logdirSize + strlen(log_sim_name)) * sizeof(char));
    log_best = (char *) malloc((logdirSize + strlen(log_best_name)) * sizeof(char));
    log_initial = (char *) malloc((logdirSize + strlen(log_initial_name)) * sizeof(char));
    log_param = (char *) malloc((logdirSize + strlen(log_param_name)) * sizeof(char));

    /* initialise the char* */
    memset(log_dir, '\0', logdirSize);
    memset(log_sim, '\0', logdirSize + strlen(log_sim_name));
    memset(log_best, '\0', logdirSize + strlen(log_best_name));
    memset(log_initial, '\0', logdirSize + strlen(log_initial_name));
    memset(log_param, '\0', logdirSize + strlen(log_param_name));

    /* construct the file names */
    strcat(log_dir, log_base_dir);
    strcat(log_dir, time_str);
    strcat(log_sim, log_dir);
    strcat(log_sim, log_sim_name);
    strcat(log_best, log_dir);
    strcat(log_best, log_best_name);
    strcat(log_initial, log_dir);
    strcat(log_initial, log_initial_name);
    strcat(log_param, log_dir);
    strcat(log_param, log_param_name);

    /* create directory with mode 700 */
    status = mkdir(log_dir, S_IRWXU);

    if (status == FAIL) {
        fprintf(stderr, "Could not create directory %s\n", log_dir);
    } else {
        all = fopen(log_sim, "w");
        best = fopen(log_best, "w");
        initial = fopen(log_initial, "w");
        param = fopen(log_param, "w");

        if ((all != NULL) && (best != NULL) && (initial != NULL) && (param != NULL)) {
            fprintf(all, "Iteration,Distance,DistanceDelta,Temperature,Variance,Accepted\n");
            fprintf(best, "x,y,z\n");
            fprintf(initial, "x,y,z\n");
            fprintf(param, "RandomNum,Iteration,Points,TMax,TDamping,"
                    "UniformInitialConfiguration\n");
            status = SUCCESS;
        } else {
            logging_close();
            status = FAIL;
        }
    }

    /* free up the memory again */
    free(log_dir);
    free(log_sim);
    free(log_best);
    free(log_initial);
    free(log_param);

    return status;
}

/**
 * Close the log files.
 */
void logging_close()
{
    if (all != NULL) {
        fclose(all);
    }
    if (best != NULL) {
        fclose(best);
    }
    if (initial != NULL) {
        fclose(initial);
    }
    if (param != NULL) {
        fclose(param);
    }
}

/**
 * Log the best result at the end of the simulation.
 *
 * @param long the random seed
 * @param int the iteration of the inner loop
 * @param int the number of points
 * @param double the initial temperature setting
 * @param double the damping factor
 * @param int flag indicating the initial method of distributing points across the sphere
 */
void logging_logParam(long seed, int iteration, int points, double initialTemperature,
                      double damping, int uniform)
{
    fprintf(param, "%ld,%d,%d,%f,%f,%d\n",
            seed, iteration, points, initialTemperature,
            damping, uniform);
}

/**
 * Log the best result at the end of the simulation.
 *
 * @param double the x-coordinate
 * @param double the y-coordinate
 * @param double the z-coordinate
 */
void logging_logBest(double x, double y, double z)
{
    logVector(best, x, y, z);
}

/**
 * Log the initial configuration before starting the simulation.
 *
 * @param double the x-coordinate
 * @param double the y-coordinate
 * @param double the z-coordinate
 */
void logging_logInitial(double x, double y, double z)
{
    logVector(initial, x, y, z);
}

/**
 * Logging throughout the simulation.
 *
 * @param double the best distance
 * @param double the current temperature
 * @param double the variance of the random walk distance
 */
void logging_logSim(long iteration, double bestDistance, double deltaDistance, double temperature, double variance, int accepted)
{
    fprintf(all, "%ld,%f,%f,%f,%f,%d\n", iteration, bestDistance, deltaDistance, temperature, variance, accepted);
}
