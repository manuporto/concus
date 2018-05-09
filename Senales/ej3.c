#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("I'm going to sleep 5 seconds and you can't SIGINT me!\n");
    sleep(5);
    printf("Done sleeping, nvd\n");
    
    return 0;
}