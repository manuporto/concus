#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static const size_t ITERACIONES = 20;
static const char* ARCHIVO_LOCK = "/tmp/ej3_lock";
static const char* ARCHIVO_OUTPUT = "salida.txt";

void escribir_archivo(int numero) {
    pid_t pid = getpid();
    for (size_t i = 0; i < ITERACIONES; i++) {
        int fd = open(ARCHIVO_OUTPUT, O_RDONLY, 0777);
        int nro_leido;
        read(fd, &nro_leido, sizeof(int));
        close(fd);
        printf("[%d] Lei un %d\n", pid, nro_leido);

        fd = open(ARCHIVO_OUTPUT, O_WRONLY, 0777);
        int nro_a_escribir = nro_leido + numero;
        write(fd, &nro_a_escribir, sizeof(int));
        close(fd);
        printf("[%d] Escribi un %d\n", pid, nro_a_escribir);
    }
}

int main() {
    int fd = open(ARCHIVO_OUTPUT, O_CREAT | O_WRONLY, 0777);
    int nro = 0;
    write(fd, &nro, sizeof(int));
    close(fd);

    if (fork() == 0) {
        escribir_archivo(1000);
        return 0;
    } else {
        escribir_archivo(-1000);
        return 0;
    }
}