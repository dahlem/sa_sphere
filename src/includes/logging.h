#ifndef LOGGING_H
#define LOGGING_H

#define SUCCESS 1
#define FAIL -1

void logging_close();

int logging_open();

void logging_logBest(double x, double y, double z);

void logging_logInitial(double x, double y, double z);

void logging_logSim(long iteration, double bestDistance, double deltaDistance,
                    double temperature, double variance, int accpeted);

void logging_logParam(long seed, int iteration, int transmitters, double initialTemperature,
                      double damping, int uniform);


#endif /* LOGGING_H */
