# ============================================================
# Makefile for IndoEng Dictionary (Indonesian-English)
# ============================================================

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2 -Wno-stringop-truncation -Wno-format-truncation
# -Wall -Wextra: Enable all warnings
# -std=c99: Use C99 standard
# -pedantic: Strict ISO C compliance
# -O2: Optimization level 2
# -Wno-stringop-truncation: Disable truncation warnings for strncpy
# -Wno-format-truncation: Disable truncation warnings for snprintf

# Target executable
TARGET = indoeng-dictionary

# Source files
SRC_DIR = src
SRC_FILES = $(SRC_DIR)/main.c \
            $(SRC_DIR)/dictionary.c \
            $(SRC_DIR)/data_structures.c \
            $(SRC_DIR)/word_data.c \
            $(SRC_DIR)/ui_utils.c

# Header files for dependency checking
HEADERS = $(SRC_DIR)/dictionary.h \
          $(SRC_DIR)/data_structures.h \
          $(SRC_DIR)/ui_utils.h

# Directories
BUILD_DIR = build
BIN_DIR = bin

# ============================================================
# Build Targets
# ============================================================

.PHONY: all clean run test help info

# Default target: build everything
all: $(BUILD_DIR) $(BIN_DIR)
	@echo ""
	@echo "Building $(TARGET)..."
	@echo "========================================"
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(TARGET) $(SRC_FILES)
	@echo ""
	@echo "Build successful!"
	@echo "Executable: $(BIN_DIR)/$(TARGET)"
	@echo "========================================"

# Create directories if they don't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Run the program
run: all
	@echo ""
	@echo "Running $(TARGET)..."
	@echo "========================================"
	@$(BIN_DIR)/$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete!"

# Show help
help:
	@echo ""
	@echo "=========================================="
	@echo "   IndoEng Dictionary - Makefile Help    "
	@echo "=========================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  run     - Build and run the program"
	@echo "  clean   - Remove build artifacts"
	@echo "  test    - Run with test input"
	@echo "  info    - Show compiler and system info"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build the project"
	@echo "  make run      - Build and run"
	@echo "  make clean    - Clean build files"
	@echo ""
	@echo "=========================================="

# Show project info
info:
	@echo ""
	@echo "=========================================="
	@echo "       IndoEng Dictionary Info          "
	@echo "=========================================="
	@echo ""
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo ""
	@echo "Source files:"
	@for f in $(SRC_FILES); do echo "  - $$f"; done
	@echo ""
	@echo "Header files:"
	@for h in $(HEADERS); do echo "  - $$h"; done
	@echo ""
	@echo "Output: $(BIN_DIR)/$(TARGET)"
	@echo "=========================================="

# Test run (with piped input)
test: all
	@echo "5" | $(BIN_DIR)/$(TARGET)

# ============================================================
# Dependencies
# ============================================================

# Automatic dependency generation (optional - uncomment if needed)
# -MMD -MP flags create .d dependency files
# $(OBJ_FILES): %.o: %.c
# 	$(CC) $(CFLAGS) -MMD -o $@ $<
# -include $(OBJ_FILES:.o=.d)