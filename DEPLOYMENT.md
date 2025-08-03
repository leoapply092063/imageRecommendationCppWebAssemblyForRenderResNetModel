# Deployment Guide for Render

## Quick Setup

### 1. Prerequisites
- Node.js installed on your local machine
- Git repository with your code
- Render account (free at https://render.com)

### 2. Local Setup

```bash
# Install dependencies
npm install

# Download the ResNet model
npm run download-model

# Test locally
npm start
```

### 3. Deploy to Render

1. **Push to GitHub**
   ```bash
   git add .
   git commit -m "Initial commit"
   git push origin main
   ```

2. **Create Render Service**
   - Go to https://dashboard.render.com/
   - Click "New +" → "Web Service"
   - Connect your GitHub repository
   - Configure:
     - **Name**: `image-recommendation-wasm`
     - **Environment**: `Node`
     - **Build Command**: `npm install && npm run download-model`
     - **Start Command**: `npm start`
     - **Plan**: Free

3. **Deploy**
   - Click "Create Web Service"
   - Wait for build to complete
   - Your app will be available at the provided URL

## File Structure

```
├── static/                          # Images
│   └── *.png                       # Your images
├── public/
│   ├── index.html                  # Main application
│   └── app.js                      # WebAssembly wrapper
├── resnet50-v1-7.onnx             # ResNet model (in root directory)
├── server.js                       # Node.js server
├── package.json                    # Dependencies
├── download-model.js               # Model downloader
├── render.yaml                     # Render config
└── README.md                       # Documentation
```

## Key Changes Made

### 1. Removed OpenCV Dependencies
- Replaced all `cv::` calls with JavaScript equivalents
- Used HTML5 Canvas for image processing
- Implemented client-side image loading and resizing

### 2. Updated Image Paths
- Changed from `C:/TesT_` to `./static/`
- All images now served from the `static/` directory
- Server automatically scans for supported image formats

### 3. WebAssembly Integration
- Created JavaScript wrapper for WebAssembly functions
- Used ONNX Runtime Web for ResNet inference
- Implemented client-side similarity calculation

### 4. Node.js Server
- Express.js server for serving static files
- API endpoint for image discovery
- CORS handling for local development

## Troubleshooting

### Common Issues

1. **Model Download Fails**
   ```bash
   # Manual download
   curl -L -o resnet50-v1-7.onnx https://github.com/onnx/models/raw/main/vision/classification/resnet/model/resnet50-v1-7.onnx
   ```

2. **Images Not Loading**
   - Ensure images are in `static/` directory
   - Check file extensions (.jpg, .jpeg, .png, .bmp)
   - Verify server is running on correct port

3. **CORS Errors**
   - Server is configured to handle CORS
   - Check browser console for specific errors

4. **Memory Issues**
   - Application uses client-side processing
   - Large images are automatically resized
   - Consider compressing images for better performance

### Performance Tips

- Use compressed images (JPEG for photos, PNG for graphics)
- The ResNet model is ~100MB - first load may be slow
- Consider implementing lazy loading for large image collections
- Client-side processing avoids server memory constraints

## Environment Variables

No environment variables are required for basic deployment. The application uses:
- `PORT`: Automatically set by Render
- `NODE_ENV`: Set to 'production' by Render

## Monitoring

- Check Render dashboard for logs
- Monitor memory usage (free tier has limits)
- Use browser developer tools for client-side debugging 