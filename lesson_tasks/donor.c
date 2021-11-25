#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

int getFileSize(int fd);

int main (int argc, char* argv[]){
    if (argc != 2) {
        fatal ("Usage: BOARD");
    }

    int fdFrom = open (argv[1], O_RDONLY);
    if(fdFrom == -1){
        fatal ("open failed\n");
    }

    key_t key = ftok ("../", 1);
    //int semId = semget(key, 1, IPC_CREAT | 0666);
    int fileSize = getFileSize (fdFrom);
    int shmFileId = shmget (key, fileSize, IPC_CREAT | 0666);
    key_t keySize = ftok ("../", 2);
    int shmSizeId = shmget (keySize, sizeof (int), IPC_CREAT|0666);

    char* arr = (char*) shmat (shmFileId, NULL, O_WRONLY);
    int* size = (int*) shmat (shmSizeId, NULL, O_WRONLY);
    *size = fileSize;
    read (fdFrom, arr, fileSize);
    shmdt (arr);
    }

    int getFileSize(int fd){
    if(fd == -1)
    return -1;

    struct stat buf;
    fstat(fd, &buf);
    off_t file_size = buf.st_size;
    return file_size;
}
