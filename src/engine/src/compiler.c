#include "compiler.h"
#ifndef _WIN32
    #include <execinfo.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#ifdef __APPLE__

void pthread_spin_init(OSSpinLock * a_lock, void * a_opts){
    *a_lock = 0;
}

#endif

#ifdef __linux__

void prefetch_range(void *addr, size_t len){
    char *cp;
    char *end = (char*)addr + len;

    for(cp = (char*)addr; cp < end; cp += PREFETCH_STRIDE){
        prefetch(cp);
    }
}

#endif

#if defined(_WIN32)
    #define REAL_PATH_SEP "\\"
    char * get_home_dir(){
        char * f_result = getenv("USERPROFILE");
        sg_assert_ptr(f_result, "getenv(USERPROFILE) returned NULL");
        return f_result;
    }
#else
    #define REAL_PATH_SEP "/"
    char * get_home_dir(){
        char * f_result = getenv("HOME");
        sg_assert_ptr(f_result, "getenv(HOME) returned NULL");
        return f_result;
    }
#endif

NO_OPTIMIZATION void v_self_set_thread_affinity(){
    v_pre_fault_thread_stack(1024 * 512);

#ifdef __linux__
    pthread_attr_t threadAttr;
    struct sched_param param;
    // Subtract 10, as the use likely cannot set that priority
    param.__sched_priority = sched_get_priority_max(SCHED_DEADLINE);
    printf(
        "Attempting to set scheduler = %i, __sched_priority = %i\n",
        SCHED_DEADLINE,
        param.__sched_priority
    );
    pthread_attr_init(&threadAttr);
    pthread_attr_setschedparam(&threadAttr, &param);
    pthread_attr_setstacksize(&threadAttr, 1024 * 1024);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setschedpolicy(&threadAttr, SCHED_DEADLINE);

    pthread_t f_self = pthread_self();
    pthread_setschedparam(f_self, SCHED_DEADLINE, &param);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    pthread_setaffinity_np(f_self, sizeof(cpu_set_t), &cpuset);

    int scheduler;
    pthread_getschedparam(f_self, &scheduler, &param);
    printf(
        "scheduler == %i, __sched_priority == %i\n",
        scheduler,
        param.__sched_priority
    );

    pthread_attr_destroy(&threadAttr);
#endif
}

void v_pre_fault_thread_stack(int stacksize){
#ifdef __linux__
    int pagesize = sysconf(_SC_PAGESIZE);
    stacksize -= pagesize * 20;

    volatile char buffer[stacksize];
    int i;

    for (i = 0; i < stacksize; i += pagesize)
    {
        buffer[i] = i;
    }

    if(buffer[0]){}  //avoid a compiler warning
#endif
}

static void _sg_assert_failed(char* msg){
    if(msg){
        fprintf(stderr, "%s\n", msg);
    } else {
        fprintf(stderr, "Assertion failed, no message provided");
    }
#ifndef _WIN32
    void* callstack[128];
    int frames;

    frames = backtrace(callstack, 128);
    backtrace_symbols_fd(callstack + 2, frames - 2, STDERR_FILENO);
#endif
    abort();
}

void sg_assert(int cond, char* msg){
    if(!cond){
        _sg_assert_failed(msg);
    }
}

void sg_assert_ptr(void* cond, char* msg){
    if(!cond){
        _sg_assert_failed(msg);
    }
}
