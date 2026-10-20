#include "motor_control.h"
#include <neorv32.h>
#include <stdint.h>

int main(void)
{
    // We need a way to print in sim mode, while we use the ILA on the gpios
    // for the real bitstream and debugging
    #ifdef UART0_SIM_MODE
    neorv32_uart0_setup(115200, 0);
    neorv32_uart0_puts("NEORV32 motor control\n");
    #endif

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

    three_phase measured_currents = {
        .a = 0.f,
        .b = 0.f,
        .c = 0.f
    };

    dq_vector target_idq = {
        .d = 0.f,
        .q = 2.f
    };

    // Enable cycle counting (clear the cy inhibit bit)
    neorv32_cpu_csr_clr(CSR_MCOUNTINHIBIT, 1u);

    volatile three_phase result;
    uint32_t start = neorv32_cpu_csr_read(CSR_MCYCLE);

    result = motor_control_update(
            &controller,
            measured_currents,
            target_idq,
            0.f,
            24.f,
            0.0001f);

    uint32_t end = neorv32_cpu_csr_read(CSR_MCYCLE);
    uint32_t cycles = end - start;

    #ifdef UART0_SIM_MODE
    neorv32_uart0_printf("It took: %u cycles\n", (unsigned int)cycles);

    /* Print duties as integers in thousandths.
     * For example, 573 means a duty of 0.573, or 57.3%.
     */
    neorv32_uart0_printf(
        "Duties / 1000: A=%u B=%u C=%u\n",
        (unsigned int)(result.a * 1000.f + 0.5f),
        (unsigned int)(result.b * 1000.f + 0.5f),
        (unsigned int)(result.c * 1000.f + 0.5f));

    neorv32_uart0_puts("Control update complete\n");

    #else
    // Make the cycle count visible to Vivado ILA on GPIO[31:0].
    neorv32_gpio_port_set(cycles);
    #endif

    while (1) {
        __asm__ volatile ("nop");
    }
}


