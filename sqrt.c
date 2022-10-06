#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

volatile int flag = 0;

struct complex{
    double Re;
    double Im;
};

void Input(struct complex res){
    if(res.Im)
        printf("Sqrt process: input %lf+i%lf\n", res.Re, res.Im);
    else
        printf("Sqrt process: input %lf\n", res.Re);
}

void Output(struct complex res){
    if(res.Im)
        printf("Sqrt process: output %lf+i%lf\n", res.Re, res.Im);
    else
        printf("Sqrt process: output %lf\n", res.Re);
}

void sigint_handler(int sig) {
        int f;
        struct complex res;
        double z, fi;

        f = open("./file", O_RDWR, 0);
        read(f, &res, sizeof(struct complex));

        Input(res);

        z=sqrt(sqrt(res.Re*res.Re+res.Im*res.Im));
        fi = atan2(res.Im, res.Re);
        res.Re = z*cos(fi/2);
        res.Im = z*sin(fi/2);

        Output(res);

        lseek(f, 0x00, SEEK_SET);
        write(f, &res, sizeof(struct complex));
        close(f);

        flag = sig;
        kill(getppid(), SIGUSR1);
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
    while(1)
        pause();
}

