/**
 * This module provides functions for vectors.
 *
 * @author Dominik Dahlem
 */
#include <assert.h>
#include <math.h>

#include "vector.h"


/**
 * Calculates the dot product of two vectors, \f$ x_1 * y_1 + x_2 * y_2 + x_3 * y_3 \f$
 *
 * @param const struct vector_t *const vector x
 * @param const struct vector_t *const vector y
 * @return the dot product.
 */
double vector_dotProduct(const struct vector_t *const x, const struct vector_t *const y)
{
    return (x->x * y->x) + (x->y * y->y) + (x->z * y->z);
}

/**
 * Copy one vector into another one.
 *
 * @param struct vector_t *const the old vector
 * @param const struct vector_t *const the new vector
 */
void vector_copy(struct vector_t *const oldVector, const struct vector_t *const newVector)
{
    oldVector->x = newVector->x;
    oldVector->y = newVector->y;
    oldVector->z = newVector->z;

    assert(oldVector->x == newVector->x);
    assert(oldVector->y == newVector->y);
    assert(oldVector->z == newVector->z);
}

/**
 * Copy an array of vectors into another one.
 *
 * @param struct vector_t *const the old vector array
 * @param const struct vector_t *const the new vector array
 * @param const int the size of the array
 */
void vector_arrayCopy(struct vector_t *const oldVector, const struct vector_t *const newVector, const int arraySize)
{
    int i = 0;

    for (i = 0; i < arraySize; i++) {
        vector_copy((oldVector + i), (newVector + i));
    }
}

/**
 * Normalise a given vector to unit length, whereby the scaling factor is
 * \f$ 1 / \sqrt{\mid A \mid} \f$
 *
 * @param struct vector_t *const the vector to be normalised.
 */
void vector_normalise(struct vector_t *const vector)
{
    double scale;

    scale = 1 / sqrt(vector_dotProduct(vector, vector));

    vector->x *= scale;
    vector->y *= scale;
    vector->z *= scale;
}
