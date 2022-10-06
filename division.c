#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

volatile int flag = 0;

struct complex{
    double Re;
    double Im;
};

void Input(struct complex buf[2]){
    if(buf[0].Im || buf[1].Im)
        printf("Division process: input %lf+i%lf, %lf+i%lf\n", buf[0].Re, buf[0].Im, buf[1].Re, buf[1].Im);
    else
        printf("Division process: input %lf, %lf\n", buf[0].Re, buf[1].Re);
}

void Output(struct complex res){
    if(res.Im)
        printf("Division process: output %lf+i%lf\n", res.Re, res.Im);
    else
        printf("Division process: output %lf\n", res.Re);
}

void sigint_handler(int sig) {
        int f;
        struct complex res, buf[2];
        double d;

        f = open("./file", O_RDWR, 0);
        read(f, buf, sizeof(struct complex)*2);

        Input(buf);

        d = buf[1].Re*buf[1].Re-buf[1].Im*buf[1].Im;
        res.Re = (buf[0].Re*buf[1].Re-buf[0].Im*buf[1].Im)/d;
        res.Im = (-buf[0].Re*buf[1].Im+buf[0].Im*buf[1].Re)/d;

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

