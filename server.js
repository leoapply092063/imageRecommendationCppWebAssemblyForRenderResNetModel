const express = require('express');
const path = require('path');
const app = express();
const PORT = process.env.PORT || 3000;

// Serve static files
app.use(express.static('public'));
app.use('/static', express.static('static'));
app.use('/', express.static(__dirname)); // Serve files from root directory

// Serve the main HTML page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// API endpoint to get image list
app.get('/api/images', (req, res) => {
    const fs = require('fs');
    const imageDir = path.join(__dirname, 'static');
    
    try {
        const files = fs.readdirSync(imageDir);
        const imageFiles = files.filter(file => {
            const ext = path.extname(file).toLowerCase();
            return ['.jpg', '.jpeg', '.png', '.bmp'].includes(ext);
        });
        
        res.json({
            images: imageFiles.map(file => `/static/${file}`),
            count: imageFiles.length
        });
    } catch (error) {
        res.status(500).json({ error: 'Failed to read image directory' });
    }
});

app.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
}); 