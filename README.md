# CN_Assignment_2: Socket programming with performance analysis

#### Members
- Anushka Srivastava (2022086)
- Apaar (2022089)

In this assignment, we set up a TCP connection between client and server program. The client requests information about top CPU consuming processes from the server and the server retrieves and sends that information to the client.

We implement three server-client models in this assignment:
1) Single-threaded TCP client-server
2) Concurrent TCP client-server
3) TCP client-server using “select”

We analyse the models using `perf` for performance analysis and compiled our findings in the report.

## Project Repository Structure

### Q1
- `client.c`: Handles the client end of the connection.
- `handleClient.c`: Helper function for server which handles client requests.
- `handleClient.h`: Header file for `handleClient.c`
- `Makefile`: Makefile to compile and clean executables in the `Q1` directory.
- `server.c`: Handles the server end of the connection.

### Q2
- #### Concurrent TCP
  File structure for concurrent TCP client-server
  + `client.c`: Handles the client end of the connection.
  + `handleClient.c`: Helper function for server which handles client requests.
  + `handleClient.h`: Header file for `handleClient.c`
  + `Makefile`: Makefile to compile and clean executables in the `Q2/'Concurrent TCP'` directory.
  +`server.c`: Handles the server end of the connection.

- #### Select TCP
  File structure for concurrent TCP client-server
  + `client.c`: Handles the client end of the connection.
  + `handleClient.c`: Helper function for server which handles client requests.
  + `handleClient.h`: Header file for `handleClient.c`
  + `Makefile`: Makefile to compile and clean executables in the `Q2/'Select TCP'` directory.
  +`server.c`: Handles the server end of the connection.

- #### Single Threaded TCP
  File structure for concurrent TCP client-server
  + `client.c`: Handles the client end of the connection.
  + `handleClient.c`: Helper function for server which handles client requests.
  + `handleClient.h`: Header file for `handleClient.c`
  + `Makefile`: Makefile to compile and clean executables in the `Q2/'Single Threaded TCP'` directory.
  +`server.c`: Handles the server end of the connection.

- `Makefile`: Makefile to compile and clean executables in the sub directories of `Q2` directory.

### Root directory of the repository

- `.gitignore`: Github utility to remove tracking of perf files and .exe files
- `connectToServer.c`: Utility function for the clients to connect to the server and terminate the connection.
- `connectToServer.h`: Header file for `connectToServer.c`
- `cpu.c`: Utility function to extract top CPU consuming processes from the system.
- `cpu.h`: Header file for `cpu.c`.
- `Makefile`: Makefile to compile and clean executables in the root directory and its subdirectories.
- `README.md`: This file.
- `report.pdf`: Analysis Report of the project.
- `structures.h`: Definition of structs used in the assignment.