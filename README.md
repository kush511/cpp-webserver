# ⚙️ C++ HTTP Server using WinSock

A basic HTTP server built from scratch in C++ using raw sockets and the Windows WinSock API.

This project serves simple HTML/CSS content from a `www/` directory — just enough to question your life choices and consider switching to Python 😅.

---

## 🧠 What I Learned

- Handling low-level networking with C++ and WinSock
- Parsing basic HTTP requests manually
- Serving static files like `.html`, `.css`, `.png`
- That C++ for backend development is... **not my cup of tea**

> "Tried C++ for backend. Built a basic HTTP server using raw sockets and WinSock.  
> Now I understand why devs switch to Python and never look back.  
> Respect to those still sipping this bitter brew."

---

## 🛠️ How to Run (Windows only)

1. Clone the repo:
```bash
git clone https://github.com/kush511/cpp-webserver.git
cd cpp-webserver

Compile the server (Make sure you have a C++ compiler installed, e.g., g++):
g++ server.cpp http_tcpServer_windows.cpp -o server.exe -lws2_32

Run the server:
./server.exe

Open your browser and visit:
http://localhost:8080



Project structure
cpp-webserver/
├── server.cpp                 # Entry point
├── http_tcpServer_windows.*  # Core logic for socket handling
├── www/                      # Static files served by server
│   ├── index.html
│   ├── about.html
│   ├── styles.css
│   └── logo.png


