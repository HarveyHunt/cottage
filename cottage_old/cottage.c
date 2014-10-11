#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/howm"
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sock, len = 0, off = 0, n = 0;
	char data[BUF_SIZE];

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

	if (send(sock, data, len, 0) == -1) {
		perror("Failed to send data");
		exit(EXIT_FAILURE);
	}

	close(sock);
}
