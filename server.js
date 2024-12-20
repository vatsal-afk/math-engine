const express = require('express');
const path = require('path');

const app = express();
const port = 8080;

// Serve static files from the current directory
app.use(express.static(path.join(__dirname, '/')));

// Serve the HTML file by default
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'window.html'));
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
