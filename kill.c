#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

volatile int flag = 0;
pid_t pid;

void sigint_handler(int sig) {
    if(sig == SIGUSR1){
        int f, buf[2], res;

        f = open("./file_in", O_RDWR, 0);
        read(f, buf, sizeof(int)*2);

        res = buf[0]+buf[1];

        lseek(f, 0x00, SEEK_SET);
        write(f, &res, sizeof(int));
        close(f);

        flag = sig;
        kill(pid, SIGUSR1);
        //pause();
    }
    else{
        return;
    }
}

int main() {
    struct sigaction sa;
    sigset_t set;

    sigemptyset(&set); // Clear set of signals
    sigaddset(&set, SIGUSR1); // Add signal to set
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sa.sa_mask = set;
    sigaction(SIGUSR1, &sa, NULL);
    pid =getppid();
    //wait(&pid);
    pause();
}
