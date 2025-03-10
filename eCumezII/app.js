// Import Firebase modules
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-app.js";
import { getDatabase, ref, onValue, set } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-database.js";

// Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyBnAyceu8ya2BEGbuGZBSYlZRUWjZtZ9sk",
    authDomain: "ecumez.firebaseapp.com",
    databaseURL: "https://ecumez-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "ecumez",
    storageBucket: "ecumez.appspot.com",
    messagingSenderId: "317326073959",
    appId: "1:317326073959:web:e2fdbc13b78af10f3474c2",
    measurementId: "G-BV1G8X01SB"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// HTML elements
const temperatureElement = document.getElementById("temperature");
const humidityElement = document.getElementById("humidity");
const openDoorButton = document.getElementById("open-door");
const closeDoorButton = document.getElementById("close-door");

// Chart.js setup
const ctx = document.getElementById("sensorChart").getContext("2d");
const chartData = {
    labels: [],
    datasets: [
        {
            label: "Temperature (°C)",
            data: [],
            borderColor: "red",
            fill: false
        },
        {
            label: "Humidity (%)",
            data: [],
            borderColor: "blue",
            fill: false
        }
    ]
};
const sensorChart = new Chart(ctx, {
    type: "line",
    data: chartData,
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: "Time" } },
            y: { title: { display: true, text: "Value" } }
        }
    }
});

// Realtime sensor data
const sensorsRef = ref(database, "senzori");
onValue(sensorsRef, (snapshot) => {
    const data = snapshot.val();
    if (data) {
        temperatureElement.textContent = data.temperatura || "--";
        humidityElement.textContent = data.vlaznost || "--";

        // Update chart
        const now = new Date().toLocaleTimeString();
        chartData.labels.push(now);
        chartData.datasets[0].data.push(data.temperatura);
        chartData.datasets[1].data.push(data.vlaznost);
        if (chartData.labels.length > 10) chartData.labels.shift();
        if (chartData.datasets[0].data.length > 10) chartData.datasets[0].data.shift();
        if (chartData.datasets[1].data.length > 10) chartData.datasets[1].data.shift();
        sensorChart.update();
    }
});

// Door control
function controlDoor(action) {
    set(ref(database, "KontrolaVrata"), action)
        .then(() => console.log(`Door ${action} command sent successfully`))
        .catch((error) => console.error("Error controlling door:", error));
}

openDoorButton.addEventListener("click", () => controlDoor("otvori"));
closeDoorButton.addEventListener("click", () => controlDoor("zatvori"));
