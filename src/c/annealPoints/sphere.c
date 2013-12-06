/**
 * This module implements functions related to sphere operations.
 *
 * @author Dominik Dahlem
 */
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sphere.h"
#include "vector.h"


/**
 * Returns a uniform random number in the range of (-1, 1).
 *
 * @return double the uniform random number
 */
double getCoordinate()
{
    return 2.0 * drand48() - 1.0;
}

/**
 * returns a normally distributed variable using the box-muller method.
 * - Step 1: generate 2 uniform variate random numbers, u and v;
 * - Step 2: \f$ y = \sqrt{-2 * \ln{u}} * \cos{2 * \pi (v - 0.5)} \f$
 *
 * @return double the normally distributed variable \f$ \in [0, 1) \f$
 */
double normalRV1()
{
    double u = 0.0;
    double v = 0.0;

    u = drand48();
    v = drand48();

    return (sqrt(-2 * log(u)) * cos(2 * M_PI * v));
}

double normalRV()
{
    double u = 0.0;
    double v = 0.0;
    double w;

    do {
        u = getCoordinate();
        v = getCoordinate();
        w = u * u + v * v;
    } while (w >= 1.0);

    return u * sqrt((-2 * log(w)) / w);
}

/**
 * Returns the euclidean distance between two vectors on a sphere. The equation used is
 * \f$ \sqrt{\mid A - B \mid} \f$
 *
 * @param const struct vector_t* vector A
 * @param const struct vector_t* vector B
 * @return double the geodesic distance between two vectors on a sphere.
 */
double euclideanDistance(const struct vector_t *const pointA, const struct vector_t *const pointB)
{
    double distance = 0.0;
    struct vector_t dv;

    dv.x = pointA->x - pointB->x;
    dv.y = pointA->y - pointB->y;
    dv.z = pointA->z - pointB->z;

    distance = sqrt(vector_dotProduct(&dv, &dv));

    return distance;
}

/**
 * Returns the distance between two vectors on a sphere. The equation used is
 * \f$ \arccos{(A \cdot B) / (\mid A \mid * \mid B \mid)} * r \f$, where
 * \f$ \mid A \mid, \mid B \mid, and r = 1\f$. It gives the arc length in radians.
 * Multiplying with \f$ \pi \f$ gives the geodesic on the unit sphere.
 *
 * @param const struct vector_t* vector A
 * @param const struct vector_t* vector B
 * @return double the geodesic distance between two vectors on a sphere.
 */
double geodesic(const struct vector_t *const pointA, const struct vector_t *const pointB)
{
    double distance = 0.0;

    distance = acos(vector_dotProduct(pointA, pointB));

    return distance * M_PI;
}

/**
 * Returns the distance between two vectors on a sphere. The euclidean distance is used here, because
 * it is faster to calculate and it doesn't change the behaviour of the simulated annealing compared
 * to the geodesic distance.
 *
 * @param const struct vector_t* vector A
 * @param const struct vector_t* vector B
 * @return double the geodesic distance between two vectors on a sphere.
 */
double distance(const struct vector_t *const pointA, const struct vector_t *const pointB)
{
    return euclideanDistance(pointA, pointB);
}

/**
 * Returns a point on a unit sphere. This function uses a method proposed by Marsaglia in 1972.
 * - Step 1: get two uniform variate random numbers u, v \f$ \in (-1, 1) \f$.
 * - Step 2: if \f$ u^2 + v^2 > 1 \f$ return to step 1.
 * - Step 3: Otherwise:
 *   \f{eqnarray*}
 *     x &=& 2 * u * \sqrt{u^2 - v^2} \\
 *     y &=& 2 * v * \sqrt{u^2 - v^2} \\
 *     z &=& 1 - 2 * (u^2 + v^2)
 *   \f}
 *
 * @return struct vector_t a point on the sphere given the algoritm described above.
 */
struct vector_t sphere_getPoint()
{
    struct vector_t vector;
    double x, y, x_squared, root;

    x = y = x_squared = root = 0.0;

    /* use the rejection method from Marsaglia (1972) to generate */
    /* uniformly distributed points on a sphere */
    do {
        x = getCoordinate();
        y = getCoordinate();
        x_squared = (x * x) + (y * y);
    } while (x_squared >= 1);

    root = sqrt(1 - (x * x) - (y * y));
    vector.x = 2 * x * root;
    vector.y = 2 * y * root;
    vector.z = 1 - 2 * x_squared;

    return vector;
}

/**
 * select the two points that have the shortest distance to each other.
 *
 * @param struct vector_t *const the point array
 * @param const int the number of points
 * @int* the array of indeces for the two closes points
 */
void sphere_selectClosest(struct vector_t *const points, const int numberTrans, int* index_min)
{
    double dist_min = DBL_MAX;
    double dist;
    int index_min_1, index_min_2;
    int i = 0;
    int j = 0;

    for (i = 0; i < numberTrans - 1; i++) {
        for (j = i + 1; j < numberTrans; j++) {
            dist = distance((points + i), (points + j));

            if (dist < dist_min) {
                index_min_1 = i;
                index_min_2 = j;
                dist_min = dist;
            }
        }
    }

    index_min[0] = index_min_1;
    index_min[1] = index_min_2;
}

/**
 * Perform a random walk on a sphere, given the current position of the walker and the maximum
 * distance.
 *
 * - Step 1: Generate a uniform variate number u, v, w \f$ \in [0, 1) \f$
 * - Step 2:
 *   \f{eqnarray*}
 *     x &=& x + u * \sigma \\
 *     y &=& y + u * \sigma \\
 *     z &=& z + u * \sigma \\
 *   \f}
 * - Step 3: Rescale the vector to map onto the unit sphere again. The scaling factor is
 *   \f$ 1/\sqrt{x^2 + y^2 + z^2} \f$
 *
 * @param const struct vector_t *const the random walker
 * @param const double the variance
 * @return struct vector_t the new location of the random walker
 */
struct vector_t sphere_walk(const struct vector_t *const point, const double variance)
{
    struct vector_t vector;
    double standardDeviation;

    standardDeviation = sqrt(variance);

    vector.x = point->x + (normalRV() * standardDeviation);
    vector.y = point->y + (normalRV() * standardDeviation);
    vector.z = point->z + (normalRV() * standardDeviation);

    vector_normalise(&vector);

    return vector;
}

/**
 * Move two vectors away from each other. The variance determines the rate at which the two points are moved
 * away from each other.
 *
 * @param struct vector *const the first point
 * @param struct vector *const the second point
 * @param const double the rate at which the two vectors are moved away from each other
 */
void sphere_moveApart(struct vector_t *const pointA, struct vector_t *const pointB, const double variance)
{
    double u = 1 + variance;
    struct vector_t v1, v2;

    v1.x = pointA->x;
    v1.y = pointA->y;
    v1.z = pointA->z;
    v2.x = pointB->x;
    v2.y = pointB->y;
    v2.z = pointB->z;

    pointA->x = v1.x + u * (v1.x - v2.x);
    pointA->y = v1.y + u * (v1.y - v2.y);
    pointA->z = v1.z + u * (v1.z - v2.z);
    pointB->x = v1.x - u * (v1.x - v2.x);
    pointB->y = v1.y - u * (v1.y - v2.y);
    pointB->z = v1.z - u * (v1.z - v2.z);

    vector_normalise(pointA);
    vector_normalise(pointB);
}

struct vector_t sphere_walk2(const struct vector_t *const point, const double variance)
{
    struct vector_t vector_ret, vector_temp;
    double c, s2, cdist, sdist;

    cdist = cos(variance);
    sdist = sin(variance);

    do {
        vector_temp = sphere_getPoint();
        c = vector_dotProduct(&vector_temp, point);
        s2 = 1.0 - c * c;
    } while (s2 < 0.01);

    /* v = SDIST*v + (CDIST*sqrt(s2) - SDIST*c)*u;  */
    vector_ret.x = vector_temp.x * sdist + (cdist * sqrt(s2) - sdist * c) * point->x;
    vector_ret.y = vector_temp.y * sdist + (cdist * sqrt(s2) - sdist * c) * point->y;
    vector_ret.z = vector_temp.z * sdist + (cdist * sqrt(s2) - sdist * c) * point->z;

    vector_normalise(&vector_ret);

    return vector_ret;
}

/**
 * Initialise the first configuration over the sphere clustering around one point.
 *
 * @param struct vector_t *const the allocated points
 * @param const int the number of points
 */
void sphere_initialiseCluster(struct vector_t *const points, const int numberTrans)
{
    int i = 0;
    struct vector_t vector;

    vector = sphere_getPoint();
    (points + 0)->x = vector.x;
    (points + 0)->y = vector.y;
    (points + 0)->z = vector.z;

    for (i = 1; i < numberTrans; i++) {
        vector = sphere_walk(points, 1.0);
        (points + i)->x = vector.x;
        (points + i)->y = vector.y;
        (points + i)->z = vector.z;
    }
}

/**
 * Initialise the first configuration uniformly over the unit sphere.
 *
 * @param struct vector_t *const the allocated point array
 * @param const in the number of points
 */
void sphere_initialiseUniformPoints(struct vector_t *const points, const int numberTrans)
{
    int i = 0;
    struct vector_t vector;

    for (i = 0; i < numberTrans; i++) {
        vector = sphere_getPoint();

        (points + i)->x = vector.x;
        (points + i)->y = vector.y;
        (points + i)->z = vector.z;
    }
}

double sphere_rieszEnergy(const struct vector_t *const points, const int numberTrans)
{
    struct vector_t vector;
    double energy = 0.0;
    int i = 0;
    int j = 0;

    for (i = 0; i < numberTrans - 1; i++) {
        for (j = i + 1; j < numberTrans; j++) {
            vector.x = (points + i)->x - (points + j)->x;
            vector.y = (points + i)->y - (points + j)->y;
            vector.z = (points + i)->z - (points + j)->z;
            energy += log(1/(vector_dotProduct(&vector, &vector)));
        }
    }

    return energy;
}

/**
 * Calculates the geodesic distance between any two points on the sphere. To improve the
 * performance, the distances are only calculated in one direction.
 *
 * @param const struct vector_t *const the allocated point array
 * @param const int the number of points
 * @return the distance between any two points on the sphere
 */
double sphere_distance(const struct vector_t *const points, const int numberTrans)
{
    double dist = 0.0;
    int i = 0;
    int j = 0;

    for (i = 0; i < numberTrans - 1; i++) {
        for (j = i + 1; j < numberTrans; j++) {
            dist += distance((points + i), (points + j));
        }
    }

    return dist;
}
