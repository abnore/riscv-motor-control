#include <stdio.h>
#include <math.h>
#include "common.h"

// math.h defines PI as a double, i will keep everything 32 bit
#define DEG_TO_RAD_F ((float)(M_PI / 180.0))

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

int main(void)
{
    ab_vector ab = { .a = 0.0f, .b = 1.54701f };
    float radians = 90 * DEG_TO_RAD_F;
    dq_vector dq = park_transform(radians, ab);

    printf("after the park transform:\ndq.q = %f\ndq.d = %f\n", dq.q, dq.d);

    /* With 45 we should get both 1.093901 and we do, with 90 we should get
     * 0 and 1.547010, and we do*/
    return 0;
}
