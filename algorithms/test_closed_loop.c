#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "motor_control.h"

/* Closed loop test where I will test whether the controller actually regulates
 * the current, and I will simulate an electrical load.
 *
 * First i will pass a simulated phase current to the function m.ctrl.update
 * this will do its thing and convert its pwm duties back into average phase
 * voltages. Then update the simulated currents using those voltages. And I
 * will repeat with new currents.
 *
 * I will use the RL equation from earlier tests:
 *      current += (voltage - resistance * current) * dt / inductance
 *
 * First start with a fixed rotor angle of zero
 * */
int main(void)
{
    const float dc_bus = 24.f;
    const float resistance = 1.f;    // ohms per phase
    const float inductance = 0.01f;  // henries per phase
    const float dt = 0.0001f;        // 100 microseconds
    const float rotor_angle = 0.f;  // fixed electrical angle in radians

    PI_control initial_pi = {
        .kp = 1.f,
        .ki = 100.f,
        .integral = 0.f,
        .error = 0.f,
        .out_min = -24.f,
        .out_max = 24.f
    };

    motor_controller controller = {
        .pi_d = initial_pi,
        .pi_q = initial_pi
    };

    three_phase currents = { .a = 0.f, .b = 0.f, .c = 0.f };
    dq_vector measured_idq = { .d = 0.f, .q = 0.f };

    FILE *fp = fopen("closed_loop.log", "w");
    fprintf(fp, "time,target_d,measured_d,target_q,measured_q,"
            "voltage_d,voltage_q,integral_d,integral_q\n");

    for (int step = 0; step < 4000; ++step) {
        dq_vector target_idq = {
            .d = 0.f,
            .q = (step >= 1000 && step < 2000) ? 20.f : 2.f
        };

        three_phase duties = motor_control_update(
            &controller, currents, target_idq,
            rotor_angle, dc_bus, dt);

        assert(isfinite(duties.a));
        assert(isfinite(duties.b));
        assert(isfinite(duties.c));
/*
    PASS: current reaches the initial 2 A target
    Assertion failed: (duties.c >= 0.f && duties.c <= 1.f),
    function main, file test_closed_loop.c, line 64.
    Abort trap: 6              ./test_closed_loop

    I added this temp print to see what the failure was:

        if (duties.a < 0.f || duties.a > 1.f ||
            duties.b < 0.f || duties.b > 1.f ||
            duties.c < 0.f || duties.c > 1.f)
        {
            fprintf(stderr, "step=%d duties: a=%.9g b=%.9g c=%.9g\n",
            step, duties.a, duties.b, duties.c);
        }

    step=1000 duties: a=0.5 b=1 c=-1.49011612e-08

    Due to a tiny rounding error we have to add a slight tolerance in the system,
    as with the other test. We need to be less strict, so i replaced this code,
    with what is currently in play:

        assert(duties.a >= 0.f && duties.a <= 1.f);
        assert(duties.b >= 0.f && duties.b <= 1.f);
        assert(duties.c >= 0.f && duties.c <= 1.f);

*/
        const float tolerance = 1e-6f;

        assert(duties.a >= -tolerance && duties.a <= 1.f + tolerance);
        assert(duties.b >= -tolerance && duties.b <= 1.f + tolerance);
        assert(duties.c >= -tolerance && duties.c <= 1.f + tolerance);

        // Duty times bus voltage gives the average terminal voltage.
        // Subtract the average of all three to get the winding voltages.
        float average_duty = (duties.a + duties.b + duties.c) / 3.f;

        three_phase voltages = {
            .a = (duties.a - average_duty) * dc_bus,
            .b = (duties.b - average_duty) * dc_bus,
            .c = (duties.c - average_duty) * dc_bus
        };

        dq_vector voltage_dq = park_transform(
            rotor_angle, clarke_transform(voltages));

        // The applied voltage vector must stay within the bus limit
        float magnitude = sqrtf(
            voltage_dq.d * voltage_dq.d +
            voltage_dq.q * voltage_dq.q);

        assert(magnitude <= dc_bus / sqrtf(3.f) + 1e-4f);

        // Simulate current changes in each winding:
        // L * di/dt = voltage - R * current
        currents.a += (voltages.a - resistance * currents.a) * dt / inductance;
        currents.b += (voltages.b - resistance * currents.b) * dt / inductance;
        currents.c += (voltages.c - resistance * currents.c) * dt / inductance;

        measured_idq = park_transform(rotor_angle, clarke_transform(currents));

        assert(isfinite(measured_idq.d));
        assert(isfinite(measured_idq.q));

        // The three-wire load should have approximately zero current sum
        assert(fabsf(currents.a + currents.b + currents.c) < 1e-3f);

        fprintf(fp, "%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                (step + 1) * dt,
                target_idq.d, measured_idq.d,
                target_idq.q, measured_idq.q,
                voltage_dq.d, voltage_dq.q,
                controller.pi_d.integral, controller.pi_q.integral);

        // Check tracking before the saturation test begins
        if (step == 999) {
            assert(fabsf(measured_idq.d) < 0.02f);
            assert(fabsf(measured_idq.q - 2.f) < 0.02f);
            puts("PASS: current reaches the initial 2 A target");
        }

        // At 1 ohm, the q current approaches the limited q voltage.
        // The 20 A target cannot be reached with this voltage limit.
        if (step == 1999) {
            assert(fabsf(measured_idq.d) < 0.02f);
            assert(fabsf(measured_idq.q - dc_bus / sqrtf(3.f)) < 0.05f);
            puts("PASS: current reaches the voltage-limited level");
        }
    }

    fclose(fp);

    assert(fabsf(measured_idq.d) < 0.02f);
    assert(fabsf(measured_idq.q - 2.f) < 0.02f);

    puts("PASS: current returns to 2 A after saturation");
    puts("Created closed_loop.log");

    return 0;
}
