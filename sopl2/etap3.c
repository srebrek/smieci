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
#include <ctype.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))

ssize_t bulk_read(int fd, char* buf, size_t count)
{
    ssize_t c;
    ssize_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(read(fd, buf, count));
        if (c < 0)
            return c;
        if (c == 0)
            return len;  // EOF
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}

ssize_t bulk_write(int fd, char* buf, size_t count)
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

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

void usage(int argc, char* argv[])
{
    printf("%s n f \n", argv[0]);
    printf("\tf - file to be processed\n");
    printf("\t0 < n < 10 - number of child processes\n");
    exit(EXIT_FAILURE);
}

volatile sig_atomic_t last_signal = 0;

void sig_handler(int sig) { last_signal = sig; }

void sigchld_handler(int sig)
{
    pid_t pid;
    for (;;)
    {
        pid = waitpid(0, NULL, WNOHANG);
        if (pid == 0)
            return;
        if (pid <= 0)
        {
            if (errno == ECHILD)
                return;
            ERR("waitpid");
        }
    }
}

void milisleep(int ms)
{
    struct timespec to_sleep = {0, ms*1000000};
    struct timespec remaining = to_sleep;
    while(remaining.tv_nsec > 0)
    {
        if(!nanosleep(&to_sleep, &remaining)) break;
        to_sleep = remaining;
    }
}


void child_work(char* data_to_process, int data_size, sigset_t oldmask, char* file_path, int number)
{
    sigsuspend(&oldmask);
    // printf("My PID = %d\n", getpid());
    // if (write(1, data_to_process, data_size) < 0) ERR("read");
    // printf("\n");
    char* result = (char*)malloc((strlen(file_path) + 3) * sizeof(char));
    if (!result) ERR("malloc");
    strcpy(result, file_path);
    char extension[3] = {'-', '1' + number, '\0'};
    strcat(result, extension);
    
    int out;
    if ((out = open(result, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
    {
        free(result);
        ERR("open");
    }
    free(result);
    for (int i = 0; i < data_size; i++)
    {
        if (isupper(data_to_process[i])) data_to_process[i] = tolower(data_to_process[i]);
        else if (islower(data_to_process[i])) data_to_process[i] = toupper(data_to_process[i]);
        if (write(out, &(data_to_process[i]), 1) < 0) ERR("write");
        milisleep(250);
    }
    if (close(out)) ERR("close");
}

void make_children(char* file_path, int n, sigset_t oldmask)
{
    int in;
    if ((in = open(file_path, O_RDONLY)) < 0) ERR("open");

    struct stat st;
    stat(file_path, &st);
    int size = st.st_size;
    int bsize = size/n;
    int lsize = size - (n-1)*(bsize);
    char* buffer = (char*)malloc(bsize*sizeof(char));
    int status;

    for (int i = 0; i < n-1; i++)
    {
        
        if (!buffer) ERR("inner_malloc");
        if (bulk_read(in, buffer, bsize) == -1) ERR("bulk_read");
        status = fork();
        switch (status)
        {
        case -1:
            ERR("fork");
            break;
        case 0:
            child_work(buffer, bsize, oldmask, file_path, i);
            free(buffer);
            if (close(in)) ERR("close");
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
        }
    }
    free(buffer);
    buffer = (char*)malloc(lsize*sizeof(char));
    if (!buffer) ERR("last_malloc");
    if (bulk_read(in, buffer, lsize) == -1) ERR("bulk_read");
    status = fork();
    switch (status)
    {
    case -1:
        ERR("fork");
        break;
    case 0:
        child_work(buffer, lsize, oldmask, file_path, n-1);
        free(buffer);
        if (close(in)) ERR("close");
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
    if (close(in)) ERR("close");
    free(buffer);
}

int main(int argc, char* argv[])
{
    if (argc != 3) usage(argc, argv);
    char* file_path = argv[1];
    int n = atoi(argv[2]);
    if (n <= 0 || n >= 10) usage(argc, argv);

    sethandler(sigchld_handler, SIGCHLD);
    sethandler(sig_handler, SIGUSR1);
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    make_children(file_path, n, oldmask);

    if (kill(0, SIGUSR1))
        ERR("kill");
    
    while (wait(NULL) > 0)
            ;
    return EXIT_SUCCESS;
}
