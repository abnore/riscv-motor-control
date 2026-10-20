# NEORV32 for use of the common makefile
NEORV32_HOME = $(CURDIR)/neorv32

# Toolchain for RISCV
RISCV_PREFIX = riscv32-unknown-elf-
MARCH = rv32i_zicsr_zifencei
MABI = ilp32

# Actual source code
ALGORITHM_SRC := $(filter-out algorithms/test_%.c,$(wildcard algorithms/*.c))
APP_SRC = $(wildcard src/*.c) $(ALGORITHM_SRC)
APP_INC = -I src -I algorithms

# Didnt see the uart output anywhere, and logs where empty. trying this
#USER_FLAGS += -DUART0_SIM_MODE
# Match the simulation's 32 KiB instruction memory
USER_FLAGS += -Wl,--defsym=__neorv32_rom_size=32768

# Trying to omptimize for speed
EFFORT = -O3
# NEORV32 build system
include $(NEORV32_HOME)/sw/common/common.mk
