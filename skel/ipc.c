#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc.h"


int create_socket()
{
	return socket(AF_UNIX, SOCK_SEQPACKET, 0);	
}

int connect_socket(int fd)
{
	struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, SOCKET_NAME, sizeof(local.sun_path) - 1);

    return connect(fd, (const struct sockaddr *) &local, sizeof(local));
}

ssize_t send_socket(int fd, const char *buf, size_t len)
{
	return send(fd, buf, len, 0);
}

ssize_t recv_socket(int fd, char *buf, size_t len)
{
	return recv(fd, buf, len, 0);
}

void close_socket(int fd)
{
	close(fd);
}
