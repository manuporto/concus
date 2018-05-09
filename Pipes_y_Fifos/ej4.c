#include <unistd.h>
#include <string.h>
#include <stdio.h>

static const size_t HIJOS = 5;

void escribir_mensaje(int fd) {
    const size_t BUFFER = 100;
    const char* msg_1 = "Primer mensaje\n";
    const char* msg_2 = "Segundo mensaje\n";
    const char* msg_3 = "Tercer mensaje\n";

    write(fd, msg_1, strlen(msg_1));
    write(fd, msg_2, strlen(msg_2));
    write(fd, msg_3, strlen(msg_3));
}

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);

    for (size_t i = 0; i < HIJOS; i++) {
        if (fork() == 0) {
            close(pipe_fd[0]);
            escribir_mensaje(pipe_fd[1]);
            close(pipe_fd[1]);
            return 0;
        }
    }

    close(pipe_fd[1]);

    char ch;
    while (read(pipe_fd[0], &ch, sizeof(char))) {
        printf("%c", ch);
    }
    
    close(pipe_fd[0]);
    return 0;
}