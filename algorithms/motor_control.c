#include "motor_control.h"
#include <math.h> // for sqrtf

#define INV_SQRT3 0.57735026919f

three_phase motor_control_update(motor_controller *controller,
            three_phase measured_currents, dq_vector target_idq,
            float rotor_angle, float dc_bus, float dt)
{
    ab_vector current_ab = clarke_transform(measured_currents);
    dq_vector current_dq = park_transform(rotor_angle, current_ab);

    // In case we need to disregard the changes later, due to limiting
    float previous_d_integral = controller->pi_d.integral;
    float previous_q_integral = controller->pi_q.integral;

    dq_vector volt_dq;
    volt_dq.d = pi_update( &controller->pi_d, target_idq.d, current_dq.d, dt);
    volt_dq.q = pi_update( &controller->pi_q, target_idq.q, current_dq.q, dt);

    // We need to limit the voltage vector to what the supply can produce, e.g.
    // if the supply is 24V the limit is ca. 13.86
    float limit = dc_bus * INV_SQRT3;
    float magnitude = sqrtf(volt_dq.d*volt_dq.d + volt_dq.q*volt_dq.q);

    if (magnitude > limit)
    {
        float scale = limit / magnitude;
        float limited_d = volt_dq.d * scale;
        float limited_q = volt_dq.q * scale;

        float change_d = controller->pi_d.integral - previous_d_integral;
        float change_q = controller->pi_q.integral - previous_q_integral;

        // Undo integral changes that is beyond the voltage limit
        if ((volt_dq.d - limited_d) * change_d > 0.f){
            controller->pi_d.integral = previous_d_integral;
        }

        if ((volt_dq.q - limited_q) * change_q > 0.f){
            controller->pi_q.integral = previous_q_integral;
        }

        volt_dq.d = limited_d;
        volt_dq.q = limited_q;
    }

    ab_vector volt_ab = inverse_park_transform(rotor_angle, volt_dq);
    three_phase duty_cycles = svpwm(volt_ab, dc_bus);

    return duty_cycles;
}
