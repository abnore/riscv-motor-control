#include "motor_control.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

// simple helper to check if the duty cycle is within spec
static void check_duties(three_phase duty)
{
    assert(isfinite(duty.a) && duty.a >= 0.f && duty.a <= 1.f);
    assert(isfinite(duty.b) && duty.b >= 0.f && duty.b <= 1.f);
    assert(isfinite(duty.c) && duty.c >= 0.f && duty.c <= 1.f);
}

/* Recover the average phase voltages from the duties.
 * Subtracting their average removes the common voltage offset */
static three_phase recover_voltages(three_phase duty, float dc_bus)
{
    float average = (duty.a + duty.b + duty.c) / 3.f;

    return (three_phase) {
        .a = (duty.a - average) * dc_bus,
        .b = (duty.b - average) * dc_bus,
        .c = (duty.c - average) * dc_bus
    };
}

int main(void)
{
    const float dc_bus = 24.f;
    const float dt = 0.001f;
    const float tolerance = 1e-4f;

    motor_controller controller = {
        .pi_d = {
            .kp = 1.f,
            .ki = 100.f,
            .integral = 0.f,
            .error = 0.f,
            .out_min = -24.f,
            .out_max = 24.f
        },
        .pi_q = {
            .kp = 1.f,
            .ki = 100.f,
            .integral = 0.f,
            .error = 0.f,
            .out_min = -24.f,
            .out_max = 24.f
        }
    };

    three_phase measured = { .a = 0.f, .b = 0.f, .c = 0.f };
    dq_vector target = { .d = 0.f, .q = 0.f };

    // Zero current demand and zero measured current should give zero voltage
    three_phase duty = motor_control_update( &controller, measured, target,
                                              0.f, dc_bus, dt);

    check_duties(duty);
    assert(fabsf(duty.a - 0.5f) < tolerance);
    assert(fabsf(duty.b - 0.5f) < tolerance);
    assert(fabsf(duty.c - 0.5f) < tolerance);

    puts("PASS: zero demand gives 50% duty on all phases");

    // Both PI outputs individually fit within + or - 24V,
    // but together they exceed the voltage vector limit.
    target = (dq_vector) { .d = 20.f, .q = 20.f };

    for (int i = 0; i < 100; ++i) {
        duty = motor_control_update(
                &controller, measured, target, 0.f, dc_bus, dt);

        check_duties(duty);

        // Check what voltage the resulting duties actually represent
        three_phase voltage = recover_voltages(duty, dc_bus);

        // At rotor angle zero, alpha=d and beta=q
        float vd = voltage.a;
        float vq = (voltage.b - voltage.c) / sqrtf(3.f);

        // Equal d/q requests should remain equal after limiting.
        // Their combined magnitude should be 24/sqrt(3).
        float expected = dc_bus / sqrtf(6.f);

        assert(fabsf(vd - expected) < tolerance);
        assert(fabsf(vq - expected) < tolerance);

        // Neither integral should accumulate during this saturation
        assert(fabsf(controller.pi_d.integral) < tolerance);
        assert(fabsf(controller.pi_q.integral) < tolerance);
    }

    puts("PASS: voltage limiting preserves direction and limits magnitude");
    puts("PASS: integrals stay at zero during repeated saturation");

    // Here we need to reduce the demand so integration is allowed again
    target = (dq_vector) { .d = 1.f, .q = 0.f };

    duty = motor_control_update(&controller, measured, target, 0.f, dc_bus, dt);

    check_duties(duty);

    assert(fabsf(controller.pi_d.integral - 1.05f) < tolerance);
    assert(fabsf(controller.pi_q.integral - 1.00f) < tolerance);

    puts("PASS: integration resumes when demand falls");
    return 0;
}
