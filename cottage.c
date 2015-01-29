#include <ctype.h>
#include <getopt.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define DEF_SOCK_PATH "/tmp/howm"
#define ENV_SOCK_VAR "HOWM_SOCK"
#define BUF_SIZE 1024
#define VERSION "0.3-pre"

/* The errors (or lack of) that could be sent back by howm. */
enum ipc_errs { IPC_ERR_NONE, IPC_ERR_SYNTAX, IPC_ERR_ALLOC, IPC_ERR_NO_FUNC,
	IPC_ERR_TOO_MANY_ARGS, IPC_ERR_TOO_FEW_ARGS, IPC_ERR_ARG_NOT_INT,
	IPC_ERR_ARG_NOT_BOOL, IPC_ERR_ARG_TOO_LARGE, IPC_ERR_ARG_TOO_SMALL,
	IPC_ERR_UNKNOWN_TYPE };
enum msg_type { MSG_FUNCTION = 1, MSG_CONFIG };

static void usage(void);

/* Send a command to howm and wait for its reply. */
int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sock, len = 0, off = 0, n = 0;
	char data[BUF_SIZE];
	char *sp;
	char sock_path[256];
	int ret, rec, ch, type = 0;

	if (argc < 2)
		usage();

	while ((ch = getopt(argc, argv, "vcf")) != -1 && !type) {
		switch (ch) {
		case 'c':
			type = MSG_CONFIG;
			break;
		case 'f':
			type = MSG_FUNCTION;
			break;
		case 'v':
			printf("%s\n", VERSION);
			exit(EXIT_SUCCESS);
		default:
			usage();
		}
	}

	if (!type)
		usage();

	argc -= 2;
	argv += 2;

	if ((sp = getenv(ENV_SOCK_VAR)) != NULL)
		snprintf(sock_path, sizeof(sock_path), "%s", sp);
	else
		snprintf(sock_path, sizeof(sock_path), "%s", DEF_SOCK_PATH);

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", sock_path);
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock == -1) {
		perror("Socket error");
		exit(EXIT_FAILURE);
	}

	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	n = snprintf(data, sizeof(data) - (2 * sizeof(char)), "%c%c", type, 0);
	len += n;
	off += n;
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
		fprintf(stderr, "Invalid syntax.\n");
		break;
	case IPC_ERR_ALLOC:
		fprintf(stderr, "Couldn't allocate memory to store args.\n");
		break;
	case IPC_ERR_NO_FUNC:
		fprintf(stderr, "No such function.\n");
		break;
	case IPC_ERR_TOO_MANY_ARGS:
		fprintf(stderr, "Too many args.\n");
		break;
	case IPC_ERR_TOO_FEW_ARGS:
		fprintf(stderr, "Too few args.\n");
		break;
	case IPC_ERR_ARG_NOT_INT:
		fprintf(stderr, "Argument wasn't an int\n");
		break;
	case IPC_ERR_ARG_NOT_BOOL:
		fprintf(stderr, "Argument wasn't a bool\n");
		break;
	case IPC_ERR_ARG_TOO_LARGE:
		fprintf(stderr, "Argument was too large\n");
		break;
	case IPC_ERR_ARG_TOO_SMALL:
		fprintf(stderr, "Argument was too small\n");
		break;
	case IPC_ERR_UNKNOWN_TYPE:
		fprintf(stderr, "Unknown type of message\n");
		break;
	}

	if (close(sock) == -1) {
		perror("Failed to close socket.\n");
		exit(EXIT_FAILURE);
	}

	return ret;
}

void usage(void)
{
	fprintf(stderr, "usage: cottage -f|-c <args>\n");
	exit(EXIT_FAILURE);
}
