#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>

const char socket_path[] = "./socket";

int main(int argc, char *argv[]) {

    char buf[BUFSIZ];
    int sock;
    struct sockaddr_un address;
    if((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        perror("socket error");
        return 1;
    }

    int enable = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
        perror("setsockopt error");
        return 1;
    }
//   struct  sockaddr_un {
//        sa_family_t     sun_family;     /* [XSI] AF_UNIX */
//        char            sun_path[104];  /* [XSI] path name (gag) */
//    };

    address.sun_family = AF_UNIX;
    unlink(socket_path);
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);

    if(bind(sock, (const struct sockaddr *) &address, sizeof(address)) < 0){
        perror("bind error");
        return 1;
    }

    if(listen(sock, 1) < 0){
        perror("listen error");
        return 1;
    }
    int client = accept(sock, NULL, NULL);
    if(client < 0){
    perror("accept error");
        if(close(sock) < 0){
            perror("unable to close server");
            }
        return 1;
    }
    if(setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
        perror("setsockopt error");
        return 1;
    }
    size_t read_count;
    while((read_count = read(client, buf, sizeof(buf))) > 0){
        for(size_t i = 0; i < read_count; ++i){
            putchar(toupper(buf[i]));
        }
    }
    if(read_count < 0){
        perror("read");
        return 1;
    }
    if(read_count == 0){
        if(close(client) < 0){
            perror("unable to close client");
            return 1;
        }
        if(close(sock) < 0){
            perror("unable to close server");
            return 1;
        }
    }
    return 0;
}