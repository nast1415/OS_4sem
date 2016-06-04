#include "kmem_cache.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "misc.h"
#include "time.h"
#include "threads.h"
#include "file_system.h"
#include "string.h"

static bool range_intersect(phys_t l0, phys_t r0, phys_t l1, phys_t r1)
{
	if (r0 <= l1)
		return false;
	if (r1 <= l0)
		return false;
	return true;
}

static void buddy_smoke_test(void)
{
#define PAGES 10
	struct page *page[PAGES];
	int order[PAGES];

	for (int i = 0; i != PAGES; ++i) {
		page[i] = alloc_pages(i);
		if (page[i]) {
			const phys_t begin = page_paddr(page[i]);
			const phys_t end = begin + (PAGE_SIZE << i);
			printf("allocated [%#llx-%#llx]\n", begin, end - 1);
			order[i] = i;
		}
	}

	for (int i = 0; i != PAGES - 1; ++i) {
		if (!page[i])
			break;
		for (int j = i + 1; j != PAGES; ++j) {
			if (!page[j])
				break;

			const phys_t ibegin = page_paddr(page[i]);
			const phys_t iend = ibegin + (PAGE_SIZE << order[i]);

			const phys_t jbegin = page_paddr(page[j]);
			const phys_t jend = jbegin + (PAGE_SIZE << order[j]);

			DBG_ASSERT(!range_intersect(ibegin, iend, jbegin, jend));
		}
	}

	for (int i = 0; i != PAGES; ++i) {
		if (!page[i])
			continue;

		const phys_t begin = page_paddr(page[i]);
		const phys_t end = begin + (PAGE_SIZE << i);
		printf("freed [%#llx-%#llx]\n", begin, end - 1);
		free_pages(page[i], order[i]);
	}
#undef PAGES
}

struct intlist {
	struct list_head link;
	int data;
};

static void slab_smoke_test(void)
{
#define ALLOCS 1000000
	struct kmem_cache *cache = KMEM_CACHE(struct intlist);
	LIST_HEAD(head);
	int i;

	for (i = 0; i != ALLOCS; ++i) {
		struct intlist *node = kmem_cache_alloc(cache);

		if (!node)
			break;
		node->data = i;
		list_add_tail(&node->link, &head);
	}

	printf("Allocated %d nodes\n", i);

	while (!list_empty(&head)) {
		struct intlist *node = LIST_ENTRY(list_first(&head),
					struct intlist, link);

		list_del(&node->link);
		kmem_cache_free(cache, node);
	}

	kmem_cache_destroy(cache);
#undef ALLOCS
}

void file_system_test() {
    printf("Started file_system tests\n");
    
    fs_node* directory1 = mkdir("lib");
    printf("name of the new directory: %s \n", directory1->name);
    fs_node* directory2 = mkdir("lib/lib2");
    printf("name of the new directory: %s \n", directory2->name);
    
    printf("Open file lib/lib2/a.txt\n");
    fs_node* file1 = open("lib/lib2/a.txt");
    
    char buffer[12];
    char read_buffer[4];
    for (int i = 0; i < 4; i++) {
        read_buffer[i] = 0;
    }

    char buffer2[12];
    char read_buffer2[4];
    for (int i = 0; i < 4; i++) {
        read_buffer2[i] = 0;
    }
    
    memcpy(buffer, "hellokitty", 10);
    buffer[10] = 0;

    printf("Write %s to lib/lib2/a.txt\n", buffer);
    write(file1, 7, 1, buffer);

    printf("Read from lib/lib2/a.txt\n");
    read(file1, 3, 1, read_buffer);
    printf("Read 1..3 chars: %s\n", read_buffer);

    printf("Open file lib/b.txt\n");
    fs_node* file2 = open("lib/b.txt");

    memcpy(buffer2, "helloworld", 10);
    buffer2[10] = 0;

    printf("Write %s to lib/lib2/a.txt\n", buffer2);
    write(file2, 7, 1, buffer2);

    printf("Read from lib/lib2/a.txt\n");
    read(file2, 3, 2, read_buffer2);
    printf("Read 2..5 chars: %s\n", read_buffer2);
    
    readdir(directory1);

    printf("file_system test finished\n");  
}

void main(void)
{
	setup_serial();
	setup_misc();
	setup_ints();
	setup_memory();
	setup_buddy();
	setup_paging();
	setup_alloc();
	setup_time();
	setup_threads();
	local_irq_enable();
	init_file_system();
	

	buddy_smoke_test();
	slab_smoke_test();

	local_irq_enable();
	//threads_test();

	file_system_test();

	while (1);
}
