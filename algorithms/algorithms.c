#include <math.h>
#include "motor_control.h"

#define SQRT3 1.73205080757f
#define CLARKE_CONST (1.0f/SQRT3)

ab_vector clarke_transform(float iA, float iB, float iC)
{
    ab_vector ab;

    // Equations simplified in a stable system that obeys Kirchhoff (No leaks)
    // i.e. iA + iB + iC = 0
    ab.a = iA;
    ab.b = (iB - iC) * CLARKE_CONST;

    return ab;
}
/* Takes the ab vector from the clarke transform and a theta as radians,
 * removing the conversion responsibility to outside of the unit */
dq_vector park_transform(float theta_as_rad, ab_vector ab)
{
    // pre compute the sin and cos functions
    float sin_theta = sinf(theta_as_rad);
    float cos_theta = cosf(theta_as_rad);

    dq_vector dq;

    dq.q = -sin_theta * ab.a + cos_theta * ab.b;
    dq.d =  cos_theta * ab.a + sin_theta * ab.b;

    return dq;
}

/* Takes the dq from the park transform and a theta as radians,
 * and transforms back to the ab vector */
ab_vector inverse_park_transform(float theta_as_rad, dq_vector dq)
{
    // pre compute the sin and cos functions
    float sin_theta = sinf(theta_as_rad);
    float cos_theta = cosf(theta_as_rad);

    ab_vector ab;

    ab.a = cos_theta * dq.d + -sin_theta * dq.q;
    ab.b = sin_theta * dq.d + cos_theta * dq.q;

    return ab;
}
