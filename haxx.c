#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define LOG_FILE "/private/var/haxx/haxx.log"
#define STAGE_TWO "/usr/local/bin/haxx"

#ifndef STAGE_THREE
#define STAGE_THREE "/Applications/notether.app/notether"
#endif

#define ENABLE_STAGE_THREE 1

char *stage_3_argv[] = {STAGE_THREE, NULL};
char *envp[] = {NULL};

void get_time_string(char *str)
{
  time_t raw_time;
  struct tm *timeinfo;
  time(&raw_time);
  timeinfo = localtime(&raw_time);
  strcpy(str, asctime(timeinfo));
  return;
}
int main(int __unused argc, char *argv[])
{
  char timestr[50] = "";
  FILE *fptr;
  if (argc > 1)
  {
    if (strcmp(argv[1], "LAUNCHD_HAXX") == 0)
    {
      fptr = stderr;
    }
    else
    {
      fptr = fopen(LOG_FILE, "a");
    }
  }
  if (fptr == NULL)
  {
    fprintf(stderr, "stage2: Unable to open %s: %s\n", LOG_FILE, strerror(errno));
    exit(1);
  }
  get_time_string(timestr);
  fprintf(fptr, "%s: stage2: hello, uid = %d\n", timestr, getuid());
#ifdef HAXX_SETUID
  if (getuid() != 0)
  {
    get_time_string(&timestr);
    fprintf(fptr, "%s: stage2: uid != 0\n", timestr);
    if (strcmp(argv[1], "LAUNCHD_HAXX") != 0)
      fclose(fptr);
    exit(1);
  }
#endif
#if ENABLE_STAGE_THREE
  /* Execute stage 3 */
  uint8_t counter = 0;
  int status = 0;
  fprintf(fptr, "%s: stage2: About to execute stage3...", timestr);
  do
  {
    pid_t child = fork();
    if (child == 0)
    {
      execve(STAGE_THREE, stage_3_argv, envp);
      fprintf(fptr, "%s: stage2: unable to execute stage3: %s\n", timestr, strerror(errno));
      exit(1);
    }
    waitpid(child, &status, 0);
    get_time_string(timestr);
    counter += 1;
    fprintf(fptr, "%s: stage3 iteration %hhu\n", timestr, counter);
  } while (WIFEXITED(status) && WEXITSTATUS(status) != 0 && counter < 30);
  get_time_string(timestr);
  if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
  {
    /* abort for visibility */
    fprintf(fptr, "%s: stage3 succeed with %hhu iterations\n", timestr, counter);
  }
  else if (counter >= 30)
  {
    fprintf(fptr, "%s: stage3 failed\n", timestr);
    abort();
  }
  else if (WIFSIGNALED(status))
  {
    fprintf(fptr, "%s: stage3 exited due to signal: %d\n", timestr, WTERMSIG(status));
    abort();
  }
  else
  {
    fprintf(fptr, "%s: stage3 exited for unknown reason\n", timestr);
    abort();
  }
#endif
  if (strcmp(argv[1], "LAUNCHD_HAXX") != 0)
    fclose(fptr);
  exit(0);
}
