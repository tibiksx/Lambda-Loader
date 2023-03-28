#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include "ipc.h"
#include "server.h"
#include <errno.h>

#ifndef OUTPUTFILE_TEMPLATE
#define OUTPUTFILE_TEMPLATE "../checker/output/out-XXXXXX"
#endif

#define BUF_SIZE 1024

static int lib_prehooks(struct lib *lib)
{
	char *template = (char *) malloc(strlen(OUTPUTFILE_TEMPLATE) * sizeof(char));
	strcpy(template, OUTPUTFILE_TEMPLATE);
        int new_fd = mkstemp(template);

        lib->outputfile = template;
        close(new_fd);

	return 0;
}

static int lib_load(struct lib *lib)
{
	return 0;
}

static int lib_execute(struct lib *lib)
{
	lib->handle = dlopen(lib->libname,RTLD_LAZY);

        if (!lib->handle) {
                freopen(lib->outputfile, "w", stdout);
		printf("Error: %s %s %s could not be executed.\n", lib->libname, lib->funcname, lib->filename);
		freopen("/dev/tty", "w", stdout);
                return 0;
        }

	freopen(lib->outputfile, "w", stdout);
	if (strlen(lib->funcname) == 0) {	
		lib->run = dlsym(lib->handle, "run");
		lib->run();
	} else {	
	
		lib->p_run = dlsym(lib->handle, lib->funcname);
		
		if (lib->p_run != NULL)	
			lib->p_run(lib->filename);
		else {
			freopen(lib->outputfile, "w", stdout);
                	printf("Error: %s %s %s could not be executed.\n", lib->libname, lib->funcname, lib->filename);
                	freopen("/dev/tty", "w", stdout);
		}
	}
	
	freopen("/dev/tty", "w", stdout);
	return 0;
}

static int lib_close(struct lib *lib)
{
	return 0;
}

static int lib_posthooks(struct lib *lib)
{
	return 0;
}

static int lib_run(struct lib *lib)
{
	int err;

	err = lib_prehooks(lib);
	if (err)
		return err;

	err = lib_load(lib);
	if (err)
		return err;

	err = lib_execute(lib);
	if (err)
		return err;

	err = lib_close(lib);
	if (err)
		return err;

	return lib_posthooks(lib);
}

static int parse_command(const char *buf, char *name, char *func, char *params)
{
	return sscanf(buf, "%s %s %s", name, func, params);
}

int main(void)
{
	int ret;
	struct lib lib;
	char buf[BUF_SIZE];

	/* TODO - Implement server connection */
	int connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (connection_socket == -1) {
		perror("socket");
		exit(-1);
	}

	struct sockaddr_un local;
	local.sun_family = AF_UNIX;
	strncpy(local.sun_path, SOCKET_NAME, sizeof(local.sun_path) - 1);
	
	ret = bind(connection_socket, (const struct sockaddr *) &local, sizeof(local));
	if (ret == -1) {
		perror("bind");
		exit(-1);
	}

	ret = listen(connection_socket, MAX_CLIENTS);
	if (ret == -1) {
		perror("listen");
		exit(-1);
	}
	
	while(1) {
		int data_socket = accept(connection_socket, NULL, NULL);
		if (data_socket == -1) {
			perror("accept");
			exit(-1);
		}

		/* TODO - get message from client */
		int bytes = recv_socket(data_socket, buf, BUF_SIZE);
		buf[bytes] = 0;

		/* TODO - parse message with parse_command and populate lib */
		lib.libname = (char *) calloc(BUF_SIZE, 1);
		lib.funcname = (char *) calloc(BUF_SIZE, 1);
		lib.filename = (char *) calloc(BUF_SIZE, 1);
		parse_command(buf, lib.libname, lib.funcname, lib.filename);

		/* TODO - handle request from client */
		ret = lib_run(&lib);
		
		if (ret != -1)
			bytes = send_socket(data_socket, lib.outputfile, strlen(lib.outputfile));
		close_socket(data_socket);
	}

	close_socket(connection_socket);
	unlink(SOCKET_NAME);
	return 0;
}