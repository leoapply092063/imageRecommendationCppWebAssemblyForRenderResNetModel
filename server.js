const express = require('express');
const path = require('path');
const fs = require('fs');
const { exec } = require('child_process');
const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(express.json());
app.use(express.static('static'));
app.use(express.static('public'));

// Store image features in memory
let imageFeatures = [];
let processedImages = new Set();

// Load image features from file
function loadImageFeatures() {
    try {
        if (fs.existsSync('image_features.txt')) {
            const data = fs.readFileSync('image_features.txt', 'utf8');
            const lines = data.trim().split('\n');
            
            imageFeatures = lines.map(line => {
                const parts = line.split(' ');
                const filename = parts[0];
                const features = parts.slice(1).map(parseFloat);
                return { filename, features };
            });
            
            console.log(`Loaded ${imageFeatures.length} image features`);
        }
    } catch (error) {
        console.error('Error loading image features:', error);
    }
}

// Calculate cosine similarity
function cosineSimilarity(vec1, vec2) {
    if (vec1.length !== vec2.length || vec1.length === 0) {
        return 0.0;
    }
    
    let dotProduct = 0.0;
    let norm1 = 0.0;
    let norm2 = 0.0;
    
    for (let i = 0; i < vec1.length; i++) {
        dotProduct += vec1[i] * vec2[i];
        norm1 += vec1[i] * vec1[i];
        norm2 += vec2[i] * vec2[i];
    }
    
    norm1 = Math.sqrt(norm1);
    norm2 = Math.sqrt(norm2);
    
    if (norm1 === 0.0 || norm2 === 0.0) {
        return 0.0;
    }
    
    return dotProduct / (norm1 * norm2);
}

// Get random images for initial display
app.get('/api/random-images', (req, res) => {
    try {
        // Ensure static directory exists
        if (!fs.existsSync('static')) {
            fs.mkdirSync('static', { recursive: true });
        }
        
        const imageFiles = fs.readdirSync('static')
            .filter(file => /\.(jpg|jpeg|png|bmp|tiff)$/i.test(file));
        
        // If no images found, return empty array
        if (imageFiles.length === 0) {
            return res.json({ images: [] });
        }
        
        // Shuffle and take first 6
        const shuffled = imageFiles.sort(() => 0.5 - Math.random());
        const selectedImages = shuffled.slice(0, 6);
        
        res.json({ images: selectedImages });
    } catch (error) {
        console.error('Error getting random images:', error);
        res.status(500).json({ error: 'Failed to get random images' });
    }
});

// Get similar images based on selected image
app.post('/api/similar-images', (req, res) => {
    try {
        const { selectedImage, excludedImages = [] } = req.body;
        
        // Find the selected image features
        const selectedFeature = imageFeatures.find(f => f.filename === selectedImage);
        if (!selectedFeature) {
            return res.status(404).json({ error: 'Selected image not found in features' });
        }
        
        // Calculate similarities for all images except excluded ones
        const candidates = imageFeatures
            .filter(f => f.filename !== selectedImage && !excludedImages.includes(f.filename))
            .map(f => ({
                filename: f.filename,
                similarity: cosineSimilarity(selectedFeature.features, f.features)
            }))
            .sort((a, b) => b.similarity - a.similarity)
            .slice(0, 5);
        
        res.json({ similarImages: candidates });
    } catch (error) {
        console.error('Error finding similar images:', error);
        res.status(500).json({ error: 'Failed to find similar images' });
    }
});

// Process images to extract features
app.post('/api/process-images', (req, res) => {
    try {
        console.log('Processing images to extract features...');
        
        // Check if feature_extractor exists
        const extractorPath = process.platform === 'win32' ? './feature_extractor.exe' : './feature_extractor';
        
        if (!fs.existsSync(extractorPath)) {
            return res.status(500).json({ 
                error: 'Feature extractor not found. Please ensure the C++ application is built.' 
            });
        }
        
        // Run the C++ feature extractor
        exec(extractorPath, (error, stdout, stderr) => {
            if (error) {
                console.error('Error running feature extractor:', error);
                return res.status(500).json({ 
                    error: 'Failed to process images. Check if images exist in static folder.' 
                });
            }
            
            console.log('Feature extraction completed');
            console.log('STDOUT:', stdout);
            if (stderr) console.log('STDERR:', stderr);
            
            loadImageFeatures();
            res.json({ message: 'Images processed successfully' });
        });
    } catch (error) {
        console.error('Error processing images:', error);
        res.status(500).json({ error: 'Failed to process images' });
    }
});

// Health check endpoint
app.get('/health', (req, res) => {
    res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

// Serve the main HTML page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Handle 404
app.use((req, res) => {
    res.status(404).json({ error: 'Not found' });
});

// Error handling middleware
app.use((error, req, res, next) => {
    console.error('Server error:', error);
    res.status(500).json({ error: 'Internal server error' });
});

// Start server
app.listen(PORT, '0.0.0.0', () => {
    console.log(`Server running on port ${PORT}`);
    console.log(`Environment: ${process.env.NODE_ENV || 'development'}`);
    loadImageFeatures();
}); 