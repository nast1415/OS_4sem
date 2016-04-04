#pragma once

#include <sys/types.h>
#include "memory.h"
#include "lock.h"

#define MAX_CNT_THREADS 1000

void setup_threads(); //Что-то, что в начале main нужно запустить, чтобы все работало (возможно, не понадобится)
pid_t create_thread(void (*fptr)(void *), void *arg);
void scedule();
pid_t get_current_thread(); //Вернуть id нашего потока, возможно, не нужно
void join(pid_t thread, void** result); //Result - куда сохранить то, что вернет нам поток

