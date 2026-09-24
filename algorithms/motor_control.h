#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H

#include <math.h>
// math.h defines PI as a double, i will keep everything 32 bit
#ifndef M_PI
    #define PI_F 3.14159265358979323846f
#else
    #define PI_F ((float)M_PI)
#endif
/* Due to this error on linux, i have the full PI_F declared like this
   motor_control.h:7:30: error: ‘M_PI’ undeclared (first use in this function)
   7 | #define PI_F         ((float)M_PI)
*/

#define DEG_TO_RAD_F (PI_F / 180.f)
#define PHASE_120_F  (2.f * PI_F / 3.f)

/* Struct for holding the alpha/beta vectors that clarke transform creates
* from the 3 phases rotatin domain into the 2 phase rotating domain.
* Also inverse park transform creates this. */
typedef struct {
    float a;
    float b;
} ab_vector;

/* Struct for holding the D and Q vectors in the stationary domain that is
* created by the park transform */
typedef struct {
    float d;
    float q;
} dq_vector;

ab_vector clarke_transform(float i_a, float i_b, float i_c);
dq_vector park_transform(float theta_rad, ab_vector ab);
ab_vector inverse_park_transform(float theta_rad, dq_vector dq);

/* Struct for a PI controller, used for both the D and Q vectors */
typedef struct {
    float kp; // proportional gain
    float ki; // integral gain
    float integral; // saved integral sum
    float error; // saving error terms
    float out_min; // windup min
    float out_max; // windup max
} PI_control;

// PI controller function, will output a voltage
float pi_update(PI_control *pi, float target, float measured, float dt);

#endif //MOTOR_CTRL_H
