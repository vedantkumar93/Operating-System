#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    if(pid>0){
        printf("Admin\t %d %d\n", pid, getpid());
    }
    else{
        printf("Computer %d %d\n", pid, getpid());
    }
}