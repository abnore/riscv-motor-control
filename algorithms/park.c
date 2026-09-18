#include <stdio.h>
#include <math.h>
#include "common.h"

dq_vector park_transform(float theta, ab_vector ab)
{
    dq_vector dq;

    // sinf and cosf expect radians
    float radians = theta * M_PI / 180.0f;
    dq.q = -sinf(radians)*ab.a + cosf(radians)*ab.b;
    dq.d = cosf(radians)*ab.a + sinf(radians) * ab.b;

    return dq;
}

int main(void)
{
    ab_vector ab = { .a = 0, .b = 1.54701f };
    dq_vector dq = park_transform(90, ab);

    printf("after the park transform:\ndq.q = %f\ndq.d = %f\n", dq.q, dq.d);

    /* With 45 we should get both 1.040317 and we do, with 90 we should get
     * 0 and 1.547010, and we do*/
    return 0;
}
