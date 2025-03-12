#define _GNU_SOURCE
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NUMBER_OF_PLAYERS 10
#define HP 100

#define ERR(source) \
    (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))


int sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        return -1;
    return 0;
}

void sigchld_handler(int sig)
{
    pid_t pid;
    for (;;)
    {
        pid = waitpid(0, NULL, WNOHANG);
        if (0 == pid)
            return;
        if (0 >= pid)
        {
            if (ECHILD == errno)
                return;
            ERR("waitpid:");
        }
    }
}

void player_work(int my_pipe, int* enemy_pipes, int team_number)
{
    pid_t pid = getpid();
    printf("[%d] druzyna %d: gotowy do gry\n", pid, team_number);
}

void create_children_and_pipes()
{
    int* t1_pipes = (int*)malloc(NUMBER_OF_PLAYERS * sizeof(int) * 2);
    int* t2_pipes = (int*)malloc(NUMBER_OF_PLAYERS * sizeof(int) * 2);

    for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        if (pipe(&(t1_pipes[2 * i])))
            ERR("pipe");
    }
    for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        if (pipe(&(t2_pipes[2 * i])))
            ERR("pipe");
    }

    for (int i = 1; i < NUMBER_OF_PLAYERS; i++) // 1, bo stary tez gra
    {
        switch (fork())
        {
            case 0:
                for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
                {
                    if (j == i)
                    {
                        if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j + 1]))) // write do siebie
                            ERR("close");
                        continue;
                    }

                    if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j])))
                        ERR("close");
                    if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j + 1])))
                        ERR("close");
                    if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j])))
                        ERR("close");
                }

                player_work(t1_pipes[2* i], t2_pipes, 1);

                for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
                {
                    if (j == i)
                    {
                        if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j]))) // read do siebie
                            ERR("close");
                        continue;
                    }
                    if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j + 1])))
                        ERR("close");
                }
                exit(EXIT_SUCCESS);

            case -1:
                ERR("Fork:");
        }
    }

    for (int i = 0; i < NUMBER_OF_PLAYERS; i++) // 1, bo stary tez gra
    {
        switch (fork())
        {
            case 0:
                for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
                {
                    if (j == i)
                    {
                        if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j + 1]))) // write do siebie
                            ERR("close");
                        continue;
                    }

                    if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j])))
                        ERR("close");
                    if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j + 1])))
                        ERR("close");
                    if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j])))
                        ERR("close");
                }

                player_work(t2_pipes[2 * i], t1_pipes, 2);

                for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
                {
                    if (j == i)
                    {
                        if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j]))) // read do siebie
                            ERR("close");
                        continue;
                    }
                    if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j + 1])))
                        ERR("close");
                }
                exit(EXIT_SUCCESS);
                
            case -1:
                ERR("Fork:");
        }
    }

    // konfiguracja starego
    for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
    {
        if (j == 0)
        {
            if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j + 1]))) // write do siebie
                ERR("close");
            continue;
        }

        if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j])))
            ERR("close");
        if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j + 1])))
            ERR("close");
        if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j])))
            ERR("close");
    }

    player_work(t1_pipes[0], t2_pipes, 1);

    for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
    {
        if (j == 0)
        {
            if (TEMP_FAILURE_RETRY(close(t1_pipes[2 * j]))) // read do siebie
                ERR("close");
            continue;
        }
        if (TEMP_FAILURE_RETRY(close(t2_pipes[2 * j + 1])))
            ERR("close");
    }
}

int main(int argc, char** argv)
{
    // if (sethandler(SIG_IGN, SIGPIPE))
    //     ERR("Setting SIGINT handler");
    // if (sethandler(sigchld_handler, SIGCHLD))
    //     ERR("Setting parent SIGCHLD:");
    create_children_and_pipes();

    int counter = 0;
    while (wait(NULL) > 0) {
        counter++;
    }
    printf("Waited %d children.\n", counter);
    return EXIT_SUCCESS;
}
