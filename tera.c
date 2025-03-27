void parent_work(mqd_t pout)
{
    srand(getpid());

    struct timespec waiting_for_queue;
    clock_gettime(CLOCK_REALTIME, &waiting_for_queue);
    waiting_for_queue.tv_sec += 1;

    while (children_left)
    {
        uint8_t ni = (uint8_t)(rand() % MAX_NUM);
        if (TEMP_FAILURE_RETRY(mq_timedsend(pout, (const char *)&ni, 1, 0, &waiting_for_queue)) && errno != ETIMEDOUT)
            ERR("mq_send");
        
        struct timespec time;
        time.tv_sec = 0;
        time.tv_nsec = 10000000;
        nanosleep(&time, NULL);
    }
    printf("[PARENT] Terminates \n");
}
