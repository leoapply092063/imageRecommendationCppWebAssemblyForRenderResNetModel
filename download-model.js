const https = require('https');
const fs = require('fs');
const path = require('path');

const MODEL_URL = 'https://github.com/onnx/models/raw/main/vision/classification/resnet/model/resnet50-v1-7.onnx';
const MODEL_PATH = path.join(__dirname, 'resnet50-v1-7.onnx');

// Create root directory if it doesn't exist (should already exist)
console.log('Model will be downloaded to root directory');

console.log('Downloading ResNet-50 model...');
console.log('URL:', MODEL_URL);
console.log('Path:', MODEL_PATH);

const file = fs.createWriteStream(MODEL_PATH);

https.get(MODEL_URL, (response) => {
    if (response.statusCode === 302 || response.statusCode === 301) {
        // Handle redirect
        console.log('Following redirect...');
        https.get(response.headers.location, (redirectResponse) => {
            redirectResponse.pipe(file);
            file.on('finish', () => {
                file.close();
                console.log('Model downloaded successfully!');
                console.log('File size:', fs.statSync(MODEL_PATH).size, 'bytes');
            });
        }).on('error', (err) => {
            fs.unlink(MODEL_PATH, () => {}); // Delete the file async
            console.error('Error downloading model:', err.message);
        });
    } else {
        response.pipe(file);
        file.on('finish', () => {
            file.close();
            console.log('Model downloaded successfully!');
            console.log('File size:', fs.statSync(MODEL_PATH).size, 'bytes');
        });
    }
}).on('error', (err) => {
    fs.unlink(MODEL_PATH, () => {}); // Delete the file async
    console.error('Error downloading model:', err.message);
}); 