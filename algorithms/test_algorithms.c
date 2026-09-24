#include <math.h>
#include <stdio.h>
#include "motor_control.h"


/* A test program for the clark/park transform on a motor with 120deg phase
 * split in a 3 phase system. */

int main(void)
{
    FILE *fp = fopen("output.log", "w");

    float current_peak = 10.f; // in mA or uA, whatever
    float current_offset = PI_F / 2.f; // gives a 90deg offset dq=0,10
    // float current_offset = PI_F / 4.f; // gives a 45deg offset dq=7.071,7.071

    fprintf(fp,"angle      iA       iB       iC       a       b        sum       d       q     |    a'       b'\n");

    /* Looping over a full spin, 30 degrees at a time, checking the transforms
     * for each step. iA + iB + iC should = 0, and d and q should be constant
     * at 0 and 10
     */
    for (int degrees = 0; degrees < 360; degrees += 15) {
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

        fprintf(fp,"%3d°  %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f  |%8.3f %8.3f\n",
               degrees,
               i_a, i_b, i_c,
               ab.a, ab.b,
               i_a + i_b + i_c,
               dq.d, dq.q,
               reconstructed.a, reconstructed.b);
    }
    /* This test prints out the following:

       angle      iA       iB       iC       a       b        sum       d       q     |    a'       b'
         0°    -0.000    8.660   -8.660   -0.000   10.000    0.000   -0.000   10.000  |  -0.000   10.000
        15°    -2.588    9.659   -7.071   -2.588    9.659    0.000   -0.000   10.000  |  -2.588    9.659
        30°    -5.000   10.000   -5.000   -5.000    8.660    0.000   -0.000   10.000  |  -5.000    8.660
        45°    -7.071    9.659   -2.588   -7.071    7.071    0.000   -0.000   10.000  |  -7.071    7.071
        60°    -8.660    8.660    0.000   -8.660    5.000    0.000    0.000   10.000  |  -8.660    5.000
        75°    -9.659    7.071    2.588   -9.659    2.588    0.000    0.000   10.000  |  -9.659    2.588
        90°   -10.000    5.000    5.000  -10.000    0.000   -0.000    0.000   10.000  | -10.000    0.000
       105°    -9.659    2.588    7.071   -9.659   -2.588    0.000   -0.000   10.000  |  -9.659   -2.588
       120°    -8.660   -0.000    8.660   -8.660   -5.000    0.000   -0.000   10.000  |  -8.660   -5.000
       135°    -7.071   -2.588    9.659   -7.071   -7.071    0.000   -0.000   10.000  |  -7.071   -7.071
       150°    -5.000   -5.000   10.000   -5.000   -8.660    0.000   -0.000   10.000  |  -5.000   -8.660
       165°    -2.588   -7.071    9.659   -2.588   -9.659    0.000   -0.000   10.000  |  -2.588   -9.659
       180°     0.000   -8.660    8.660    0.000  -10.000    0.000    0.000   10.000  |   0.000  -10.000
       195°     2.588   -9.659    7.071    2.588   -9.659    0.000    0.000   10.000  |   2.588   -9.659
       210°     5.000  -10.000    5.000    5.000   -8.660   -0.000    0.000   10.000  |   5.000   -8.660
       225°     7.071   -9.659    2.588    7.071   -7.071   -0.000    0.000   10.000  |   7.071   -7.071
       240°     8.660   -8.660   -0.000    8.660   -5.000   -0.000   -0.000   10.000  |   8.660   -5.000
       255°     9.659   -7.071   -2.588    9.659   -2.588   -0.000   -0.000   10.000  |   9.659   -2.588
       270°    10.000   -5.000   -5.000   10.000    0.000   -0.000   -0.000   10.000  |  10.000    0.000
       285°     9.659   -2.588   -7.071    9.659    2.588    0.000    0.000   10.000  |   9.659    2.588
       300°     8.660    0.000   -8.660    8.660    5.000   -0.000   -0.000   10.000  |   8.660    5.000
       315°     7.071    2.588   -9.659    7.071    7.071   -0.000   -0.000   10.000  |   7.071    7.071
       330°     5.000    5.000  -10.000    5.000    8.660   -0.000   -0.000   10.000  |   5.000    8.660
       345°     2.588    7.071   -9.659    2.588    9.659   -0.000   -0.000   10.000  |   2.588    9.659
    */

    // Now let us test the PI update function
    const float resistance = 1.0f;    // ohms
    const float inductance = 0.01f;   // henries
    const float dt = 0.0001f;         // 100 microseconds

    float current = 0.0f;

    PI_control pi = {
        .kp = 1.0f,
        .ki = 100.0f,
        .integral = 0.0f,
        .error = 0.0f,
        .out_min = -5.0f,            // volts
        .out_max = 5.0f
    };


    fprintf(fp,"\ntime,target,current,voltage,integral\n");
    for (int step = 0; step < 2000; ++step) {
        float target = (step < 1000) ? 10.0f : 2.0f;

        float voltage = pi_update(&pi, target, current, dt);

        current += (voltage - resistance * current) * dt / inductance;

        fprintf(fp, "%.4f,%.4f,%.4f,%.4f,%.4f\n",
                (step + 1) * dt, target, current, voltage, pi.integral);
    }

    fclose(fp);
    return 0;
}
