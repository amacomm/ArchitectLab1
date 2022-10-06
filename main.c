#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

pid_t plus, minus, mul, div, sqr;

volatile int flag = 0;

void sigint_handler(int sig) {
    flag = sig;
}

struct complex{
    double Re;
    double Im;
};

void Output(struct complex x1, struct complex x2){
    if(x1.Im||x2.Im)
        printf("X1= %lf+i%lf, X2= %lf+i%lf\n", x1.Re, x1.Im, x2.Re, x2.Im);
    else
        printf("X1= %lf, X2= %lf\n", x1.Re, x2.Re);
}

struct complex SendRecv(pid_t child, struct complex x, struct complex y){
    struct complex buf[2] = {x, y},  res;
    int f;

    f = open("./file", O_RDWR, 0);
    write(f, &buf, sizeof(struct complex)*2);

    close(f);

    kill(child,SIGUSR1);
    pause();

    f = open("./file", O_RDWR, 0);
    read(f, &res, sizeof(struct complex));
    close(f);

    return res;
}

void MainWork(){

    double in1, in2, in3;
    struct complex a, b, c, add, x1, x2, post;

    printf("Enter a, b, c: ");
    scanf("%lf", &in1);
    scanf("%lf", &in2);
    scanf("%lf", &in3);

    a.Re = in1; a.Im = 0;
    b.Re = in2; b.Im = 0;
    c.Re = in3; c.Im = 0;

    x2 = SendRecv(mul, b, b);
    post.Re =4; post.Im = 0;
    add = SendRecv(mul, post, a);
    add = SendRecv(mul, add, c);
    x2 = SendRecv(minus, x2, add);
    x2 = SendRecv(sqr, x2, x2);
    post.Re = 0;
    add = SendRecv(minus, post, b);
    x1 = SendRecv(plus, add, x2);
    x2 = SendRecv(minus, add, x2);
    post.Re = 2;
    add = SendRecv(mul, post, a);
    x1 = SendRecv(div, x1, add);
    x2 = SendRecv(div, x2, add);
    Output(x1, x2);
}
void End(){
   kill(plus, SIGINT);
   kill(minus, SIGINT);
   kill(mul, SIGINT);
   kill(div, SIGINT);
   kill(sqr, SIGINT);
}

void creatProcess(){
    plus = fork();
    if (plus){
        minus = fork();
        if (minus){
            mul = fork();
            if (mul){
                div = fork();
                if (div){
                    sqr = fork();
                    if (sqr){
                        MainWork();
                        End();
                    }
                    else{
                        execl("./sqrt", "", NULL);
                        perror("Error to start sqrt");
                    }
                }
                else{
                    execl("./division", "", NULL);
                    perror("Error to start division");
                }
            }
            else{
                execl("./multiplication", "", NULL);
                perror("Error to start multiplication");
            }
        }
        else{
            execl("./minus", "", NULL);
            perror("Error to start minus");
        }
    }
    else{
        execl("./plus", "", NULL);
        perror("Error to start plus");
    }
}


int main() {

    struct sigaction sa;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sa.sa_mask = set;
    sigaction(SIGUSR1, &sa, NULL);
    creatProcess();

}
