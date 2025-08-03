// image_recommendation_wasm.cpp
// WebAssembly-compatible image recommendation system
// Removed all OpenCV dependencies - UI is now handled by JavaScript

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

struct Image_Result {
    std::string path;
    double score;
};

// WebAssembly-compatible image processing functions
class ImageProcessor {
private:
    // ImageNet normalization constants
    const double mean_[3] = {0.485, 0.456, 0.406};
    const double std_[3] = {0.229, 0.224, 0.225};
    
public:
    // Normalize image data for ResNet input
    std::vector<float> normalizeImage(const std::vector<uint8_t>& imageData, int width, int height) {
        std::vector<float> normalized(width * height * 3);
        
        for (int i = 0; i < width * height; i++) {
            for (int c = 0; c < 3; c++) {
                int srcIdx = i * 4 + c; // RGBA format
                int dstIdx = i * 3 + c; // RGB format
                
                if (srcIdx < imageData.size()) {
                    float pixel = static_cast<float>(imageData[srcIdx]) / 255.0f;
                    normalized[dstIdx] = (pixel - mean_[c]) / std_[c];
                }
            }
        }
        
        return normalized;
    }
    
    // Calculate cosine similarity between two feature vectors
    double cosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2) {
        if (v1.size() != v2.size() || v1.empty()) {
            return 0.0;
        }
        
        double dotProduct = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;
        
        for (size_t i = 0; i < v1.size(); i++) {
            dotProduct += v1[i] * v2[i];
            norm1 += v1[i] * v1[i];
            norm2 += v2[i] * v2[i];
        }
        
        norm1 = std::sqrt(norm1);
        norm2 = std::sqrt(norm2);
        
        if (norm1 == 0.0 || norm2 == 0.0) {
            return 0.0;
        }
        
        return dotProduct / (norm1 * norm2);
    }
    
    // Process image data and return normalized tensor
    std::vector<float> processImage(const std::vector<uint8_t>& imageData, int width, int height) {
        // Resize to 224x224 if needed (this would be done in JavaScript)
        // For now, assume input is already 224x224
        return normalizeImage(imageData, width, height);
    }
    
    // Calculate similarity between two images
    double calculateImageSimilarity(const std::vector<uint8_t>& img1, const std::vector<uint8_t>& img2, 
                                   int width1, int height1, int width2, int height2) {
        auto normalized1 = processImage(img1, width1, height1);
        auto normalized2 = processImage(img2, width2, height2);
        
        return cosineSimilarity(normalized1, normalized2);
    }
    
    // Get image statistics
    std::vector<double> getImageStats(const std::vector<uint8_t>& imageData) {
        std::vector<double> stats(3, 0.0); // mean for R, G, B
        
        if (imageData.empty()) return stats;
        
        int pixelCount = imageData.size() / 4; // RGBA format
        
        for (int i = 0; i < pixelCount; i++) {
            for (int c = 0; c < 3; c++) {
                stats[c] += imageData[i * 4 + c];
            }
        }
        
        for (int c = 0; c < 3; c++) {
            stats[c] /= pixelCount;
        }
        
        return stats;
    }
};

// Global instance for WebAssembly
ImageProcessor processor;

// WebAssembly exported functions
extern "C" {
    // Initialize the processor
    EMSCRIPTEN_KEEPALIVE
    void initProcessor() {
        // Any initialization if needed
    }
    
    // Calculate similarity between two images
    EMSCRIPTEN_KEEPALIVE
    double calculateSimilarity(uint8_t* img1, int size1, int width1, int height1,
                              uint8_t* img2, int size2, int width2, int height2) {
        std::vector<uint8_t> image1(img1, img1 + size1);
        std::vector<uint8_t> image2(img2, img2 + size2);
        
        return processor.calculateImageSimilarity(image1, image2, width1, height1, width2, height2);
    }
    
    // Process single image
    EMSCRIPTEN_KEEPALIVE
    void processImage(uint8_t* imageData, int size, int width, int height, float* output) {
        std::vector<uint8_t> image(imageData, imageData + size);
        auto normalized = processor.processImage(image, width, height);
        
        // Copy to output buffer
        for (size_t i = 0; i < normalized.size(); i++) {
            output[i] = normalized[i];
        }
    }
    
    // Get image statistics
    EMSCRIPTEN_KEEPALIVE
    void getImageStats(uint8_t* imageData, int size, double* stats) {
        std::vector<uint8_t> image(imageData, imageData + size);
        auto statsVec = processor.getImageStats(image);
        
        for (size_t i = 0; i < statsVec.size(); i++) {
            stats[i] = statsVec[i];
        }
    }
}

// JavaScript bindings using Embind
EMSCRIPTEN_BINDINGS(image_processor) {
    class_<ImageProcessor>("ImageProcessor")
        .constructor<>()
        .function("normalizeImage", &ImageProcessor::normalizeImage)
        .function("cosineSimilarity", &ImageProcessor::cosineSimilarity)
        .function("processImage", &ImageProcessor::processImage)
        .function("calculateImageSimilarity", &ImageProcessor::calculateImageSimilarity)
        .function("getImageStats", &ImageProcessor::getImageStats);
    
    // Register vector types
    register_vector<float>("FloatVector");
    register_vector<uint8_t>("Uint8Vector");
    register_vector<double>("DoubleVector");
    
    // Register struct
    value_object<Image_Result>("ImageResult")
        .field("path", &Image_Result::path)
        .field("score", &Image_Result::score);
}

// Main function for WebAssembly module
int main() {
    // WebAssembly module initialization
    std::cout << "Image Recommendation WebAssembly Module Loaded" << std::endl;
    return 0;
}
