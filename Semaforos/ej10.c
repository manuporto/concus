// Implementar el problema de consumidores - productores con buffer acotado

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

static const char* ARCHIVO_SEMAFORO_1 = "/bin/bash";
static const char* ARCHIVO_SEMAFORO_2 = "/bin/ls";
static const char* ARCHIVO_SHMEM = "/bin/cat";
static const char LETRA = 'B';
static const int BUFF_SIZE = 5;

// Semaforo
int sem_init(const char* archivo, const char letra, int init_counter) {
    key_t key = ftok(archivo, letra);
    int sem_id = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    union semun init;
    init.val = init_counter;
    int res = semctl(sem_id, 0, SETVAL, init);
    return sem_id;
}

void p(int sem_id) {
    struct sembuf operacion;
    operacion.sem_num = 0;
    operacion.sem_op = -1;

    semop(sem_id, &operacion, 1); 
}

void v(int sem_id) {
    struct sembuf operacion;
    operacion.sem_num = 0;
    operacion.sem_op = 1;

    semop(sem_id, &operacion, 1); 
}


// Memoria compartida
int shm_init(const char* archivo, const char letra) {
    key_t key = ftok(archivo, letra);
    return shmget(key, sizeof(int) * BUFF_SIZE, 0644 | IPC_CREAT);
}

int main() {
    int sem_productor = sem_init(ARCHIVO_SEMAFORO_1, LETRA, BUFF_SIZE);
    int sem_consumidor = sem_init(ARCHIVO_SEMAFORO_2, LETRA, 0);
    int shm_id = shm_init(ARCHIVO_SHMEM, LETRA);

    if (fork() == 0) {
        // Consumidor
        int* buff = shmat(shm_id, NULL, SHM_RDONLY);
        int i = 0;
        int num;
        do {
            p(sem_consumidor);
            num = buff[i++ % BUFF_SIZE];
            v(sem_productor);
            printf("[CONS] Numero leido: %d\n", num);
        } while (num != -1);

        shmdt(buff);
        return 0;
    } else {
        // Productor
        int* buff = shmat(shm_id, NULL, 0);
        int i;
        for (i = 0; i < 20; i++) {
            printf("[PROD] Numero a escribir: %d\n", i);
            p(sem_productor);
            buff[i % BUFF_SIZE] = i;
            v(sem_consumidor);
        }
        p(sem_productor);
        buff[i % BUFF_SIZE] = -1;
        v(sem_consumidor);

        wait(NULL);
        shmdt(buff);
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem_productor, 0, IPC_RMID);
        semctl(sem_consumidor, 0, IPC_RMID);
        return 0;
    }
}