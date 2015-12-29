#include <ctype.h>
#include <getopt.h>
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
#define VERSION "0.3"

/* The errors (or lack of) that could be sent back by howm. */
enum ipc_errs { IPC_ERR_NONE, IPC_ERR_SYNTAX, IPC_ERR_ALLOC, IPC_ERR_NO_FUNC,
	IPC_ERR_TOO_MANY_ARGS, IPC_ERR_TOO_FEW_ARGS, IPC_ERR_ARG_NOT_INT,
	IPC_ERR_ARG_NOT_BOOL, IPC_ERR_ARG_TOO_LARGE, IPC_ERR_ARG_TOO_SMALL,
	IPC_ERR_UNKNOWN_TYPE, IPC_ERR_NO_CONFIG };
enum msg_type { MSG_FUNCTION = 1, MSG_CONFIG, MSG_OP_HELPER };

static void usage(void);
static int send_command(int argc, char *argv[], int type);
static int send_op(char *argv[]);
static int setup_socket(struct sockaddr_un addr);
static void handle_error(int err);

/* Send a command to howm and wait for its reply. */
int main(int argc, char *argv[])
{
	int ret, ch, type = 0;

	if (argc < 2)
		usage();

	while ((ch = getopt(argc, argv, "vcfo")) != -1 && !type) {
		switch (ch) {
		case 'c':
			type = MSG_CONFIG;
			break;
		case 'f':
			type = MSG_FUNCTION;
			break;
		case 'o':
			type = MSG_OP_HELPER;
			break;
		case 'v':
			printf("%s\n", VERSION);
			exit(EXIT_SUCCESS);
		default:
			usage();
		}
	}

	if (!type || (type == MSG_OP_HELPER && argc < 5))
		usage();

	argc -= 2;
	argv += 2;

	ret = type == MSG_OP_HELPER ? send_op(argv)
					: send_command(argc, argv, type);

	if (ret == -1)
		perror("Failed to send data");

	if (ret != IPC_ERR_NONE)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

static int send_command(int argc, char *argv[], int type)
{
	struct sockaddr_un addr;
	int sock;
	int n, len = 0, off = 0, ret, recvd;
	char data[BUF_SIZE];

	sock = setup_socket(addr);

	n = snprintf(data, sizeof(data) - (2 * sizeof(char)), "%c%c", type, 0);
	len += n;
	off += n;
	for (; argc > 0 && sizeof(data) - off > 0; off += n, argc--, argv++) {
		n = snprintf(data + off, sizeof(data) - off, "%s%c", *argv, 0);
		len += n;
	}

	ret = write(sock, data, len);
	if (ret == -1)
		perror("Failed to send command");

	recvd = read(sock, &ret, sizeof(ret));
	if (recvd == -1) {
		perror("Failed to receive response");
		ret = -1;
	}

	if (ret != IPC_ERR_NONE)
		handle_error(ret);

	if (close(sock) == -1) {
		perror("Failed to close socket.\n");
		exit(EXIT_FAILURE);
	}

	return ret;
}

/**
 * Howm expects a connection, a single command and then for the
 * socket to be closed. send_command opens and closes a socket,
 * so howm behaves as if cottage were to be invoked 3 times.
 */
static int send_op(char *argv[])
{
	int ret = 0;
	char *op[] = { *argv++ };
	char *count[] = { "count", *argv++ };
	char *motion[] = { "motion", *argv };

	/* Send op_* command */
	ret = send_command(1, op, MSG_FUNCTION);
	if (ret != IPC_ERR_NONE)
		return ret;

	/* Send count x command */
	ret = send_command(2, count, MSG_FUNCTION);
	if (ret != IPC_ERR_NONE)
		return ret;

	/* Send motion x command */
	ret = send_command(2, motion, MSG_FUNCTION);
	if (ret != IPC_ERR_NONE)
		return ret;

	return IPC_ERR_NONE;
}

static void handle_error(int ret)
{
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
	case IPC_ERR_NO_CONFIG:
		fprintf(stderr, "Unknown config option\n");
		break;
	default:
		fprintf(stderr, "Unknown error code\n");
		break;
	}
}

static int setup_socket(struct sockaddr_un addr)
{
	char *sp;
	char sock_path[256];
	int sock;

	sp = getenv(ENV_SOCK_VAR);

	if (sp != NULL)
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

	return sock;
}

static void usage(void)
{
	fprintf(stderr, "usage: cottage -f|-c|-o <args>\n");
	exit(EXIT_FAILURE);
}
