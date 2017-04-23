#include <stdio.h>
#include <signal.h>

static bool * quit;

static void signothing (int sig, siginfo_t * info, void * ptr) {
  printf ("%s : %d\n", __FUNCTION__, sig);
  * quit = false;
}

void AtExit (bool * x) {
  quit = x;
  struct sigaction action;
  action.sa_sigaction = signothing;
  action.sa_flags = SA_SIGINFO;
  sigfillset (& action.sa_mask);
  if (sigaction (SIGINT, &action, NULL)) {
    perror("SIGINT"); return;
  }
  if (sigaction (SIGQUIT, &action, NULL)) {
    perror("SIGQUIT"); return;
  }
}

