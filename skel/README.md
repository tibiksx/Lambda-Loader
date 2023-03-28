The ipc.c file contains the implementation of the functions used by both the client and the server.

create_socket returns a sequential unix socket.

connect_socket creates a local sockaddr_un filled with the path to the socket, which is later
passed to the connect function with file desc given as a parameter.

send_socket calls the send function on files desc, buf and buf size given as parameters
recv_socket calls the recv function on files desc, buf and buf size given as parameters

close_socket calls close on the given descriptor table.

server.c is the server implementation. The first step is creating the connection socket
to the client and binding to the localhost. The server is listening on the port and waiting for
an upcoming connection. When the accept is validated (it has connected to the client), bytes are 
received from the client and are parsed using the parse_Command function. The passing lib_run is 
called through a suite of helper functions.

Prehooks initializes a template buffer to be used by mkstemp to generate a name
random output file.
Execute loads the dynamic library into memory. If the library is not a valid one,
the handler will detect it and it will be able to display an error message.
Otherwise, the standard output is redirected to the output file, it is put in the pointer to the function
run or p_run address of the function given and subsequently called. The output is redirected to the
standard output. Good follow-up: multithreading.