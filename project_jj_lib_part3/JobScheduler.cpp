//
// Created by wmsksf on 14/01/20.
//

#include "JobScheduler.h"

JobScheduler::JobScheduler() : WorkerIsRunning{true}
{
    pthread_mutex_init(&WorkerMtx,nullptr);
    pthread_cond_init(&WorkerCV,nullptr);
}

JobScheduler::~JobScheduler()
{
    s_cout{} << "~JobScheduler...\n";

    pthread_mutex_destroy(&WorkerMtx);
    pthread_cond_destroy(&WorkerCV);

    s_cout{} << "~JobScheduler.\n";
}

void JobScheduler::init(int threads)
{
    ThreadPoolSize = threads;
    pthread_t thread;
    for (int i = 0; i <ThreadPoolSize; ++i) {
        pthread_create(&thread,nullptr,worker_thread, (void*)this);
        ThreadPool.push_back(thread);
    }
}

void JobScheduler::stop()
{
    s_cout{} << "stop...\n";

    pthread_mutex_lock(&WorkerMtx);
    WorkerIsRunning = false;
    pthread_cond_broadcast(&WorkerCV);
    s_cout{} << "broadcast...\n";
    pthread_mutex_unlock(&WorkerMtx);
    s_cout{} << "join...\n";
    for (auto &t : ThreadPool)
        pthread_join(t, nullptr);

    s_cout{} << "stop.\n";
}

void JobScheduler::schedule(Job &Job)
{
    pthread_mutex_lock(&WorkerMtx);
    JobsQueue.push(&Job);
    pthread_cond_signal(&WorkerCV);
    pthread_mutex_unlock(&WorkerMtx);
}


void JobScheduler::barrier()
{

}

void* worker_thread(void *arg)
{
    auto* scheduler = (JobScheduler*)arg;
    for (;;)
    {
        pthread_mutex_lock(&(scheduler->WorkerMtx));

        while (!scheduler->JobsQueue.size() && scheduler->WorkerIsRunning) {
            pthread_cond_wait(&(scheduler->WorkerCV), &(scheduler->WorkerMtx));
        }
        if (!scheduler->WorkerIsRunning && !scheduler->JobsQueue.size()) {
            pthread_mutex_unlock(&(scheduler->WorkerMtx));
            break;
        }

        Job *p = scheduler->JobsQueue.front();
        scheduler->JobsQueue.pop();
        pthread_mutex_unlock(&(scheduler->WorkerMtx));

        p->run();
        delete p;
    }
}

job::job(int n) : n{n} { }
void job::run()
{
    s_cout{} << "Start Job " << n << std::endl;
    sleep(2);
    s_cout{} << " Stop Job " << n << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
pthread_mutex_t s_cout::s_mux{};
s_cout::~s_cout()
{
    pthread_mutex_lock(&s_mux);
    std::cout << this->str();
    pthread_mutex_unlock(&s_mux);
}