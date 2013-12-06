#ifndef SPHERE_H
#define SPHERE_H

#include "vector.h"

/**
 * The circumference of a unit sphere. \f$ C = 2 * \pi * r\f$, where \f$ r = 1 \f$.
 */
#define CIRCUMFERENCE 2 * M_PI



void sphere_initialiseUniformPoints(struct vector_t *const transmitters, const int numberTrans);
void sphere_initialiseCluster(struct vector_t *const transmitters, const int numberTrans);
double sphere_rieszEnergy(const struct vector_t *const transmitters, const int numberTrans);
double sphere_distance(const struct vector_t *const transmitters, const int numberTrans);
double sphere_distance2(const struct vector_t *const transmitters, const int numberTrans, const int index);
struct vector_t sphere_getPoint();
void sphere_selectClosest(struct vector_t *const transmitters, const int numberTrans, int* index_mim);
struct vector_t sphere_walk(const struct vector_t *const transmitter, const double variance);
void sphere_moveApart(struct vector_t *const transmitterA, struct vector_t *const transmitterB, const double variance);


#endif /* SPHERE_H */
