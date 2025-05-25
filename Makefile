# === CONFIG ===

# Detect platform
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    SYS_LIBS := #-lGL -ldl -lm -pthread 
    PLATFORM := LINUX
else ifeq ($(OS),Windows_NT)
    SYS_LIBS := #-lopengl32 -lgdi32 -luser32 #-lshell32 -lkernel32 
    PLATFORM := WINDOWS
endif

# Compiler
CC := gcc
CXX := gcc
CFLAGS := -Wall -Werror -Wextra -O3
CXXFLAGS := -Wall -Werror -Wextra -O3
PROJECT_FLAGS    :=
PROJECT_CXXFLAGS := 

INCLUDES := -Isrc -Ivendor -Iinclude
LIBS := 

# Linker flags
LDFLAGS := $(LIBS) $(SYS_LIBS)

# Project structure
SRC_DIR := src
INCLUDE_DIR := include
VENDOR_DIR := vendor
BUILD_DIR := build

# Source files (your code + glad/glfw if using source)
C_SRC   := 	src/main.c \
			src/todo.c \
			src/stream.c \
			src/todo_cmp.c \
			src/todo_cmd.c \
			src/todo_api.c \
			src/todo_help.c \
			src/todo_args.c \
			src/todo_error.c \
			src/todoio.c \
			src/todo_get_alg.c \
			src/todo_hash.c \
			src/todo_version.c \
			src/bitwise.c \
			src/stream_reader.c \
			src/stream_writer.c \
			src/stream_sort.c \
			src/todo_sort/todo_sort.c \
			src/todo_sort/sort_bytes.c \
			src/todo_sort/sort_title.c \
			src/todo_sort/sort_uint32.c \
			include/gc_string.c \
			include/gc_tokens.c \
			include/gc_speed_test.c \
			include/gc_bsearch.c
CXX_SRC := 	

# Object files for debug and release
C_OBJ_DEBUG := $(C_SRC:%.c=$(BUILD_DIR)/debug/%.o)
CXX_OBJ_DEBUG := $(CXX_SRC:%.cpp=$(BUILD_DIR)/debug/%.o)
C_OBJ_RELEASE := $(C_SRC:%.c=$(BUILD_DIR)/release/%.o)
CXX_OBJ_RELEASE := $(CXX_SRC:%.cpp=$(BUILD_DIR)/release/%.o)

# Final binary names for debug and release
TARGET_DEBUG := $(BUILD_DIR)/debug
TARGET_RELEASE := $(BUILD_DIR)/todo

# Debug flags
DEBUG_FLAGS := -DGC_DEBUG

# === RULES ===
debug: $(TARGET_DEBUG)
release: $(TARGET_RELEASE)
all: $(TARGET_DEBUG) $(TARGET_RELEASE)



# Debug build
$(TARGET_DEBUG): $(C_OBJ_DEBUG) $(CXX_OBJ_DEBUG)
	@mkdir -p $(BUILD_DIR)/debug
	$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS) $(DEBUG_FLAGS) 
	@echo "Debug build complete: $(TARGET_DEBUG)"

# Release build
$(TARGET_RELEASE): $(C_OBJ_RELEASE) $(CXX_OBJ_RELEASE)
	@mkdir -p $(BUILD_DIR)/release
	$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "Release build complete: $(TARGET_RELEASE)"

# Compile C files with gcc (debug version)
$(BUILD_DIR)/debug/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(PROJECT_FLAGS) $(DEBUG_FLAGS)
	@touch $@

# Compile C++ files with g++ (debug version)
$(BUILD_DIR)/debug/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ $(PROJECT_FLAGS) $(DEBUG_FLAGS) $(PROJECT_CXXFLAGS) 
	@touch $@

# Compile C files with gcc (release version)
$(BUILD_DIR)/release/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(PROJECT_FLAGS)
	@touch $@

# Compile C++ files with g++ (release version)
$(BUILD_DIR)/release/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ $(PROJECT_FLAGS) $(PROJECT_CXXFLAGS)
	@touch $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
