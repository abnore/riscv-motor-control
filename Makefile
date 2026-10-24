# Overwriting, to tell the common makefile where my neorv32 folder is
NEORV32_HOME = $(CURDIR)/neorv32

# My toolchain for RISCV, overwriting none and specifying 32 bit
RISCV_PREFIX = riscv32-unknown-elf-

# Actual source code
ALGORITHM_SRC := $(filter-out algorithms/test_%.c,$(wildcard algorithms/*.c))
APP_SRC = $(wildcard src/*.c) $(ALGORITHM_SRC)
APP_INC = -I src -I algorithms

# -------------------------------------
# Didnt see the uart output anywhere, and logs where empty. trying this
# This section is sim mode. Comment the section out when building for hardware
USER_FLAGS += -DUART0_SIM_MODE
# -------------------------------------

# Match the simulations 32 kib instruction memory
USER_FLAGS += -Wl,--defsym=__neorv32_rom_size=32768

# Disable caches to match the measured ZedBoard baseline, this only runs in sim
GHDL_RUN_FLAGS ?= -gICACHE_EN=false -gDCACHE_EN=false --stop-time=1ms

# Trying to omptimize for speed
EFFORT = -O3

# NEORV32 build system
include $(NEORV32_HOME)/sw/common/common.mk
