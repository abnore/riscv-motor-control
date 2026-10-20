#include <neorv32.h>

/* Super simple proof of concept and toolchain.
 * After make install and then make GHDL_RUN_FLAGS="--stop-time=200us" sim
 * we should see this lines printed */
int main(void)
{
    neorv32_uart0_setup(115200, 0);
    neorv32_uart0_puts("Hello from NEORV32!\n");

    while (1);
}
