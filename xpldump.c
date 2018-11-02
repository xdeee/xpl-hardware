
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_PORT 49005
#define PROL_DATA "DATA*"
#define PROL_SIZE (sizeof(PROL_DATA) - 1)


typedef struct Block
{
	int index;
	float data[8];
} Block;

static volatile int running = 1;

int checkPrologue(char *data, char *data2) {
	int i=0;

	while(data[i]) {
		if (data[i] != data2[i]) return 0;
		i++;
	}
	return 1;
}

char* readableSize(double size, char *buf) {
	int i = 0;

	const char* units[] = { "bytes", "KB", "MB", "GB", "TB", "PB" };
	while (size>1024 && i < sizeof(units)) {
		size /= 1024;
		i++;
	}

	sprintf(buf, "%.*f %s", i, size, units[i]);
	return buf;
}

void dump(char* buf, int len) {
	while(len--) {
		printf("%x ", *buf);
		buf++;
	}
	printf("\n");
} 

void intHandler(int sig) {
	running = 0;
}

int main(int argc, char *argv[]) {

	char buf[1024];
	double total = 0;
	int sock;
	struct sockaddr_in name;
	struct hostent *hp, *gethostbyname();
	int bytes;
	int doDump = 0;

	while(argc--) {
		if (strcmp("-d", argv[argc]) == 0)
			doDump = 1;
	}

	signal(SIGINT, intHandler);

	printf("Listen activating.\n");

  /* Create socket from which to read */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)   {
		perror("Opening datagram socket");
		exit(1);
	}

  /* Bind our local address so that the client can send to us */
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	name.sin_port = htons(SERVER_PORT);

	if (bind(sock, (struct sockaddr *) &name, sizeof(name))) {
		perror("binding datagram socket");
		exit(1);
	}

	printf("Socket has port number #%d\n", ntohs(name.sin_port));
	int i=0;
	while ( running && (bytes = read(sock, buf, 1024)) > 0 ) {

		printf("\e[1;1H\e[2J"); //clear screen and move coursor to 1:1
		printf("#%d. recv bytes: %d\n",i++ , bytes);
		if (doDump)
			dump(buf, bytes);

		total += bytes;

		if (bytes >= (PROL_SIZE + sizeof(Block)) && checkPrologue(PROL_DATA, buf)) {

			Block *msg = (Block*) (buf+PROL_SIZE);
			bytes -= PROL_SIZE;

			while(bytes >= sizeof(Block)) {
				printf("Index: %d\n", msg->index);

				for (int i=0; i<8; i++)
					printf("\t%.3f", msg->data[i]);
				printf("\n");

				bytes -= sizeof(Block);
				msg++;
			}
		}
	}

	close(sock);
	printf("Total recv: %s\n", readableSize(total, buf));
}
