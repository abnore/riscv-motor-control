library ieee;
use ieee.std_logic_1164.all;

library neorv32;

entity motor_control is
    port(
        clk     : in  std_logic;
        rst     : in  std_logic
    );
end entity;

architecture rtl of motor_control is
    signal rstn : std_logic;
    signal gpio_out : std_ulogic_vector(31 downto 0);

attribute mark_debug : string;
attribute mark_debug of gpio_out : signal is "true";

begin

    rstn <= not rst;

cpu_inst : entity neorv32.neorv32_top
    generic map (
        CLOCK_FREQUENCY => 100_000_000,
        BOOT_MODE_SELECT => 2,
        RISCV_ISA_M => true,
        RISCV_ISA_C => true,
        RISCV_ISA_Zicntr => true,
        IMEM_EN => true,
        IMEM_SIZE => 32 * 1024,
        DMEM_EN => true,
        DMEM_SIZE => 16 * 1024,
        IO_GPIO_NUM => 32,
        -- CPU execution
        CPU_FAST_MUL_EN   => true,
        CPU_FAST_SHIFT_EN => true,
        CPU_FAST_MUL_REGS => 1, 

        -- Memory timing
        IMEM_OUTREG_EN => false,
        DMEM_OUTREG_EN => true
    )
    port map (
        clk_i       => clk,
        rstn_i      => rstn,
       
        -- have to list out all outputs to set it open
        uart0_txd_o => open,
        rstn_ocd_o => open,
        rstn_wdt_o => open,
        trace_cpu0_o => open,
        trace_cpu1_o => open,
        jtag_tdo_o => open,
        smc_ioen_o => open,
        smc_sck_o => open,
        smc_csn_o => open,
        smc_sdo_o => open,
        xbus_adr_o => open,
        xbus_dat_o => open,
        xbus_cti_o => open,
        xbus_tag_o => open,
        xbus_we_o => open,
        xbus_sel_o => open,
        xbus_stb_o => open,
        xbus_cyc_o => open,
        slink_rx_rdy_o => open,
        slink_tx_dat_o => open,
        slink_tx_dst_o => open,
        slink_tx_val_o => open,
        slink_tx_lst_o => open,
        gpio_dir_o => open,
        gpio_o => gpio_out,
        uart0_rtsn_o => open,
        uart1_txd_o => open,
        uart1_rtsn_o => open,
        spi_clk_o => open,
        spi_dat_o => open,
        spi_csn_o => open,
        sdi_dat_o => open,
        twi_sda_o => open,
        twi_scl_o => open,
        twd_sda_o => open,
        onewire_o => open,
        pwm_o => open,
        cfs_out_o => open,
        neoled_o => open,
        mtime_time_o => open
    );

end architecture;
