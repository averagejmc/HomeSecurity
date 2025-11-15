let armed = false;

function toggleSecurity() {
  armed = !armed;
  // Update button
  const btn = document.getElementById("arm-btn");
  btn.innerText = armed ? "Disarm System" : "Arm System";
  btn.style.backgroundColor = armed ? "#dc3545" : "#007bff"; // red when armed, blue when disarmed

  // Update status text
  const status = document.getElementById("system-status");
  status.innerHTML = armed
    ? "System is <span style='color:red'>ARMED</span>"
    : "System is <span style='color:green'>DISARMED</span>";

  // Send armed status to backend
  fetch("/toggle_security", {
    method: "POST",
    body: JSON.stringify({ armed: armed }),
    headers: { "Content-Type": "application/json" },
  })
    .then((response) => {
      if (!response.ok) {
        console.error("Failed to toggle security status.");
      }
    })
    .catch((error) => {
      console.error("Error sending toggle request:", error);
    });
}

var socket = io();
socket.on("mqtt_message", function (msg) {
  let topic = msg.data.split(":")[0];
  let payload = msg.data.split(":")[1].trim();

  if (topic === "home/motion") {
    document.getElementById("motion").innerText = "Motion: " + payload;
    document.getElementById("motion").className =
      "sensor-box " +
      (payload.includes("detected") ? "motion-detected" : "motion-clear");
  } else if (topic === "home/door") {
    document.getElementById("door").innerText = "Door: " + payload;
    document.getElementById("door").className =
      "sensor-box " + (payload.includes("open") ? "door-open" : "door-closed");
  } else if (topic === "home/rfid") {
    document.getElementById("rfid").innerText = "Last RFID: " + payload;
    document.getElementById("rfid").className = "sensor-box rfid-active";
  }
});

function showTab(tabId, btn) {
  document
    .querySelectorAll(".tab-content")
    .forEach((el) => el.classList.remove("active"));
  document
    .querySelectorAll(".tab-btn")
    .forEach((el) => el.classList.remove("active"));

  const tab = document.getElementById(tabId);
  if (tab) tab.classList.add("active");

  if (btn) btn.classList.add("active");

  if (tabId === "logs") {
    loadLogs();
  }
}

function loadLogs() {
  fetch("/logs")
    .then((res) => res.json())
    .then((data) => {
      const container = document.getElementById("log-container");
      container.innerHTML = data.logs.length
        ? data.logs.map((log) => `<p>${log}</p>`).join("")
        : "<p>No logs available.</p>";
    })
    .catch(() => {
      document.getElementById("log-container").innerText =
        "Failed to load logs.";
    });
}
