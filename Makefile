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
UPCXX_CODEMODE ?= opt
export UPCXX_CODEMODE

# MPI compiler
MPICXX = mpicxx

# Default compiler
CXX = upcxx

# Set different compilers for targets with specific prefixes
upcxx%: CXX = $(UPCXX)
mpi%: CXX = $(MPICXX)

# Source and build directories
SRC_DIR = ./benchmarks
INC_DIR = ./include
LIB_DIR = ./lib
BUILD_DIR = ./build
OBJ_DIR = ./obj

# Optimization flags
OPT = -O3

# Dependency flags
DEPFLAGS= -MP -MMD -MF $(OBJ_DIR)/$*.Td

# Compiler flags
CXXFLAGS = -std=c++17 -I$(INC_DIR)
ifeq ($(UPCXX_CODEMODE),debug)
CXXFLAGS += -g
else
CXXFLAGS += $(OPT)
endif
CXXFLAGS += $(DEPFLAGS)

# Find all .cpp files in the source directory
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Find all .cpp files in the lib directory
LIB_SOURCES := $(wildcard $(LIB_DIR)/*.cpp)

# Get list of lib object files
LIB_OBJECTS = $(addprefix $(OBJ_DIR)/, $(notdir $(LIB_SOURCES:.cpp=.o)))

# Filter out the object files that start with upcxx
MPI_LIB_OBJECTS = $(filter-out $(OBJ_DIR)/upcxx%, $(LIB_OBJECTS))
UPCXX_LIB_OBJECTS = $(filter-out $(OBJ_DIR)/mpi%, $(LIB_OBJECTS))

upcxx%: LINK_OBJECTS = $(UPCXX_LIB_OBJECTS)
mpi%: LINK_OBJECTS = $(MPI_LIB_OBJECTS)

# Get list of executable names
EXE_NAMES := $(basename $(notdir $(SOURCES)))

# Default rule
all: $(EXE_NAMES)

# Rule to compile a lib source file into an object file
$(LIB_OBJECTS): $(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR); \
	$(CXX) $(CXXFLAGS) -c $< -o $@; \
	mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d && touch $@

# Rule to compile a specific source file
$(EXE_NAMES): %: $(LIB_OBJECTS)
	@target=$@; \
	file=$$(find $(SRC_DIR) -name $$target.cpp); \
	build_file=$(BUILD_DIR)/$$(dirname $$file | sed "s|$(SRC_DIR)/||")/$@; \
	mkdir -p $$(dirname $$build_file); \
	$(CXX) $(CXXFLAGS) -o $$build_file $$file $(LINK_OBJECTS); \
	mv -f $(OBJ_DIR)/$$target.Td $(OBJ_DIR)/$$target.d && touch $$build_file;

# Rule to clean the build and object directories
clean:
	rm -rf $(BUILD_DIR) $(OBJ_DIR)

# Include the dependencies
-include $(patsubst %,$(OBJ_DIR)/%.d,$(basename $(EXE_NAMES)))

.PHONY: all clean

.PRECIOUS: $(OBJ_DIR)/%.o