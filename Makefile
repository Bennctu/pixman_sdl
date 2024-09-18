# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
CFLAGS += -fsanitize=address

# Libraries and dependencies
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 pixman-1)
SDL_LIBS := $(shell pkg-config --libs sdl2 pixman-1)

# Directories
SRCDIR = src
INCDIR = include
TESTDIR = test
BUILDDIR = build

# Source files
SRC_FILES = $(SRCDIR)/sdl_manager.c
TEST_FILES = $(TESTDIR)/composite_test.c

# Object files
OBJ_FILES = $(SRC_FILES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TEST_OBJ_FILES = $(TEST_FILES:$(TESTDIR)/%.c=$(BUILDDIR)/%.o)

# Output
EXEC = composite_test

# Rules
.PHONY: all clean

all: $(EXEC)

# Build the final executable
$(EXEC): $(OBJ_FILES) $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(TEST_OBJ_FILES) $(SDL_LIBS) -o $(EXEC)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Clean the build directory and executable
clean:
	rm -rf $(BUILDDIR) $(EXEC)

