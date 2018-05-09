#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);
    pid_t pid = fork();

    if (pid == 0) {
        // Close read fd
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        const char* msg = "Soy el hijito y este es el mensaje que le mando a mi viejo\n";

        printf("%s", msg);

        close(pipe_fd[1]);
        return 0;
    } else {
        // Close write fd
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        const size_t BUFFER = 100;
        char buffer[BUFFER];

        read(STDIN_FILENO, buffer, BUFFER);
        printf("[PADRE] Lei el siguiente mensaje: %s", buffer);

        close(pipe_fd[0]);
        return 0;
    }
}