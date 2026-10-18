#include <math.h>
#include <stdio.h>
#include "motor_control.h"


const char* filename = "output.log";
/* A test program for the clark/park transform on a motor with 120deg phase
 * split in a 3 phase system. */

int main(void)
{
    FILE *fp = fopen(filename, "w");

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

        ab_vector ab = clarke_transform((three_phase){ .a = i_a, .b = i_b, .c = i_c });
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

    printf("Created %s with test results.\n", filename);


    /* Finally lets test the SVPWM.
     * PI controllers choose d/q voltages
     * -> inverse Park gives alpha/beta voltages
     * -> inverse Clarke gives A/B/C phase voltages
     * -> SVPWM converts those into achievable duties
     */

    float dc_bus = 24.f;
    float tolerance = 1e-6f; // because of ieee float inaccuracies
    /*
     * The three voltage commands come from inverse Clarke.
     * SVPWM shifts them together so they fit within the supply voltage.
     * Then each shifted voltage is divided by the bus voltage to get
     * the duty cycle for that phase.
     *
     * For this test the bus voltage is 12V:
     * A zero voltage command gives 50% duty on all three phases.
     * The command alpha=3, beta=0 gives duties of
     * 68.75% on A and 31.25% on B and C.
     */
    ab_vector test_voltages[] = { // selected from the output of the first test
        { .a =   0.0f,       .b =  10.0f       }, //   0 degrees
        { .a =  -7.0710678f, .b =   7.0710678f }, //  45 degrees
        { .a = -10.0f,       .b =   0.0f       }, //  90 degrees
        { .a =   0.0f,       .b = -10.0f       }, // 180 degrees
        { .a =  10.0f,       .b =   0.0f       }  // 270 degrees
    };


    // only chose 5, because it takes time to hand calculate expected value by
    // doing the inverse clarke, using the formula in the function, offset by
    // -(max(va, vb, vc) + min(va, vb, vc)) / 2 and each duty manually by doing
    // 0.5 + (x + offset) / 24.f. E.g for a = 0 and b = 10 with the bus on 24V:
    //
    // A = 0, B = sqrt(3)/2*10 = 8.660254, C is the same only negative
    // offset = 0 because min + max cancels out here.
    // duties is A = 0.5 + (0/24) = 0.5, B = 0.5 +(8.660254/24) = 0.8608439,
    // C = 0.5 + (-8.660254/24) = 0.1391561
    //
    three_phase expected[] = {
        { .a = 0.5000000f, .b = 0.8608439f, .c = 0.1391561f },
        { .a = 0.1514515f, .b = 0.8485485f, .c = 0.3382381f },
        { .a = 0.1875000f, .b = 0.8125000f, .c = 0.8125000f },
        { .a = 0.5000000f, .b = 0.1391561f, .c = 0.8608439f },
        { .a = 0.8125000f, .b = 0.1875000f, .c = 0.1875000f }
    };

    fprintf(fp, "\nSVPWM test, DC bus = %.1f V\n", dc_bus);
    fprintf(fp,
            " Alpha[V]   Beta[V] |       Duty A/B/C [%%]       |     Expected A/B/C [%%]     | Result\n");

    for (size_t i = 0; i < 5; ++i) {
        three_phase duties = svpwm(test_voltages[i], dc_bus);

        int passed =
            fabsf(duties.a - expected[i].a) < tolerance &&
            fabsf(duties.b - expected[i].b) < tolerance &&
            fabsf(duties.c - expected[i].c) < tolerance;

        fprintf(fp,
                "%9.4f %9.4f | %8.3f %8.3f %8.3f | %8.3f %8.3f %8.3f | %s\n",
                test_voltages[i].a, test_voltages[i].b,
                100.f * duties.a, 100.f * duties.b, 100.f * duties.c,
                100.f * expected[i].a,
                100.f * expected[i].b,
                100.f * expected[i].c,
                passed ? "PASS" : "FAIL");
    }

    fclose(fp);
    return 0;
}
