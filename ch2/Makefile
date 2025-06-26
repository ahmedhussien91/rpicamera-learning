# Makefile for Bayer to RGB Converter

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = bayer_converter
SOURCE = understand-bayerRawImage.cpp

# Default target
all: $(TARGET)

# Compile the application
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Clean compiled files
clean:
	rm -f $(TARGET) *.ppm

# Test run with sample parameters (requires input file)
test: $(TARGET)
	@echo "Testing Bayer converter..."
	@echo "Note: This requires an input file 'test_bayer.raw'"
	@echo "Usage: ./run.sh 640 480 RGGB test_bayer.raw output.ppm"

# Show usage help
help:
	@echo "Bayer to RGB Converter Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all                    - Compile the application (default)"
	@echo "  clean                  - Remove compiled files and output images"
	@echo "  test                   - Show test information"
	@echo "  help                   - Show this help message"
	@echo "  create-test-file       - Create a sample test file"
	@echo "  install-deps          - Show dependency information"
	@echo ""
	@echo "To run the application, use the run.sh script:"
	@echo "  ./run.sh <width> <height> <pattern> <input_file> [output_file]"
	@echo ""
	@echo "Examples:"
	@echo "  ./run.sh 640 480 RGGB my_image.raw"
	@echo "  ./run.sh 1920 1080 BGGR camera.raw result.ppm"
	@echo ""
	@echo "Supported Bayer patterns: RGGB, BGGR, GRBG, GBRG"

# Create a sample test file (generates random data for testing)
create-test-file:
	@echo "Creating sample test file (100x100 RGGB pattern)..."
	@dd if=/dev/urandom of=test_bayer.raw bs=1 count=10000 2>/dev/null
	@echo "Created test_bayer.raw (100x100 pixels)"
	@echo "Run: ./run.sh 100 100 RGGB test_bayer.raw test_output.ppm"

# Install dependencies (if needed)
install-deps:
	@echo "No additional dependencies required for this C++ application."
	@echo "Make sure you have g++ compiler installed."

.PHONY: all clean test help create-test-file install-deps
