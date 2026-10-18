# NEORV32 for use of the common makefile
NEORV32_HOME = $(CURDIR)/neorv32

# Toolchain for RISCV
RISCV_PREFIX = riscv32-unknown-elf-
MARCH = rv32i_zicsr_zifencei
MABI = ilp32

# Actual source code
APP_SRC = $(wildcard src/*.c)
APP_INC = -I src -I algorithms

# Didnt see the uart output anywhere, and logs where empty. trying this
USER_FLAGS += -DUART0_SIM_MODE

# NEORV32 build system
include $(NEORV32_HOME)/sw/common/common.mk
