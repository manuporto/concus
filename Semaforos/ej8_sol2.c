// Implementar el problema de lectores y escritores en ambiente Unix-Linux utilizando semaforos

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
static const char* ARCHIVO_SEMAFORO_3 = "/usr/bin/tail";
static const char* ARCHIVO_SHMEM = "/usr/bin/head";
static const char LETRA = 'B';
static const size_t ESCRITORES = 1;
static const size_t LECTORES = 3;

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
    return shmget(key, sizeof(int), 0644 | IPC_CREAT | IPC_EXCL);
}

int main() {
    int sem_room_empty = sem_init(ARCHIVO_SEMAFORO_1, LETRA, 1);
    int sem_mutex = sem_init(ARCHIVO_SEMAFORO_2, LETRA, 1);
    int sem_molinete = sem_init(ARCHIVO_SEMAFORO_3, LETRA, 1);
    int shm_lectores = shm_init(ARCHIVO_SHMEM, LETRA);

    for (size_t i = 0; i < ESCRITORES; i++) {
        if (fork() == 0) {
            pid_t pid = getpid();

            p(sem_molinete);
            p(sem_room_empty);
            printf("[%d] Escribiendo...\n", pid);
            v(sem_molinete);
            v(sem_room_empty);
            
            printf("[%d] Termine!\n", pid);
            return 0;
        }
    }

    for (size_t i = 0; i < LECTORES; i++) {
        if (fork() == 0) {
            pid_t pid = getpid();
            int lectores = 0; // Esto esta mal, hay que inicializarlo una unica vez
            shmat(shm_lectores, &lectores, 0);

            p(sem_molinete);
            v(sem_molinete);

            p(sem_mutex);
            lectores++;
            printf("[%d] #Lectores al entrar: %d\n", pid, lectores);
            if (lectores == 1) p(sem_room_empty);
            v(sem_mutex);

            printf("[%d] Leyendo...\n", pid);

            p(sem_mutex);
            lectores--;
            printf("[%d] #Lectores al salir: %d\n", pid, lectores);
            if (lectores == 0) v(sem_room_empty);
            v(sem_mutex);

            shmdt(&lectores);
            printf("[%d] Termine!\n", pid);
            return 0;
        }
    }

    size_t total = ESCRITORES + LECTORES;
    for (size_t i = 0; i < total; i++) wait(NULL);

    semctl(sem_room_empty, 0, IPC_RMID);
    semctl(sem_mutex, 0, IPC_RMID);
    semctl(sem_molinete, 0, IPC_RMID);
    shmctl(shm_lectores, IPC_RMID, NULL);
    return 0;
}