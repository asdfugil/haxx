#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define LOG_FILE "/private/var/haxx/haxx.log"

int main (int __unused argc, char* argv[]) {
  FILE* fptr = fopen(LOG_FILE,"a");
  if (fptr == NULL) {
    fprintf(stderr, "stage2: Unable to open %s: %s\n", LOG_FILE,  strerror(errno));
    exit(1);
  }
  time_t raw_time;
  struct tm* timeinfo;
  time(&raw_time);
  timeinfo = localtime(&raw_time);
  fprintf(fptr, "%s: stage2: hello, uid = %d\n", asctime(timeinfo), getuid());
  if (getuid() != 0) {
    fprintf(fptr, "%s: stage2: uid != 0\n", asctime(timeinfo));
    fclose(fptr);
    exit(1);
  }
  fprintf(fptr, "%s: stage2: we are done, we out there\n", asctime(timeinfo));
  fclose(fptr);
  exit(0);
}
