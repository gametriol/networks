import express from "express";
import http from "http";
import { Server } from "socket.io";
import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(express.static(path.join(__dirname, "public")));

let users = {}; // socket.id -> username

io.on("connection", (socket) => {
  console.log("A user connected:", socket.id);

  // When a user joins with a name
  socket.on("join", (username) => {
    users[socket.id] = username;
    io.emit("user-list", Object.values(users));
    io.emit("message", { sender: "Server", text: `${username} joined the chat.` });
  });

  // When someone sends a message
  socket.on("message", (text) => {
    const sender = users[socket.id];
    io.emit("message", { sender, text });
  });

  // When someone disconnects
  socket.on("disconnect", () => {
    const username = users[socket.id];
    delete users[socket.id];
    io.emit("user-list", Object.values(users));
    io.emit("message", { sender: "Server", text: `${username} left the chat.` });
  });
});

const PORT = 5000;
server.listen(PORT, () => console.log(`Server running on http://localhost:${PORT}`));


import os from "os";
import dns from "dns";
import * as gateway from "default-gateway";

app.get("/api/network-info", async (req, res) => {
  try {
    const interfaces = os.networkInterfaces();
    const networkData = [];

    Object.keys(interfaces).forEach((name) => {
      interfaces[name].forEach((iface) => {
        if (iface.family === "IPv4" && !iface.internal) {
          networkData.push({
            interface: name,
            ip: iface.address,
            mac: iface.mac,
            netmask: iface.netmask,
          });
        }
      });
    });

    const hostname = os.hostname();
    const dnsServers = dns.getServers();

    let defaultGateway = "Unavailable";
    try {
      const result = await gateway.v4();
      defaultGateway = result.gateway;
    } catch (err) {
      console.warn("⚠️ Could not detect gateway:", err.message);
    }

    res.json({
      hostname,
      interfaces: networkData,
      dnsServers,
      defaultGateway,
    });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});
