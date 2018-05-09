#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void show_message() {
    pid_t pid = getpid();
    while(1) {
        printf("[Hijo: %d] CHECK>>>>\n", pid);
        sleep(1);
    }
}

int main() {
    pid_t pid_1 = fork();

    if (pid_1 == 0) {
        show_message();
        return 0;
    } else {
        pid_t pid_2 = fork();

        if (pid_2 == 0) {
            show_message();
            return 0;
        } else {
            printf("[PADRE] Espero 3 segundos...\n");
            sleep(3);            
            kill(pid_1, SIGSTOP);
            printf("[PADRE] Ya espere, suspendo al primer hijo.\n");

            printf("[PADRE] Espero 3 segundos y lo reanudo...\n");
            sleep(3);                        
            kill(pid_1, SIGCONT);
            printf("[PADRE] Ya espere, reanudo al primer hijo.\n");

            printf("[PADRE] Espero 3 segundos y termino los procesos.\n");            
            sleep(3);
            kill(pid_1, SIGINT);
            kill(pid_2, SIGINT);
            printf("[PADRE] Senal enviada\n");            

            wait(NULL);
            wait(NULL);
            return 0;
        }
    }
}