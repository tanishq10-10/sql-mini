#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <unistd.h>

#endif

int main(int argc, char *argv[]) {
  const char *mode = NULL;
  int port = 0;

  // Start from 1 because argv[0] is the program name (e.g., "./node")
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
      mode = argv[++i]; // grab next element, skip it on next iteration
    } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
      port = atoi(argv[++i]);
    }
  }

  // Validate: both mode and port must be provided
  if (mode == NULL || port == 0) {
    fprintf(stderr, "Usage: %s --mode <leader|follower> --port <number>\n",
            argv[0]);
    return 1;
  }

  printf("Starting as %s on port %d\n", mode, port);

  // ── 1. Winsock init (Windows only) ───────────────────────────────────
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
    return 1;
  }
#endif

  // ── 2. socket() ─────────────────────────────────────────────────────
  //    AF_INET     = IPv4
  //    SOCK_STREAM = TCP (reliable, ordered byte stream)
  //    0           = let the OS pick the protocol (TCP for SOCK_STREAM)
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  // Allow address reuse so we can restart quickly without "Address already in
  // use"
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt,
             sizeof(opt));

  // ── 3. bind() ───────────────────────────────────────────────────────
  //    Tell the OS: "this socket listens on <port> on any network interface"
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));    // zero out the struct
  addr.sin_family = AF_INET;         // IPv4
  addr.sin_addr.s_addr = INADDR_ANY; // bind to 0.0.0.0 (all interfaces)
  addr.sin_port = htons(port);       // convert port to network byte order

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return 1;
  }

  // ── 4. listen() ─────────────────────────────────────────────────────
  //    Mark the socket as passive — it will accept() incoming connections.
  //    The backlog (128) is how many pending connections the OS will queue.
  if (listen(server_fd, 128) < 0) {
    perror("listen");
    return 1;
  }

  printf("Listening on port %d...\n", port);

  // ── 5. accept() loop ─────────────────────────────────────────────────────
  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // accept() blocks execution until someone connects
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
      perror("accept failed");
      continue; // skip to the next connection
    }

    printf("New client connected!\n");

    // Send a test message to prove two-way connection works
    const char *msg = "Hello from the node!\n";
    send(client_fd, msg, strlen(msg), 0);

// Close the client connection (cross-platform)
#ifdef _WIN32
    closesocket(client_fd);
#else
    close(client_fd);
#endif
  }

  // ── Cleanup ─────────────────────────────────────────────────────────
#ifdef _WIN32
  closesocket(server_fd);
  WSACleanup();
#else
  close(server_fd);
#endif

  return 0;
}