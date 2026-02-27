CXX ?= g++

# path #
SRC_PATH = src
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# executable #
BIN_NAME = lilac

# extensions #
SRC_EXT = cpp

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)
#need to link keystone
# flags #
COMPILE_FLAGS = -std=c++23 -Wall -Wextra -g
INCLUDES = -I include/ -I /usr/local/include -I $(HOME)/local/include
# Space-separated pkg-config libraries used by this project
LIBS = -lkeystone #-L /usr/local/lib -L $(HOME)/local/lib

ASAN_OPTS = detect_leaks=1:strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)



# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	#@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) -o $@ ${LIBS}

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.SILENT: run
run: $(BIN_PATH)/$(BIN_NAME)
	make -s
	@echo "Running $(BIN_NAME):"
	./$(BIN_PATH)/$(BIN_NAME)

.SILENT: normal
normal:
	cmake --build cmake-build-debug --target lilac -j 10
	@echo "Running $(BIN_NAME) (normal):"
	ASAN_OPTIONS=$(ASAN_OPTS) ./cmake-build-debug/lilac data/function.llc -g -o output.out

.SILENT: run-normal
run-normal: normal
	@echo "Running output.out:"
	-./output.out; echo "Exit code: $$?"

.SILENT: compiles
compiles:
	cmake --build cmake-build-debug --target lilac -j 10
	@echo "Running $(BIN_NAME) (compiles):"
	ASAN_OPTIONS=$(ASAN_OPTS) ./cmake-build-debug/lilac data/compiles.llc -o output.out

.SILENT: run-compiles
run-compiles: compiles
	@echo "Running output.out:"
	-./output.out; echo "Exit code: $$?"

.SILENT: parse
parse:
	cmake --build cmake-build-debug --target lilac -j 10
	@echo "Running $(BIN_NAME) (parse):"
	ASAN_OPTIONS=$(ASAN_OPTS) ./cmake-build-debug/lilac data/function.llc -p


.PHONY: clean
clean::
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

SHAREDLIBNAME = lilac.so
EXENAME = lilac

include TESTUDO_CUSTOM_MAKEFILE