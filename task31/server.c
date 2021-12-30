#include <sys/socket.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <netinet/in.h>
#include <stdbool.h>

#define BACKLOG 100
#define MAX_CONN 100

const char path[] = "./socket";
const char sigint_message[] = "SIGINT";

int sigint_pipe_array[2];
void sigint_handler(int a) {
    write(sigint_pipe_array[1], sigint_message, sizeof(sigint_message));
}

int main() {

    int sock;
    struct pollfd poll_fds[MAX_CONN];
    char buffer[BUFSIZ];
    struct sockaddr_un address = {.sun_family = AF_UNIX};

    int sigint_pipe = pipe(sigint_pipe_array);
    if(sigint_pipe < 0){
        perror("cant create a pipe");
        exit(1);
    }

    for (int i = 0; i < MAX_CONN; i++) {
        poll_fds[i].fd = -1;
    }

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error!\n");
        exit(1);
    }

    unlink(path);
    strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);

    if (bind(sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
        if(close(sock) < 0) perror("Unable to close server");
        perror("Socket naming error!\n");
        exit(2);
    }

    if (listen(sock, BACKLOG) == -1) {
        if(close(sock) < 0) perror("Unable to close server");
        perror("Listening error!\n");
        exit(3);
    }

    poll_fds[0].fd = sigint_pipe_array[0];
    poll_fds[0].events = POLLIN;

    poll_fds[1].fd = sock;
    poll_fds[1].events = POLLIN;

    signal(SIGINT, sigint_handler);
    bool was_signal = false;
    while(1) {
        poll(poll_fds, MAX_CONN, -1);
        if(poll_fds[0].revents & POLLIN){
            char signal_message[BUFSIZ];
            read(poll_fds[0].fd, signal_message, BUFSIZ);
            if(strcmp(signal_message, "SIGINT") == 0){
                printf("Signal have processed\n");
                signal(SIGINT, sigint_handler);
                for(int i = 1; i < MAX_CONN && !was_signal; i++){
                    if(close(i) < 0){
                        perror("unable to close");
                    }
                    exit(1);
                }
            }
        }
        if (poll_fds[1].revents & POLLIN) {
            int new_client;
            if ((new_client = accept(sock, NULL, NULL)) == -1) {
                perror("Connection accepting error!\n");
                if(close(sock) < 0) perror("Unable to close server");
            }
            const char accepting_client_message[] = "New client connected.\n";
            write(STDOUT_FILENO, accepting_client_message, sizeof(accepting_client_message));
            int num_of_client;
            for (num_of_client = 1; num_of_client < MAX_CONN; num_of_client++) {
                if (poll_fds[num_of_client].fd < 0) {
                    poll_fds[num_of_client].fd = new_client;
                    poll_fds[num_of_client].events = POLLIN;
                    break;
                }
            }
            if (num_of_client == MAX_CONN) {
                const char connection_limit_message[] = "New client connected.\n";
                write(STDOUT_FILENO, connection_limit_message, sizeof(connection_limit_message));
            }
        }
        for (int i = 2; i < MAX_CONN; i++) {
            if (poll_fds[i].fd < 0) {
                continue;
            }
            if (poll_fds[i].revents & POLLIN) {
                memset(buffer, 0, BUFSIZ);
                size_t bytes_read = read(poll_fds[i].fd, buffer, BUFSIZ);
                if (bytes_read < 0) {
                    perror("Reading error!");
                }
                else if (bytes_read == 0) {
                    const char closing_client_message[] = "New client connected.\n";
                    write(STDOUT_FILENO, closing_client_message, sizeof(closing_client_message));
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1;
                }
                else {
                    for(int j = 0; j < bytes_read; j++) {
                        if (islower(buffer[j])) {
                            putchar(toupper(buffer[j]));
                        }
                        else {
                            putchar(buffer[j]);
                        }
                    }
                }
            }
        }
    }
}