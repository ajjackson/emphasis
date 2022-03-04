# Project Name
TARGET ?= emphasis

DEBUG = 1
OPT = -O0

# Library Locations
BLUEMCHEN_DIR ?= kxmx_bluemchen
LIBDAISY_DIR ?= $(BLUEMCHEN_DIR)/libDaisy
DAISYSP_DIR ?= $(BLUEMCHEN_DIR)/DaisySP

# Sources
CPP_SOURCES = $(BLUEMCHEN_DIR)/src/kxmx_bluemchen.cpp src/${TARGET}.cpp src/lib/cookbook-biquad.cpp
C_INCLUDES += -I$(BLUEMCHEN_DIR)/src -Isrc/lib

USE_FATFS = 1

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
