const socket = io();
let username = "";

while (!username) {
  username = prompt("Enter your name:");
}
socket.emit("join", username);

const chatBox = document.getElementById("chat-box");
const usersDiv = document.getElementById("users");
const input = document.getElementById("msg");
const sendBtn = document.getElementById("send");

sendBtn.onclick = () => {
  const text = input.value.trim();
  if (text) {
    socket.emit("message", text);
    input.value = "";
  }
};

socket.on("message", (data) => {
  const msg = document.createElement("div");
  msg.classList.add("msg");
  if (data.sender === "Server") msg.classList.add("server");
  msg.textContent = `${data.sender}: ${data.text}`;
  chatBox.appendChild(msg);
  chatBox.scrollTop = chatBox.scrollHeight;
});

socket.on("user-list", (list) => {
  usersDiv.textContent = `Active users: ${list.length} (${list.join(", ")})`;
});

const detectBtn = document.getElementById("detect");
const infoBox = document.getElementById("network-info");

detectBtn.onclick = async () => {
  try {
    const res = await fetch("/api/network-info");
    const data = await res.json();
    infoBox.style.display = "block";
    infoBox.textContent = JSON.stringify(data, null, 2);
  } catch (err) {
    infoBox.style.display = "block";
    infoBox.textContent = "Error fetching network info: " + err;
  }
};

