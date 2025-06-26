#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>

/*
 * Famous Bayer Filter Patterns:
 * 
 * RGGB (Red-Green-Green-Blue):
 * R G R G
 * G B G B
 * R G R G
 * G B G B
 * 
 * BGGR (Blue-Green-Green-Red):
 * B G B G
 * G R G R
 * B G B G
 * G R G R
 * 
 * GRBG (Green-Red-Blue-Green):
 * G R G R
 * B G B G
 * G R G R
 * B G B G
 * 
 * GBRG (Green-Blue-Red-Green):
 * G B G B
 * R G R G
 * G B G B
 * R G R G
 */

enum BayerPattern {
    RGGB,
    BGGR,
    GRBG,
    GBRG
};

class BayerToRGB {
private:
    int width, height;
    BayerPattern pattern;
    std::vector<uint8_t> bayerData;
    std::vector<uint8_t> rgbData;

public:
    BayerToRGB(int w, int h, BayerPattern p) : width(w), height(h), pattern(p) {
        bayerData.resize(width * height);
        rgbData.resize(width * height * 3);
    }

    bool readBayerImage(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }
        
        file.read(reinterpret_cast<char*>(bayerData.data()), width * height);
        if (!file.good()) {
            std::cerr << "Error: Failed to read image data" << std::endl;
            return false;
        }
        
        file.close();
        return true;
    }

    void demosaic() {
        // Simple bilinear interpolation demosaicing
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                int idx = y * width + x;
                int rgbIdx = idx * 3;
                
                uint8_t r, g, b;
                
                switch (pattern) {
                    case RGGB:
                        demosaicRGGB(x, y, r, g, b);
                        break;
                    case BGGR:
                        demosaicBGGR(x, y, r, g, b);
                        break;
                    case GRBG:
                        demosaicGRBG(x, y, r, g, b);
                        break;
                    case GBRG:
                        demosaicGBRG(x, y, r, g, b);
                        break;
                }
                
                rgbData[rgbIdx] = r;     // Red
                rgbData[rgbIdx + 1] = g; // Green
                rgbData[rgbIdx + 2] = b; // Blue
            }
        }
    }

private:
    void demosaicRGGB(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) {
        int idx = y * width + x;
        
        if ((y % 2 == 0 && x % 2 == 0) || (y % 2 == 1 && x % 2 == 1)) {
            // R or B pixel
            if (y % 2 == 0 && x % 2 == 0) {
                // R pixel
                r = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                b = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            } else {
                // B pixel
                b = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                r = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            }
        } else {
            // G pixel
            g = bayerData[idx];
            if (y % 2 == 0) {
                // G pixel in R row
                r = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                b = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            } else {
                // G pixel in B row
                b = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                r = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            }
        }
    }

    void demosaicBGGR(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) {
        int idx = y * width + x;
        
        if ((y % 2 == 0 && x % 2 == 0) || (y % 2 == 1 && x % 2 == 1)) {
            // B or R pixel
            if (y % 2 == 0 && x % 2 == 0) {
                // B pixel
                b = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                r = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            } else {
                // R pixel
                r = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                b = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            }
        } else {
            // G pixel
            g = bayerData[idx];
            if (y % 2 == 0) {
                // G pixel in B row
                b = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                r = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            } else {
                // G pixel in R row
                r = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                b = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            }
        }
    }

    void demosaicGRBG(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) {
        int idx = y * width + x;
        
        if ((y % 2 == 0 && x % 2 == 1) || (y % 2 == 1 && x % 2 == 0)) {
            // R or B pixel
            if (y % 2 == 0 && x % 2 == 1) {
                // R pixel
                r = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                b = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            } else {
                // B pixel
                b = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                r = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            }
        } else {
            // G pixel
            g = bayerData[idx];
            if (y % 2 == 0) {
                // G pixel in G row
                r = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                b = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            } else {
                // G pixel in B row
                b = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                r = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            }
        }
    }

    void demosaicGBRG(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) {
        int idx = y * width + x;
        
        if ((y % 2 == 0 && x % 2 == 1) || (y % 2 == 1 && x % 2 == 0)) {
            // B or R pixel
            if (y % 2 == 0 && x % 2 == 1) {
                // B pixel
                b = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                r = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            } else {
                // R pixel
                r = bayerData[idx];
                g = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x] + 
                     bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 4;
                b = (bayerData[(y-1)*width + (x-1)] + bayerData[(y-1)*width + (x+1)] +
                     bayerData[(y+1)*width + (x-1)] + bayerData[(y+1)*width + (x+1)]) / 4;
            }
        } else {
            // G pixel
            g = bayerData[idx];
            if (y % 2 == 0) {
                // G pixel in G row
                b = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                r = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            } else {
                // G pixel in R row
                r = (bayerData[y*width + (x-1)] + bayerData[y*width + (x+1)]) / 2;
                b = (bayerData[(y-1)*width + x] + bayerData[(y+1)*width + x]) / 2;
            }
        }
    }

public:
    bool saveRGBImage(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create output file " << filename << std::endl;
            return false;
        }
        
        // Write PPM header
        file << "P6\n" << width << " " << height << "\n255\n";
        
        // Write RGB data
        file.write(reinterpret_cast<const char*>(rgbData.data()), width * height * 3);
        
        file.close();
        return true;
    }

    void printImageInfo() const {
        std::cout << "Image dimensions: " << width << "x" << height << std::endl;
        std::cout << "Bayer pattern: ";
        switch (pattern) {
            case RGGB: std::cout << "RGGB"; break;
            case BGGR: std::cout << "BGGR"; break;
            case GRBG: std::cout << "GRBG"; break;
            case GBRG: std::cout << "GBRG"; break;
        }
        std::cout << std::endl;
    }
};

BayerPattern stringToPattern(const std::string& patternStr) {
    if (patternStr == "RGGB") return RGGB;
    if (patternStr == "BGGR") return BGGR;
    if (patternStr == "GRBG") return GRBG;
    if (patternStr == "GBRG") return GBRG;
    
    std::cerr << "Invalid Bayer pattern: " << patternStr << std::endl;
    std::cerr << "Valid patterns: RGGB, BGGR, GRBG, GBRG" << std::endl;
    exit(1);
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <width> <height> <pattern> <input_file> [output_file]" << std::endl;
    std::cout << "  width:       Image width in pixels" << std::endl;
    std::cout << "  height:      Image height in pixels" << std::endl;
    std::cout << "  pattern:     Bayer pattern (RGGB, BGGR, GRBG, or GBRG)" << std::endl;
    std::cout << "  input_file:  Path to input Bayer raw image file" << std::endl;
    std::cout << "  output_file: Path to output RGB image file (optional, default: output_rgb.ppm)" << std::endl;
    std::cout << std::endl;
    std::cout << "Example: " << programName << " 640 480 RGGB input_bayer.raw output.ppm" << std::endl;
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc < 5 || argc > 6) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    int width = std::atoi(argv[1]);
    int height = std::atoi(argv[2]);
    std::string patternStr = argv[3];
    std::string inputFile = argv[4];
    std::string outputFile = (argc == 6) ? argv[5] : "output_rgb.ppm";
    
    // Validate arguments
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Width and height must be positive integers" << std::endl;
        return 1;
    }
    
    BayerPattern pattern = stringToPattern(patternStr);
    
    BayerToRGB converter(width, height, pattern);
    
    std::cout << "Bayer to RGB Converter" << std::endl;
    converter.printImageInfo();
    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Read Bayer raw image
    if (!converter.readBayerImage(inputFile)) {
        std::cerr << "Failed to read Bayer image: " << inputFile << std::endl;
        return 1;
    }
    
    std::cout << "Successfully read Bayer image" << std::endl;
    
    // Convert to RGB
    std::cout << "Converting Bayer to RGB..." << std::endl;
    converter.demosaic();
    
    // Save RGB image as PPM
    if (!converter.saveRGBImage(outputFile)) {
        std::cerr << "Failed to save RGB image: " << outputFile << std::endl;
        return 1;
    }
    
    std::cout << "Successfully converted and saved RGB image: " << outputFile << std::endl;
    
    return 0;
}
