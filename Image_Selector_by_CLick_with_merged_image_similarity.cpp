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

// WebAssembly-compatible math functions
class ImageProcessor {
public:
    
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
    
    // Calculate cosine similarity between feature vectors
    EMSCRIPTEN_KEEPALIVE
    double cosineSimilarity(float* vec1, float* vec2, int size) {
        std::vector<float> vector1(vec1, vec1 + size);
        std::vector<float> vector2(vec2, vec2 + size);
        
        return processor.cosineSimilarity(vector1, vector2);
    }
    


}

// JavaScript bindings using Embind
EMSCRIPTEN_BINDINGS(image_processor) {
    class_<ImageProcessor>("ImageProcessor")
        .constructor<>()
        .function("cosineSimilarity", &ImageProcessor::cosineSimilarity);
    
    // Register vector types
    register_vector<float>("FloatVector");
    register_vector<uint8_t>("Uint8Vector");
    
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
