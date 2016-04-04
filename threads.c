#include "threads.h"
#include "list.h"

typedef enum {NEW, RUNNING, WAITING, TERMINATED} thread_state;

struct thread {
	struct page* stack; //Кусочек памяти, у него есть указатель на начало
	void* stack_pointer; //Конец стека, нужен, когда мы передаем управление другому потоку, для него стек начнется здесь
	void* result; //То, что вернет функция
	thread_state state;
}


void schedule() {
	
}


