# Image Recommendation System

A web-based image recommendation system that uses ResNet-50 to extract feature vectors from images and finds similar images using cosine similarity.

## Features

- Extract feature vectors from images using ResNet-50 model
- Calculate cosine similarity between image features
- Display 6 random images in a 2x3 grid
- Click on any image to see 5 most similar images
- Beautiful, responsive web interface
- Real-time image processing and recommendation

## Requirements

- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.16 or higher
- Node.js 14.0 or higher
- ONNX Runtime
- Images in the `static` folder

## Local Installation

1. **Install ONNX Runtime:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libonnxruntime-dev
   
   # macOS
   brew install onnxruntime
   
   # Windows (using vcpkg)
   vcpkg install onnxruntime
   ```

2. **Install Node.js dependencies:**
   ```bash
   npm install
   ```

3. **Build the C++ feature extractor:**
   ```bash
   npm run build
   ```

## Setup

1. **Place your images in the `static` folder:**
   - Supported formats: JPG, JPEG, PNG, BMP, TIFF
   - The system will automatically detect and process these images

2. **Ensure the ResNet model is in the project root:**
   - The `resnet50-v1-7.onnx` file should be in the project root directory

## Local Usage

1. **Start the server:**
   ```bash
   npm start
   ```

2. **Open your browser and navigate to:**
   ```
   http://localhost:3000
   ```

3. **Using the application:**
   - Click "Process Images" to extract features from all images in the static folder
   - Click "Load Random Images" to display 6 random images
   - Click on any image to see 5 most similar images based on cosine similarity
   - The selected image appears in the top-left, with similar images arranged around it

## Render Deployment

This project is configured for deployment on Render using Docker.

### Prerequisites

1. **GitHub Repository:** Push your code to a GitHub repository
2. **Render Account:** Sign up at [render.com](https://render.com)

### Deployment Steps

1. **Connect to Render:**
   - Log in to your Render dashboard
   - Click "New +" and select "Web Service"
   - Connect your GitHub repository

2. **Configure the service:**
   - **Name:** `image-recommendation-system` (or your preferred name)
   - **Environment:** `Docker`
   - **Region:** Choose closest to your users
   - **Branch:** `main` (or your default branch)
   - **Root Directory:** Leave empty (if your code is in the root)
   - **Build Command:** Leave empty (handled by Dockerfile)
   - **Start Command:** `npm start`

3. **Environment Variables (optional):**
   - `NODE_ENV`: `production`
   - `PORT`: `3000` (Render will set this automatically)

4. **Deploy:**
   - Click "Create Web Service"
   - Render will automatically build and deploy your application

### Post-Deployment

1. **Upload Images:**
   - After deployment, you'll need to upload images to the `static` folder
   - You can do this by:
     - Using Render's file system (if available)
     - Adding images to your repository and redeploying
     - Using a file upload feature (not implemented in this version)

2. **Process Images:**
   - Visit your deployed application
   - Click "Process Images" to extract features
   - Then click "Load Random Images" to see your images

## File Structure

```
├── feature_extractor.cpp    # C++ feature extraction code
├── CMakeLists.txt          # CMake build configuration
├── Dockerfile              # Docker configuration for Render
├── render.yaml             # Render deployment configuration
├── server.js               # Node.js server
├── package.json            # Node.js dependencies
├── public/
│   └── index.html         # Web interface
├── static/                 # Place your images here
├── resnet50-v1-7.onnx     # ResNet model
└── README.md              # This file
```

## API Endpoints

- `GET /` - Main application page
- `GET /health` - Health check endpoint
- `GET /api/random-images` - Get 6 random images
- `POST /api/process-images` - Process all images to extract features
- `POST /api/similar-images` - Get similar images based on selected image

## Technical Details

- **Feature Extraction:** Uses ResNet-50 to extract 2048-dimensional feature vectors
- **Similarity Calculation:** Cosine similarity between feature vectors
- **Image Processing:** Resizes images to 224x224 and normalizes with ImageNet statistics
- **Web Interface:** Responsive grid layout with smooth animations
- **Deployment:** Docker-based deployment optimized for Render

## Troubleshooting

### Local Development

1. **ONNX Runtime not found:**
   - Ensure ONNX Runtime is properly installed
   - Check that the library is in your system's library path

2. **Images not loading:**
   - Verify images are in the `static` folder
   - Check file permissions
   - Ensure supported image formats are used

3. **Feature extraction fails:**
   - Ensure the ResNet model file is present
   - Check that images are valid and readable
   - Verify C++ build completed successfully

### Render Deployment

1. **Build fails:**
   - Check that all files are committed to your repository
   - Ensure the Dockerfile is in the root directory
   - Verify that the ResNet model file is included

2. **Application doesn't start:**
   - Check the logs in Render dashboard
   - Ensure the PORT environment variable is set correctly
   - Verify that the C++ application built successfully

3. **No images appear:**
   - Upload images to the `static` folder
   - Click "Process Images" to extract features
   - Check that images are in supported formats

## License

MIT License 