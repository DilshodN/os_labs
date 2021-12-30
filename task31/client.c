#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char socket_path[] = "./socket";

int main(int argc, char *argv[]) {
    char buf[BUFSIZ];
    int sock;
    struct sockaddr_un address;
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);

    if (connect(sock, (const struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("connect error");
        return 1;
    }
    memset(buf, 0, sizeof(buf));
    size_t read_count;
    while ((read_count = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (strcmp(buf, "exit") == 0) {
            break;
        }
        if (write(sock, buf, read_count) != read_count) {
            perror("write error");
            return 1;
        }
    }
    if (close(sock) < 0) {
        perror("unable close socket");
        return 1;
    }
    return 0;

}
