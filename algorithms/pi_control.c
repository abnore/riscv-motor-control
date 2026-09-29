#include "motor_control.h"
/*  The PI-section from the Digital Motor Control Software Library
 * https://dev.ti.com/tirex/explore/node?isTheia=false&node=A__ANIiXA6VgJbL29tUf0TOyw__motor_control_c2000ware_sdk_software_package__0.jXikd__LATEST
 * https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/171/PI-section-from-DMC-MATH_5F00_v13.1.pdf
 * Theory and tips inspired this implementation which is my own
 *
 * The update loop will run based on pre configures kp and ki values, this will
 * be tuned outside of this function
 *
 * each controller will regulate one current component after park, d or q.
 * The measured will be the actual d/q currents gotten through Clarke and Park.
 * Each controller will then outputs voltage
 *
 * From the TI library:
 *  Tuning the P+I controller
 *  A suggested general technique for tuning the controller is now described.
 *
 *      Step 1. Ensure integral is set to zero and proportional gain set to
 *              one.
 *      Step 2. Gradually adjust proportional gain variable (Kp) while
 *              observing the step response to achieve optimum rise time and
 *              overshoot compromise.
 *      Step 3. If necessary, gradually increase integral gain (Ki) to optimize
 *              the return of the steady state output to nominal. The
 *              controller will be very sensitive to this term and may become
 *              unstable so be sure to start with a very small number. Integral
 *              gain will result in an increase in overshoot and oscillation,
 *              so it may be necessary to slightly decrease the Kp term again
 *              to find the best balance. Note that if the integral gain is
 *              used then set to zero, a small residual term may persist in ui
 *
 */
float pi_update(PI_control *PI, float target, float measured, float dt)
{
    // Calculate the error and change
    float previous_error = PI->error;
    PI->error = target - measured;
    float average_error = 0.5f * (previous_error + PI->error);
    float change = PI->ki * average_error * dt;

    // Calculate a desired integral and the resulting output, and check it
    float desired = PI->integral + change;
    float proportional = PI->kp * PI->error;
    float output = proportional + desired;

    // Now we use desired unless it pushes further beyond out limit, if so we
    // use the last value for integral!
    // Windup happens when the output reaches its limit but the integral keeps
    // growing. This will prevent that extra accumulation
    if (!( (output > PI->out_max && change > 0.0f) ||
           (output < PI->out_min && change < 0.0f)))
    {
        PI->integral = desired;
    }

    output = proportional + PI->integral;

    /// Clamp the output to out_min and out_max.
    if (output > PI->out_max)       output = PI->out_max;
    else if (output < PI->out_min)  output = PI->out_min;

    return output;
}
