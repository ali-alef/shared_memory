# Shared Memory

## Description

This project implements a multi-threaded, ZeroMQ-based messaging system with shared memory for inter-thread communication. It handles external requests, processes them, and manages a shared lock mechanism.

## Features

- **ZeroMQ Integration**: Utilizes ZeroMQ for message passing between different components.
- **Multi-threading**: Handles different tasks concurrently using multiple threads.
- **Shared Memory**: Employs shared memory for communication between threads.
- **JSON Processing**: Uses JSON for data serialization and deserialization.
- **Timed Set**: Implements a timed set for managing request deduplication and expiration.
- **Environment Configuration**: Reads configuration settings from environment-specific files.

## File Structure

- `main.cpp`: Contains the main logic for handling requests, processing shared memory, and managing locks.
- `settings.cpp`: Reads environment variables from configuration files and stores them in a map.
- `utils.cpp`: Provides utility functions and initializes shared memory functions.
- `timed_set.cpp`: Implements a timed set data structure for managing requests.
- `timed_set.f`: Header file for the timed set implementation.

## Dependencies

- [ZeroMQ](https://zeromq.org/) library for messaging.
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing and serialization.
- Standard C++ libraries (`<thread>`, `<mutex>`, `<chrono>`, etc.).

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/ali-alef/shared_memory.git
   cd shared_memory
   ```

2. Install dependencies:
   - Ensure ZeroMQ is installed on your system.
   - Ensure the `nlohmann/json` library is available.

3. Build the project:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

1. Set the required environment variables:
   ```sh
   export ENV=local  # or other environment name (default is local)
   ```

2. Run the application:
   * compile shared_memory.c file by running:
       ```sh 
       gcc -shared -fPIC -o ./shared/shared_memory.so ./shared/shared_memory.c
       ```
   * clear the shared memory by running `clear_shared_memory.py` file.
   * Compile the `malek_taj` main.cpp by running:
     ```sh
     g++ main.cpp timed_set.cpp utils.cpp settings.cpp -o main -lzmq
     ```
   * Run the compiled `malek_taj` file:
     ```sh
     ./main
     ```
   * Run the Python service main file `main.py` that is in the `python_service` directory.
   * Run the Python client main file `main.py` that is in the `python_client` directory.
   * **Note:** You should run each file from its base directory. For example, to run the compiled `main` file of `malek_taj`, you should be in the `malek_taj` directory.

   **Warning: Ensure you are in the correct base directory before running each file.**
 

## Detailed Description

### Main Components

1. **`get_outer_requests` Function (in `main.cpp`)**
   - Listens for incoming requests on a specified ZeroMQ port.
   - Writes received data to shared memory.

2. **`send_outer_requests` Function (in `main.cpp`)**
   - Sends requests to an external service and handles the response.
   - Returns true if the response is "200 OK".

3. **`handle_shared_requests` Function (in `main.cpp`)**
   - Reads data from shared memory and processes it.
   - Uses a timed set to ensure request deduplication.
   - Publishes processed data to a ZeroMQ publisher socket.

4. **`handle_shared_lock` Function (in `main.cpp`)**
   - Manages locking mechanism using shared memory.
   - Listens for lock and unlock requests on a specified ZeroMQ port.

5. **`read_env` Function (in `settings.cpp`)**
   - Reads environment variables from a file specific to the current environment.
   - Stores the variables in a map for use throughout the application.

6. **`initialize_shared_object` Function (in `utils.cpp`)**
   - Loads and initializes shared memory functions from a dynamic library.

7. **`to_lower` Function (in `utils.cpp`)**
   - Converts a string to lowercase.

8. **`split_string` Function (in `utils.cpp`)**
   - Splits a string into tokens based on a delimiter.

9. **`first_empty_spot` Function (in `utils.cpp`)**
   - Finds the first empty spot in the shared memory.

10. **`TimedSet` Class (in `timed_set.cpp`)**
    - **`add` Function**: Adds an item to the timed set with an expiration duration.
    - **`cleanupExpiredItems` Function**: Removes expired items from the set.
    - **`contains` Function**: Checks if an item is in the set.
    - **`empty` Function**: Checks if the set is empty.

11. **`main` Function (in `main.cpp`)**
    - Initializes the environment and shared memory.
    - Starts the necessary threads for handling requests and locks.

## Contributing

Contributions are welcome! Please submit a pull request or open an issue to discuss your ideas.

## License

This project is licensed under the MIT License.
