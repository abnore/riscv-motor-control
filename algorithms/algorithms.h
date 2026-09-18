#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <math.h>

// math.h defines PI as a double, i will keep everything 32 bit
#define PI_F         ((float)M_PI)
#define DEG_TO_RAD_F (PI_F / 180.f)
#define PHASE_120_F  (2.f * PI_F / 3.f)

// Struct for holding the alpha/beta vectors that clarke transform creates
// from the 3 phases rotatin domain into the 2 phase rotating domain.
// Also inverse park transform creates this.
typedef struct {
    float a;
    float b;
} ab_vector;

// Struct for holding the D and Q vectors in the stationary domain that is
// created by the park transform
typedef struct {
    float d;
    float q;
} dq_vector;

ab_vector clarke_transform(float i_a, float i_b, float i_c);
dq_vector park_transform(float theta_rad, ab_vector ab);
ab_vector inverse_park_transform(float theta_rad, dq_vector dq);

#endif
