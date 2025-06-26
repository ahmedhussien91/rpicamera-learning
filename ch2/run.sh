#!/bin/bash

# Bayer to RGB Converter Runner Script
# Usage: ./run.sh <width> <height> <pattern> <input_file> [output_file]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TARGET="$SCRIPT_DIR/bayer_converter"

# Function to show usage
show_usage() {
    echo "Bayer to RGB Converter Runner Script"
    echo ""
    echo "Usage: $0 <width> <height> <pattern> <input_file> [output_file]"
    echo ""
    echo "Parameters:"
    echo "  width       - Image width in pixels (positive integer)"
    echo "  height      - Image height in pixels (positive integer)"
    echo "  pattern     - Bayer pattern (RGGB, BGGR, GRBG, or GBRG)"
    echo "  input_file  - Path to input Bayer raw image file"
    echo "  output_file - Path to output RGB image file (optional, default: output_rgb.ppm)"
    echo ""
    echo "Examples:"
    echo "  $0 640 480 RGGB input_bayer.raw"
    echo "  $0 1920 1080 BGGR camera.raw result.ppm"
    echo "  $0 800 600 GRBG test_image.raw output.ppm"
    echo ""
    echo "Supported Bayer patterns: RGGB, BGGR, GRBG, GBRG"
    echo ""
    echo "Predefined configurations:"
    echo "  $0 --640x480-rggb <input_file> [output_file]"
    echo "  $0 --1920x1080-bggr <input_file> [output_file]"
    echo "  $0 --800x600-grbg <input_file> [output_file]"
    echo "  $0 --1280x720-gbrg <input_file> [output_file]"
}

# Function to run with predefined configuration
run_predefined() {
    local config="$1"
    local input_file="$2"
    local output_file="$3"
    
    case "$config" in
        "--640x480-rggb")
            width=640
            height=480
            pattern="RGGB"
            default_output="output_640x480_rggb.ppm"
            ;;
        "--1920x1080-bggr")
            width=1920
            height=1080
            pattern="BGGR"
            default_output="output_1920x1080_bggr.ppm"
            ;;
        "--800x600-grbg")
            width=800
            height=600
            pattern="GRBG"
            default_output="output_800x600_grbg.ppm"
            ;;
        "--1280x720-gbrg")
            width=1280
            height=720
            pattern="GBRG"
            default_output="output_1280x720_gbrg.ppm"
            ;;
        *)
            echo "Error: Unknown predefined configuration: $config"
            show_usage
            exit 1
            ;;
    esac
    
    if [ -z "$input_file" ]; then
        echo "Error: Input file required for predefined configuration"
        show_usage
        exit 1
    fi
    
    output_file="${output_file:-$default_output}"
    
    echo "Running with predefined configuration: $config"
    echo "Dimensions: ${width}x${height}, Pattern: $pattern"
    echo "Input: $input_file, Output: $output_file"
    
    run_converter "$width" "$height" "$pattern" "$input_file" "$output_file"
}

# Function to validate parameters
validate_params() {
    local width="$1"
    local height="$2"
    local pattern="$3"
    local input_file="$4"
    
    # Check if width and height are positive integers
    if ! [[ "$width" =~ ^[1-9][0-9]*$ ]]; then
        echo "Error: Width must be a positive integer, got: $width"
        return 1
    fi
    
    if ! [[ "$height" =~ ^[1-9][0-9]*$ ]]; then
        echo "Error: Height must be a positive integer, got: $height"
        return 1
    fi
    
    # Check if pattern is valid
    case "$pattern" in
        "RGGB"|"BGGR"|"GRBG"|"GBRG")
            ;;
        *)
            echo "Error: Invalid Bayer pattern: $pattern"
            echo "Valid patterns: RGGB, BGGR, GRBG, GBRG"
            return 1
            ;;
    esac
    
    # Check if input file exists
    if [ ! -f "$input_file" ]; then
        echo "Error: Input file does not exist: $input_file"
        return 1
    fi
    
    return 0
}

# Function to run the converter
run_converter() {
    local width="$1"
    local height="$2"
    local pattern="$3"
    local input_file="$4"
    local output_file="$5"
    
    # Check if binary exists
    if [ ! -f "$TARGET" ]; then
        echo "Error: Binary not found: $TARGET"
        echo "Please compile first with: make"
        exit 1
    fi
    
    # Make binary executable if needed
    if [ ! -x "$TARGET" ]; then
        chmod +x "$TARGET"
    fi
    
    # Run the converter
    if [ -n "$output_file" ]; then
        "$TARGET" "$width" "$height" "$pattern" "$input_file" "$output_file"
    else
        "$TARGET" "$width" "$height" "$pattern" "$input_file"
    fi
}

# Main script logic
main() {
    # Check for help
    if [ "$1" = "-h" ] || [ "$1" = "--help" ] || [ $# -eq 0 ]; then
        show_usage
        exit 0
    fi
    
    # Check for predefined configurations
    if [[ "$1" == --* ]]; then
        run_predefined "$1" "$2" "$3"
        exit $?
    fi
    
    # Check minimum required parameters
    if [ $# -lt 4 ]; then
        echo "Error: Missing required parameters"
        echo ""
        show_usage
        exit 1
    fi
    
    # Parse parameters
    width="$1"
    height="$2"
    pattern="$3"
    input_file="$4"
    output_file="$5"
    
    # Validate parameters
    if ! validate_params "$width" "$height" "$pattern" "$input_file"; then
        exit 1
    fi
    
    # Run the converter
    run_converter "$width" "$height" "$pattern" "$input_file" "$output_file"
}

# Run main function with all arguments
main "$@"
