#include "locks.h"

void lock(struct lock_descriptor *descriptor) {
	local_irq_disable();
	
	while (descriptor->is_locked == 1) {
		shedule;	
	}
	descriptor->is_locked = 1;

	local_irq_enable();
}

void unlock(struct lock_descriptor *descriptor) {
	local_irq_disable();
	descriptor->is_locked = 0;
	local_irq_enable();
}
