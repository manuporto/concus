/*
Escribir un programa en lenguage c o cpp utilizando semaforos de System V para resolver el problema del barbero:
* Un barbero corta el pelo a sus clientes, de a uno por vez.
* Si no tiene clientes para atender, el barbero se pone a dormir y cuando un cliente llega realizan el corte de pelo.
* Si llega un cliente y el barbero esta ocupado con otro cliente, debe esperar a que termine para ser atendido.
* Considere que ingresan m clientes a la barberia
* El barbero realiza el corte de pelo invocando la funcion cortar_pelo()
* El cliente recibe el corte de pelo invocando la funcion recibir_corte_de_pelo().
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

static const size_t CLIENTES = 5;
static const char* ARCHIVO_SEMAFORO_1 = "/bin/bash";
static const char* ARCHIVO_SEMAFORO_2 = "/bin/ls";
static const char LETRA = 'B';

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

int main() {
    int sem_id_cliente = sem_init(ARCHIVO_SEMAFORO_1, LETRA, 0);
    int sem_id_barbero = sem_init(ARCHIVO_SEMAFORO_2, LETRA, 1);

    for (size_t i = 0; i < CLIENTES; i++) {
        if (fork() == 0) {
            pid_t pid = getpid();

            printf("[CLIENTE: %d] Esperando a ser atendido...\n", pid);
            v(sem_id_cliente);
            p(sem_id_barbero);
            printf("[CLIENTE: %d] Pelo cortado\n", pid);

            return 0;
        }
    }

    for (size_t i = 0; i < CLIENTES; i++) {
        printf("[BARBERO] Esperando clientes...\n");
        p(sem_id_cliente);
        printf("[BARBERO] Ya corte el pelo\n");
        v(sem_id_barbero);
    }

    for (size_t i = 0; i < CLIENTES; i++) {
        wait(NULL);
    }

    semctl(sem_id_cliente, 0, IPC_RMID);
    semctl(sem_id_barbero, 0, IPC_RMID);
    return 0;
}