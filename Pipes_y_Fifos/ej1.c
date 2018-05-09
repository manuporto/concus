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
        const char* msg = "Soy el hijito y este es el mensaje que le mando a mi viejo\n";

        write(pipe_fd[1], msg, strlen(msg));

        close(pipe_fd[1]);
        return 0;
    } else {
        // Close write fd
        close(pipe_fd[1]);
        const size_t BUFFER = 100;
        char buffer[BUFFER];

        read(pipe_fd[0], buffer, BUFFER);
        printf("%s", buffer);

        close(pipe_fd[0]);
        return 0;
    }
}