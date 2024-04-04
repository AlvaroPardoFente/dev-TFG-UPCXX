ifeq ($(UPCXX_INSTALL),)
$(warning UPCXX_INSTALL environment variable is not set, assuming upcxx is in the PATH)
UPCXX=upcxx
UPCXXRUN=upcxx-run
else
ifeq ($(wildcard $(UPCXX_INSTALL)/bin/upcxx),)
$(error Please set UPCXX_INSTALL=/path/to/upcxx/install)
else
UPCXX=$(UPCXX_INSTALL)/bin/upcxx
UPCXXRUN=$(UPCXX_INSTALL)/bin/upcxx-run
endif
endif

UPCXX_THREADMODE ?= seq
export UPCXX_THREADMODE
UPCXX_CODEMODE ?= debug
export UPCXX_CODEMODE

# MPI compiler
MPICXX = mpicxx

# Compiler flags
CXXFLAGS = -std=c++17 -O3

# Source and build directories
SRC_DIR = ./benchmarks
BUILD_DIR = ./build

# Find all .cpp files in the source directory
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
# Get list of object files, with paths
# Get list of executable names
EXE_NAMES := $(basename $(notdir $(SOURCES)))

# Default rule
all: $(EXE_NAMES)

# Rule to compile a specific source file
%:
	@file=$$(find $(SRC_DIR) -name $@.cpp); \
	build_file=$(BUILD_DIR)/$$(dirname $$file | sed "s|$(SRC_DIR)/||")/$@; \
	mkdir -p $$(dirname $$build_file); \
	if echo $$file | xargs basename | grep -q "^upcxx"; then \
		$(UPCXX) $(CXXFLAGS) -o $$build_file $$file; \
	else \
		if echo $$file | xargs basename | grep -q "^mpi"; then \
			$(MPICXX) $(CXXFLAGS) -o $$build_file $$file; \
		fi; \
	fi

# Rule to clean the build directory
clean:
	rm -rf $(BUILD_DIR)