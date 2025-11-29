let armed = false;

// arm the system
function toggleSecurity() {
  armed = !armed;

  const btn = document.getElementById("arm-btn");
  btn.innerText = armed ? "Disarm System" : "Arm System";
  btn.style.backgroundColor = armed ? "#dc3545" : "#007bff"; // red = armed, blue = disarmed

  const status = document.getElementById("system-status");
  status.innerHTML = armed
    ? "System is <span style='color:red'>ARMED</span>"
    : "System is <span style='color:green'>DISARMED</span>";

  fetch("/toggle_security", {
    method: "POST",
    body: JSON.stringify({ armed }),
    headers: { "Content-Type": "application/json" },
  }).catch((err) => console.error("Error sending toggle request:", err));
}

// switch tabs
function showTab(tabId) {
  document
    .querySelectorAll("#auto, #maintenance, #sleep")
    .forEach((el) => el.classList.remove("active"));

  document
    .querySelectorAll(".tab-btn")
    .forEach((el) => el.classList.remove("active"));

  const tab = document.getElementById(tabId);
  if (tab) tab.classList.add("active");

  const btn = document.querySelector(`.tab-btn[data-tab="${tabId}"]`);
  if (btn) btn.classList.add("active");

  if (tabId === "auto") loadLogs();
}

// load logs
async function loadLogs() {
  const res = await fetch("/logs");
  const data = await res.json();

  const container = document.getElementById("log-container");
  container.innerHTML = ""; // clear previous

  if (!data.logs.length) {
    container.innerHTML = "<p>No logs yet.</p>";
    return;
  }

  const list = document.createElement("ul");
  list.classList.add("log-list");

  data.logs.forEach((log) => {
    const item = document.createElement("li");
    item.classList.add("log-item");
    item.innerHTML = `
      <strong>${log.timestamp}</strong><br>
      Armed: ${log.armed}<br>
      Motion: ${log.motion}<br>
      Door: ${log.door}<br>
      RFID: ${log.rfid}<br>
      Door Open Time: ${log.door_open_time_seconds}s
    `;
    list.appendChild(item);
  });

  container.appendChild(list);
}

// listener for mqtt
var socket = io();
socket.on("mqtt_message", function (msg) {
  let [topic, payload] = msg.data.split(":");
  payload = payload.trim();

  if (topic === "home/motion") {
    const el = document.getElementById("motion");
    el.innerText = "Motion: " + payload;
    el.className =
      "sensor-box " +
      (payload.includes("detected") ? "motion-detected" : "motion-clear");
  } else if (topic === "home/door") {
    const el = document.getElementById("door");
    el.innerText = "Door: " + payload;
    el.className =
      "sensor-box " + (payload.includes("open") ? "door-open" : "door-closed");
  } else if (topic === "home/rfid") {
    const el = document.getElementById("rfid");
    el.innerText = "Last RFID: " + payload;
    el.className = "sensor-box rfid-active";
  }
});

// for mode switching
// socket.on("mode_update", function(data) {
//   const mode = data.mode; // 'auto', 'maintenance', 'sleep'
//   showModeTab(mode);  // a new function to switch tabs
// });

document.addEventListener("DOMContentLoaded", () => {
  // Arm button listener
  document.getElementById("arm-btn").addEventListener("click", toggleSecurity);

  // Tab buttons listener using delegation
  document.querySelectorAll(".tab-btn").forEach((btn) => {
    btn.addEventListener("click", () => showTab(btn.dataset.tab));
  });

  showTab("auto"); 
});