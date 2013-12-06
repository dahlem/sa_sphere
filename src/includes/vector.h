#ifndef VECTOR_H
#define VECTOR_H

struct vector_t {
    double x;
    double y;
    double z;
};

double vector_dotProduct(const struct vector_t *const x, const struct vector_t *const y);

void vector_arrayCopy(struct vector_t *const oldVector, const struct vector_t *const newVector, const int arraySize);

void vector_copy(struct vector_t *const oldVector, const struct vector_t *const newVector);

void vector_normalise(struct vector_t *const vector);

#endif /* VECTOR_H */
