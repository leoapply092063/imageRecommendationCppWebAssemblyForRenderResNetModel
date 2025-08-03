# Image Recommendation System with WebAssembly

This project implements an image recommendation system using WebAssembly, JavaScript, and ResNet for feature extraction. It allows users to select an image and find similar images from a collection using deep learning-based similarity matching.

## Features

- **WebAssembly Integration**: C++ code compiled to WebAssembly for performance
- **ResNet Model**: Uses ResNet-50 for feature extraction
- **Modern UI**: Beautiful, responsive web interface
- **Real-time Processing**: Client-side image processing and similarity calculation
- **Node.js Server**: Express.js server for serving static files and API endpoints

## Prerequisites

### For Local Development:
- Node.js (v16 or higher)
- npm or yarn
- Emscripten SDK (for WebAssembly compilation)

### For Deployment:
- Render account (free tier available)
- ResNet model file

## Setup Instructions

### 1. Install Dependencies

```bash
npm install
```

### 2. Get the ResNet Model

You need to download the ResNet-50 ONNX model. You can get it from:

1. **ONNX Model Zoo**: Download `resnet50-v1-7.onnx` from the [ONNX Model Zoo](https://github.com/onnx/models/tree/main/vision/classification/resnet)
2. **Direct Download**: Use this command to download it directly:

```bash
# Download ResNet model (you'll need to do this manually)
# Place the resnet50-v1-7.onnx file in the root directory
```

### 3. Compile WebAssembly (Optional)

If you want to compile the C++ code to WebAssembly:

```bash
# Install Emscripten SDK first
# Then run:
npm run build
```

### 4. Run Locally

```bash
npm start
```

The application will be available at `http://localhost:3000`

## Deployment to Render

### 1. Prepare Your Repository

1. Make sure your ResNet model is in the root directory
2. Commit all files to your Git repository
3. Push to GitHub

### 2. Deploy on Render

1. Go to [Render Dashboard](https://dashboard.render.com/)
2. Click "New +" and select "Web Service"
3. Connect your GitHub repository
4. Configure the service:
   - **Name**: `image-recommendation-wasm`
   - **Environment**: `Node`
   - **Build Command**: `npm install`
   - **Start Command**: `npm start`
   - **Plan**: Free

### 3. Environment Variables

No additional environment variables are required for basic deployment.

## Project Structure

```
├── static/                          # Static files (images)
│   └── *.png                       # Your images
├── public/                          # Web assets
│   └── index.html                  # Main application
├── resnet50-v1-7.onnx             # ResNet model (in root directory)
├── server.js                       # Node.js server
├── package.json                    # Dependencies
├── render.yaml                     # Render configuration
└── README.md                       # This file
```

## How It Works

1. **Image Loading**: The server scans the `static/` directory for images
2. **Image Selection**: Users can select an image from a grid display
3. **Feature Extraction**: Selected image is processed through ResNet to extract features
4. **Similarity Calculation**: All other images are processed and compared using cosine similarity
5. **Recommendations**: Top similar images are displayed with similarity scores

## Technical Details

### WebAssembly Integration
- C++ code compiled to WebAssembly using Emscripten
- JavaScript handles UI and image processing
- ONNX Runtime Web for model inference

### Image Processing
- Images are resized to 224x224 pixels (ResNet input size)
- Normalized using ImageNet mean and standard deviation
- Feature vectors extracted from the penultimate layer

### Similarity Calculation
- Cosine similarity between feature vectors
- Higher scores indicate more similar images
- Results sorted by similarity score

## Troubleshooting

### Common Issues

1. **Model not found**: Make sure `resnet50-v1-7.onnx` is in the root directory
2. **Images not loading**: Check that images are in the `static/` directory with supported extensions (.jpg, .jpeg, .png, .bmp)
3. **CORS errors**: The server is configured to serve static files correctly
4. **Memory issues**: The application uses client-side processing to avoid server memory constraints

### Performance Tips

- Use compressed images for faster loading
- Consider implementing lazy loading for large image collections
- The ResNet model is loaded once and cached in the browser

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is open source and available under the MIT License. 