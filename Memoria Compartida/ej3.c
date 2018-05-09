#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

static const char* SHM_FILE = "/bin/bash";
static const char SHM_LETTER = 'A';
static const size_t SHM_BUFFER = 100;

int main(int argc, char *argv[]) {

    pid_t id = fork();

    if (id == 0) {
        sleep(2);
        key_t key = ftok(SHM_FILE, SHM_LETTER);
        int shm_id = shmget(key, sizeof(char) * SHM_BUFFER, 0644 | IPC_CREAT);
       
        char* buffer = (char*) shmat(shm_id, NULL, SHM_RDONLY);
        printf("[HIJO] El mensaje leido es: %s\n", buffer);

        shmdt(buffer);

        return 0;
    }
    else {
        key_t key = ftok(SHM_FILE, SHM_LETTER);
        int shm_id = shmget(key, sizeof(char) * SHM_BUFFER, 0644 | IPC_CREAT);
        char* buffer = (char*) shmat(shm_id, NULL, 0);
        char* msg = "Hola hijito";
        
        strcpy(buffer, msg);
        printf("[PADRE] El mensaje escrito es: %s\n", buffer);

        int estado;
        wait(&estado);
        shmdt(buffer);
        shmctl(shm_id, IPC_RMID, NULL);

        return 0;
    }
}