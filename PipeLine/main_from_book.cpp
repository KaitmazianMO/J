#include <sys/wait.h>

#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <fcntl.h>

int errExit(const char * msg) {
    exit(fprintf(stderr, "%s\n", msg));
}

int
main(int argc, char * argv[]) {
    int pfd[2]; /* Файловые дескрипторы канала */
    int pfd2[2];
    if (pipe(pfd) == -1) /* Создаем канал */
        errExit("pipe");
    if (pipe(pfd2) == -1) /* Создаем канал */
        errExit("pipe");        
    if (fcntl(pfd2[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);
    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        /* Первый потомок выполняет 'ls', записывая результат в канал */
        if (close(pfd[0]) == -1) /* Считывающий конец не используется */
            errExit("close 1 ");
        /* Дублируем стандартный вывод на записывающем конце канала;
        закрываем лишний дескриптор */
        if (pfd[1] != STDOUT_FILENO) {
            /* Проверка на всякий случай */
            if (dup2(pfd[1], STDOUT_FILENO) == -1)
                errExit("dup2 1");
            if (close(pfd[1]) == -1)
                errExit("close 2");
        }       
        execlp ("ls", "ls", "-la", "/", NULL); /* Записывает в канал */
        errExit("execlp ls");
    default:
        /* Родитель выходит из этого блока, чтобы создать следующего потомка */
        //wait (NULL);
        break;
    }
    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        /* Второй потомок выполняет 'wc', считывая ввод из канала */
        if (close(pfd[1]) == -1) /* Записывающий конец не используется */
            errExit("close 3");
        /* Дублируем стандартный ввод на считывающем конце канала;
        закрываем лишний дескриптор */
        if (pfd[0] != STDIN_FILENO) {
            /* Проверка на всякий случай */
            if (dup2(pfd[0], STDIN_FILENO) == -1)
                errExit("dup2 2");
            if (close(pfd[0]) == -1)
                errExit("close 4");
        }
        dup2 (pfd2[1], STDOUT_FILENO);     
        close (pfd2[1]);
        execlp ("grep", "grep", "bin", NULL); /* Читает из канала */
        errExit("execlp wc ");
    default:
        //wait (NULL);/* Родитель выходит из этого блока */
        break;
    }
    /* Родитель закрывает лишние дескрипторы канала и ждет завершения дочерних процессов */
    printf ("reached reading\n");
    char buff[256] = {};
    while (int err = read (pfd2[0], buff, sizeof buff)) {
        if (err == -1) {
            errExit ("read fail");
        }
        err = write (STDOUT_FILENO, buff, sizeof buff);
        if (err = -1) {
            errExit ("write fail");
        }
    } 
    
    if (close(pfd[0]) == -1)
        errExit("close 5");
    if (close(pfd[1]) == -1)
        errExit("close 6");
    if (wait(NULL) == -1)
        errExit("wait 1");
    if (wait(NULL) == -1)
        errExit("wait 2");
    exit(EXIT_SUCCESS);
}