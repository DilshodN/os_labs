#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

off_t find_size(int fd){
    off_t size = lseek(fd, 0l, SEEK_END);
    if(size == -1){
        perror("Could not find the size of file!\n");
        return size;
    }
    lseek(fd, 0l, SEEK_SET);
    return size;
}

void read_to_buff(int fd, char* buffer, off_t size){
    ssize_t rv = read(fd, buffer, size);
    if(rv == -1){
        perror("Can't read a file!\n");
        return;
    }
}

void parse(const char buff[], uint32_t line, char buff_line[], off_t size){
    if(size <= 0){
        perror("Invalid size of buffer!\nй");
        return;
    }
    int count = 0;
    for(off_t i = 0; i < size; i ++){
        if(buff[i] == '\n' || buff[i] == '\0'){
            count++;
        }
    }
    printf("%d", count);
}

int main(int argc, char* argv[]) {
    printf("task5:\n");

    if(argc <= 1){
        perror("Arguments count error\n"
               "Example: ./a.out filename.txt\n");
    }
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1 ){
        perror("Could not open the file!\n");
        return fd;
    }

    off_t size_of_file = find_size(fd);
    printf("Size of file: %lld bytes\n", size_of_file);
    char buff[BUFSIZ];
    read_to_buff(fd, buff, size_of_file);
//    printf("%s", buff);
    char line_buff[BUFSIZ];
    uint32_t line = 0;
    parse(buff, line, line_buff, size_of_file);
    close(fd);
    return 0;
}
