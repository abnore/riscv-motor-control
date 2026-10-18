#include "motor_control.h"
/*
 * https://www.ecpe.org/infocentre/ecpe-online-course/dcac/space-vector-modulation/
 * https://www.mathworks.com/discovery/space-vector-modulation.html
 *
 * Space Vector Modulation is the industry standard and maximizes DC bus voltage
 * utilization, allowing a higher achievable output voltage for the same DC link
 * compared with sinusoidal pulse width modulation (SPWM).
 *
 * https://www.youtube.com/shorts/RWYun84ULWo
 *
 * Space vector PWM implementation, based on these sources:
 * https://software-dl.ti.com/processor-industrial-sw/esd/motor_control_sdk/am243x/2025_00_00_06/docs/api_guide_am243x/svgen_8h_source.html
 * https://imperix.com/doc/implementation/svpwm-vs-spwm-modulation-techniques
 * https://www.microchip.com/content/dam/mchp/documents/OTH/ApplicationNotes/ApplicationNotes/00955a.pdf
 *
 * Takes in the alpha/beta voltage commands in volts from the inverse park,and
 * the DC bus voltage, that has to be positive.
 *
 * This then centres the phase voltage within the available supply, before
 * converting it into PWM duty cycles.
 *
 * The output is three phase duty cycles between 0 and 1.
 */

static inline float max_float(float a, float b) { return a > b ? a : b; }
static inline float min_float(float a, float b) { return a < b ? a : b; }

three_phase svpwm(ab_vector voltage, float dc_bus)
{
    three_phase abc = inverse_clarke_transform(voltage);
    three_phase pwm;

    // Centering
    float max = max_float(abc.a, max_float(abc.b, abc.c));
    float min = min_float(abc.a, min_float(abc.b, abc.c));
    float offset = -0.5f * (max + min);

    // Voltage to PWM, this is why dc_bus *MUST* be greater than zero!
    float inverse_bus = 1.0f / dc_bus;

    pwm.a = 0.5f + (abc.a + offset) * inverse_bus;
    pwm.b = 0.5f + (abc.b + offset) * inverse_bus;
    pwm.c = 0.5f + (abc.c + offset) * inverse_bus;

    return pwm;
}
