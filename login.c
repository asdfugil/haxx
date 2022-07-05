/* This file is used to make NewTerm work unjailbroken */

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

#define POSIX_SPAWN_PERSONA_FLAGS_OVERRIDE 1
#define LOGIN_REAL "/usr/bin/login.real"

int posix_spawnattr_set_persona_np(const posix_spawnattr_t* __restrict, uid_t, uint32_t);
int posix_spawnattr_set_persona_uid_np(const posix_spawnattr_t* __restrict, uid_t);
char* envp[] = {NULL};

int main (int __unused argc, char* argv[]) {
  posix_spawnattr_t attr;
  posix_spawnattr_init(&attr);
  posix_spawnattr_set_persona_np(&attr, /*persona_id=*/99, POSIX_SPAWN_PERSONA_FLAGS_OVERRIDE);
  posix_spawnattr_set_persona_uid_np(&attr, 0);
  int pid = 0;
  int ret = posix_spawnp(&pid, LOGIN_REAL, NULL, &attr, argv, envp);
  waitpid(pid, NULL, 0);
  return 0;
}
