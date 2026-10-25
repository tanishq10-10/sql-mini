#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  std::string mode = "";
  int port = 0;

  // Start from 1 because argv[0] is the program name (e.g., "./node")
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "--mode" && i + 1 < argc) {
      // TODO: grab the next element (argv[i+1]) and save it to 'mode'
      // Hint: increment 'i' here so the loop skips the value on the next
      // iteration
    } else if (arg == "--port" && i + 1 < argc) {
      // TODO: grab the next element, convert it to an integer (using
      // std::stoi), and save to 'port' Hint: increment 'i' here as well
    }
  }

  // TODO: Add a check to ensure both mode and port were actually provided.
  // If they are missing, print an error message and exit(1).

  std::cout << "Starting as " << mode << " on port " << port << "\n";

  return 0;
}