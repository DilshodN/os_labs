#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 257

typedef struct {
  off_t offset;
  size_t lineLength;
} Record;

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: filename as argument\n");
    return 1;
  }

  int fd = 0;

  if ((fd = open(argv[1], O_RDONLY)) == -1) {
    perror("Can not open file");
    return 1;
  }

  size_t lineCount = 1;
  Record *records = calloc(lineCount, sizeof(Record));
  size_t lineLen = 0;

  char c = 0;

  while (read(fd, &c, sizeof(char))) {
    if (c == '\n') {
      if ((records = realloc(records, (lineCount + 1) * sizeof(Record))) == NULL) {
        perror("No memory");
        return 1;
      }

      records[lineCount].lineLength = lineLen;
      records[lineCount].offset = lseek(fd, 0L, SEEK_CUR);
      lineCount++;
      lineLen = 0;
      continue;
    }
    lineLen++;
  }

  lineCount--;

  if (lineCount == 0) {
    printf("file is empty");
    close(fd);
    return 1;
  }

  long long lineNumber = 0;

  size_t bufsize = BUF_SIZE;

  char *buf = calloc(bufsize, sizeof(char));


  while (printf("Enter line number from 1 to %zu:\n", lineCount) && scanf("%lld", &lineNumber)) {
    if (lineNumber == 0) {
      break;
    }

    if (lineNumber < 0 || lineNumber > lineCount) {
      printf("Bad line number: valid values 1-%zd\n", lineCount);
      continue;
    }

    if (records[lineNumber].lineLength + 1 > bufsize) {
      bufsize = records[lineNumber].lineLength + 1;
      if (realloc(buf, bufsize * sizeof(char)) == NULL) {
        perror("No memory");
        close(fd);
        free(records);
        return 1;
      }
    }

    memset(buf, 0, bufsize * sizeof(char));

    if (lseek(fd, records[lineNumber - 1].offset, SEEK_SET) == -1) {
      perror("Seek err");
      free(records);
      free(buf);
      close(fd);
      return 1;
    }

    if (read(fd, buf, records[lineNumber].lineLength * sizeof(char)) == -1) {
      perror("Read err");
      free(records);
      free(buf);
      close(fd);
      return 1;
    }

    printf("%s\n", buf);
  }

  close(fd);
  free(records);
  free(buf);
  return 0;
}
