#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>

// ONNX Runtime headers
#include <onnxruntime_cxx_api.h>

// Image processing headers (using stb_image for simplicity)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct ImageFeature {
    std::string filename;
    std::vector<float> features;
    double similarity;
};

class ResNetFeatureExtractor {
private:
    Ort::Env env;
    Ort::Session session;
    std::vector<const char*> input_names;
    std::vector<const char*> output_names;
    
    // ResNet preprocessing constants
    const int input_height = 224;
    const int input_width = 224;
    const int channels = 3;
    const std::vector<float> mean = {0.485f, 0.456f, 0.406f};
    const std::vector<float> std = {0.229f, 0.224f, 0.225f};

public:
    ResNetFeatureExtractor(const std::string& model_path) {
        // Initialize ONNX Runtime environment
        env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "resnet_feature_extractor");
        
        // Session options
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
        
        // Load the model
        session = Ort::Session(env, model_path.c_str(), session_options);
        
        // Get input and output names
        Ort::AllocatorWithDefaultOptions allocator;
        input_names.push_back(session.GetInputName(0, allocator));
        output_names.push_back(session.GetOutputName(0, allocator));
    }
    
    std::vector<float> preprocess_image(const std::string& image_path) {
        int width, height, channels;
        unsigned char* data = stbi_load(image_path.c_str(), &width, &height, &channels, 3);
        
        if (!data) {
            throw std::runtime_error("Failed to load image: " + image_path);
        }
        
        // Resize and normalize image
        std::vector<float> input_tensor(input_height * input_width * 3);
        
        // Simple nearest neighbor resize (for production, use proper resizing)
        for (int y = 0; y < input_height; y++) {
            for (int x = 0; x < input_width; x++) {
                int src_x = (x * width) / input_width;
                int src_y = (y * height) / input_height;
                
                for (int c = 0; c < 3; c++) {
                    int src_idx = (src_y * width + src_x) * 3 + c;
                    int dst_idx = (y * input_width + x) * 3 + c;
                    
                    float pixel_value = static_cast<float>(data[src_idx]) / 255.0f;
                    // Normalize with ImageNet mean and std
                    input_tensor[dst_idx] = (pixel_value - mean[c]) / std[c];
                }
            }
        }
        
        stbi_image_free(data);
        return input_tensor;
    }
    
    std::vector<float> extract_features(const std::string& image_path) {
        try {
            // Preprocess image
            std::vector<float> input_tensor = preprocess_image(image_path);
            
            // Create input tensor
            std::vector<int64_t> input_shape = {1, 3, input_height, input_width};
            auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
            
            Ort::Value input_tensor_ort = Ort::Value::CreateTensor<float>(
                memory_info, input_tensor.data(), input_tensor.size(),
                input_shape.data(), input_shape.size()
            );
            
            // Run inference
            auto output_tensors = session.Run(
                Ort::RunOptions{nullptr},
                input_names.data(), &input_tensor_ort, 1,
                output_names.data(), output_names.size()
            );
            
            // Extract features (use the last layer before classification)
            float* output_data = output_tensors[0].GetTensorMutableData<float>();
            size_t output_size = output_tensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            
            std::vector<float> features(output_data, output_data + output_size);
            return features;
            
        } catch (const std::exception& e) {
            std::cerr << "Error extracting features from " << image_path << ": " << e.what() << std::endl;
            return std::vector<float>();
        }
    }
    
    double cosine_similarity(const std::vector<float>& vec1, const std::vector<float>& vec2) {
        if (vec1.size() != vec2.size() || vec1.empty()) {
            return 0.0;
        }
        
        double dot_product = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;
        
        for (size_t i = 0; i < vec1.size(); i++) {
            dot_product += vec1[i] * vec2[i];
            norm1 += vec1[i] * vec1[i];
            norm2 += vec2[i] * vec2[i];
        }
        
        norm1 = std::sqrt(norm1);
        norm2 = std::sqrt(norm2);
        
        if (norm1 == 0.0 || norm2 == 0.0) {
            return 0.0;
        }
        
        return dot_product / (norm1 * norm2);
    }
    
    std::vector<ImageFeature> process_images(const std::string& static_folder) {
        std::vector<ImageFeature> image_features;
        std::vector<std::string> image_extensions = {".jpg", ".jpeg", ".png", ".bmp", ".tiff"};
        
        for (const auto& entry : std::filesystem::directory_iterator(static_folder)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (std::find(image_extensions.begin(), image_extensions.end(), extension) != image_extensions.end()) {
                    std::string image_path = entry.path().string();
                    std::vector<float> features = extract_features(image_path);
                    
                    if (!features.empty()) {
                        ImageFeature img_feature;
                        img_feature.filename = entry.path().filename().string();
                        img_feature.features = features;
                        img_feature.similarity = 0.0;
                        image_features.push_back(img_feature);
                    }
                }
            }
        }
        
        return image_features;
    }
    
    std::vector<ImageFeature> find_similar_images(const std::vector<ImageFeature>& all_features, 
                                                  const std::string& selected_image, 
                                                  const std::vector<std::string>& excluded_images,
                                                  int num_similar = 5) {
        std::vector<ImageFeature> similar_images;
        
        // Find the selected image features
        const ImageFeature* selected_feature = nullptr;
        for (const auto& feature : all_features) {
            if (feature.filename == selected_image) {
                selected_feature = &feature;
                break;
            }
        }
        
        if (!selected_feature) {
            return similar_images;
        }
        
        // Calculate similarities
        std::vector<ImageFeature> candidates;
        for (const auto& feature : all_features) {
            // Skip the selected image and excluded images
            if (feature.filename == selected_image || 
                std::find(excluded_images.begin(), excluded_images.end(), feature.filename) != excluded_images.end()) {
                continue;
            }
            
            ImageFeature candidate = feature;
            candidate.similarity = cosine_similarity(selected_feature->features, feature.features);
            candidates.push_back(candidate);
        }
        
        // Sort by similarity (descending)
        std::sort(candidates.begin(), candidates.end(), 
                  [](const ImageFeature& a, const ImageFeature& b) {
                      return a.similarity > b.similarity;
                  });
        
        // Return top N similar images
        for (int i = 0; i < std::min(num_similar, static_cast<int>(candidates.size())); i++) {
            similar_images.push_back(candidates[i]);
        }
        
        return similar_images;
    }
};

// Main function for testing
int main() {
    try {
        std::string model_path = "resnet50-v1-7.onnx";
        std::string static_folder = "static";
        
        ResNetFeatureExtractor extractor(model_path);
        std::vector<ImageFeature> features = extractor.process_images(static_folder);
        
        std::cout << "Processed " << features.size() << " images." << std::endl;
        
        // Save features to file for web application
        std::ofstream features_file("image_features.txt");
        for (const auto& feature : features) {
            features_file << feature.filename << " ";
            for (size_t i = 0; i < feature.features.size(); i++) {
                features_file << std::fixed << std::setprecision(6) << feature.features[i];
                if (i < feature.features.size() - 1) features_file << " ";
            }
            features_file << std::endl;
        }
        features_file.close();
        
        std::cout << "Features saved to image_features.txt" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 