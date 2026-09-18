#include <math.h>
#include <stdio.h>
#include "algorithms.h"

/* A test program for the clark/park transform on a motor with 120deg phase
 * split in a 3 phase system. */

int main(void)
{

    float current_peak = 10.f; // in mA or uA, whatever
    //float current_offset = PI_F / 2.f; // gives a 90deg offset dq=0,10
    float current_offset = PI_F / 4.f; // gives a 45deg offset dq=7.071,7.071

    printf("angle      iA       iB       iC      sum       d       q\n");

    /* Looping over a full spin, 30 degrees at a time, checking the transforms
     * for each step. iA + iB + iC should = 0, and d and q should be constant
     * at 0 and 10
     */
    for (int degrees = 0; degrees < 360; degrees += 30) {
        float rotor_angle = (float)degrees * DEG_TO_RAD_F;

        /* the current vector is 90 degrees ahead of the rotor,
         * ideally d = 0 and q = current_peak.
         */
        float current_angle = rotor_angle + current_offset;

        float i_a = current_peak * cosf(current_angle);
        float i_b = current_peak * cosf(current_angle - PHASE_120_F);
        float i_c = current_peak * cosf(current_angle + PHASE_120_F);

        ab_vector ab = clarke_transform(i_a, i_b, i_c);
        dq_vector dq = park_transform(rotor_angle, ab);
        ab_vector reconstructed = inverse_park_transform(rotor_angle, dq);

        printf("%3d°  %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",
               degrees,
               i_a, i_b, i_c,
               i_a + i_b + i_c,
               dq.d, dq.q);
    }
    return 0;
}
