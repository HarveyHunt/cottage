#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/howm"
#define BUF_SIZE 1024

enum ipc_errs { IPC_ERR_NONE, IPC_ERR_SYNTAX, IPC_ERR_ALLOC, IPC_ERR_NO_CMD, IPC_ERR_TOO_MANY_ARGS,
	IPC_ERR_TOO_FEW_ARGS, IPC_ERR_ARG_NOT_INT, IPC_ERR_ARG_TOO_LARGE };

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sock, len = 0, off = 0, n = 0;
	char data[BUF_SIZE];
	int ret, rec;

	if (argc < 2) {
		printf("usage: cottage <command> [<args>]\n");
		return EXIT_FAILURE;
	}

	argc--, argv++;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCK_PATH);
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock == -1) {
		perror("Socket error");
		exit(EXIT_FAILURE);
	}

	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	for (; argc > 0 && sizeof(data) - off > 0; off += n, argc--, argv++) {
		n = snprintf(data + off, sizeof(data) - off, "%s%c", *argv, 0);
		len += n;
	}

	if (write(sock, data, len) == -1) {
		perror("Failed to send data");
		exit(EXIT_FAILURE);
	}

	rec = read(sock, &ret, sizeof(ret));
	if (rec == -1)
		perror("Failed to receive response");

	switch (ret) {
	case IPC_ERR_SYNTAX:
		printf("Invalid syntax.\n");
		break;
	case IPC_ERR_ALLOC:
		printf("Couldn't allocate memory to store args.\n");
		break;
	case IPC_ERR_NO_CMD:
		printf("No such command.\n");
		break;
	case IPC_ERR_TOO_MANY_ARGS:
		printf("Too many args.\n");
		break;
	case IPC_ERR_TOO_FEW_ARGS:
		printf("Too few args.\n");
		break;
	case IPC_ERR_ARG_NOT_INT:
		printf("Argument wasn't an int\n");
		break;
	case IPC_ERR_ARG_TOO_LARGE:
		printf("Argument was too large\n");
		break;
	}

	if (close(sock) == -1) {
		printf("Failed to close socket.\n");
		exit(EXIT_FAILURE);
	}
	return ret;
}
