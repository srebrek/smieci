
#include <bits/types/sigset_t.h>
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUF 256

#define ERR(source) \
    (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t sig_count = 0;
volatile sig_atomic_t last_sig = 0;
volatile sig_atomic_t alarm_sig = 0;

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

void parent_sig_handler(int sig)
{
    if(sig == SIGALRM) alarm_sig = 1;
    last_sig = sig;
}

void child_sig_handler(int sig)
{
    last_sig = sig;
    sig_count++;
}

ssize_t bulk_write(int fd, char *buf, size_t count)
{
    ssize_t c;
    ssize_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(write(fd, buf, count));
        if (c < 0)
            return c;
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}


void child_work(int n, sigset_t oldmask)
{
    alarm(1);
    pid_t pid = getpid();
    srand(time(NULL)*pid);
    int s = rand() % 91 + 10;
    s *= 1024;
    char c = '0' + n;
    printf("[%d] Child number: %d, Randomized number: %d\n", pid, n, s);
    char buf[MAX_BUF];
    if(sprintf(buf, "%d.txt", pid) < 0) ERR("sprintf");
    int fd = open(buf, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0777);
    char * text = (char *)malloc(s*sizeof(char) + 1);
    memset(text, c, s*sizeof(char) + 1);
    text[s] = '\0';
    // for(int i = 0; i < s; i++)
    // {
    //     text[i] = c;
    // }
    int written_blocks = 0;
    while(1)
    {
        last_sig = 0;
        fprintf(stderr,"[%d] Now stopping...\n", pid);
        while(last_sig != SIGUSR1 && last_sig != SIGALRM)
        {
            sigsuspend(&oldmask);
        }
        fprintf(stderr, "[%d] Starting after signal...\n", pid);
        if(alarm_sig) break;
        while(written_blocks < sig_count)
        {
            if(bulk_write(fd, text, s) < 0) ERR("bulk_write");
            written_blocks++;
        }
    }
    free(text);
    printf("[%d] Terminating.., signals received: %d, Blocks written: %d.\n", pid, sig_count, written_blocks);
}

void milisleep(int ms)
{
    struct timespec t = {0, ms*1e6};
    struct timespec remaining = t;
    while(remaining.tv_nsec > 0)
    {
        if(!nanosleep(&t, &remaining)) break;
        t = remaining;
    }
}

void parent_work()
{
    int count = 0;
    alarm(1);
    while(last_sig != SIGALRM)
    {
        milisleep(10);
        kill(0, SIGUSR1);
        fprintf(stderr, "Ps\n");
        count++;
    }
    printf("Signals sent: %d\n", count);
}

void usage(char *name)
{
    fprintf(stderr, "USAGE: %s m b s \n", name);
    fprintf(stderr,
            "m - number of 1/1000 milliseconds between signals [1,999], "
            "i.e. one milisecond maximum\n");
    fprintf(stderr, "b - number of blocks [1,999]\n");
    fprintf(stderr, "s - size of of blocks [1,999] in MB\n");
    fprintf(stderr, "name of the output file\n");
    exit(EXIT_FAILURE);
}

void create_chidren(int argc, char **argv, sigset_t oldmask)
{
    int n;
    pid_t pid;
    for(int i = 1; i<argc; i++)
    {
        n = atoi(argv[i]);
        if(n <= 0) ERR("atoi");
        if((pid = fork()) < 0) ERR("fork");
        if(!pid)
        {
            // sigprocmask(SIG_SETMASK, &oldmask, NULL);
            sethandler(child_sig_handler, SIGUSR1);
            child_work(n, oldmask);
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char **argv)
{
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    sethandler(parent_sig_handler, SIGALRM);
    create_chidren(argc, argv, oldmask);
    parent_work();
    while (wait(NULL) > 0)
            ;
    return EXIT_SUCCESS;
}