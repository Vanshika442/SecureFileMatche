document.addEventListener('DOMContentLoaded', () => {
    const fileInput = document.getElementById('fileInput');
    const fileBtn = document.querySelector('.file-custom-btn');
    const fileName = document.getElementById('fileName');
    const searchBtn = document.getElementById('searchBtn');
    const resultSection = document.getElementById('resultSection');

    // Generator Logic (Only if element exists)
    const genBtn = document.getElementById('genBtn');
    if (genBtn) {
        genBtn.addEventListener('click', () => {
            const plain = document.getElementById('plainInput').value;
            if (!plain) return alert("Enter text first!");

            // Encrypt (Caesar +1) to match C++ Backend
            let encrypted = "";
            for (let i = 0; i < plain.length; i++) {
                encrypted += String.fromCharCode(plain.charCodeAt(i) + 1);
            }

            // Download
            const blob = new Blob([encrypted], { type: 'text/plain' });
            const url = window.URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = "my_secret_file.txt";
            a.click();
            window.URL.revokeObjectURL(url);
        });
    }

    // File Input Custom Trigger
    fileBtn.addEventListener('click', () => fileInput.click());

    fileInput.addEventListener('change', (e) => {
        if (e.target.files.length > 0) {
            fileName.textContent = e.target.files[0].name;
        } else {
            fileName.textContent = "No file chosen";
        }
    });

    searchBtn.addEventListener('click', async () => {
        const file = fileInput.files[0];
        const pattern = document.getElementById('patternInput').value;
        const keyVal = document.getElementById('keyInput').value;
        const algo = document.getElementById('algoSelect').value;

        if (!file || !pattern) {
            alert("Please select a file and enter a search pattern.");
            return;
        }

        searchBtn.disabled = true;
        searchBtn.innerHTML = "Searching...";

        try {
            const text = await readFile(file);

            const payload = {
                text: text,
                pattern: pattern,
                algorithm: algo,
                key: keyVal || "1"
            };

            const response = await fetch('http://localhost:8080/search', {
                method: 'POST',
                body: JSON.stringify(payload)
            });

            if (!response.ok) throw new Error("Server Error");

            const data = await response.json();
            displayResult(data, algo);

        } catch (err) {
            console.error(err);
            alert("Error connecting to backend server. Make sure C++ app is running.");
        } finally {
            searchBtn.disabled = false;
            searchBtn.innerHTML = "Start Search";
        }
    });

    function readFile(file) {
        return new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = (e) => resolve(e.target.result);
            reader.onerror = (e) => reject(e);
            reader.readAsText(file);
        });
    }

    function displayResult(data, algo) {
        resultSection.style.display = 'block';

        const statusEl = document.getElementById('resFound');
        if (data.found) {
            statusEl.textContent = "Match Found";
            statusEl.className = "value found";
        } else {
            statusEl.textContent = "Not Found";
            statusEl.className = "value not-found";
        }

        document.getElementById('resIndex').textContent = data.index !== -1 ? data.index : "-";
        document.getElementById('resTime').textContent = data.time_ms.toFixed(3) + " ms";
        document.getElementById('resAlgo').textContent = algo.toUpperCase();
    }
});
