#include "os.h"

/*
 * Following global vars are defined in mem.S
 */
extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/*
 * _alloc_start points to the actual start address of heap pool
 * _alloc_end points to the actual end address of heap pool
 * _num_pages holds the actual max number of pages we can allocate.
 */
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST  (uint8_t)(1 << 1)

/*
 * Page Descriptor 
 * flags:
 * - bit 0: flag if this page is taken(allocated)
 * - bit 1: flag if this page is the last page of the memory block allocated
 */
struct Page {
	uint8_t flags;
};

static inline void _clear(struct Page *page)
{
	page->flags = 0;
}

static inline int _is_free(struct Page *page)
{
	if (page->flags & PAGE_TAKEN) {
		return 0;
	} else {
		return 1;
	}
}

static inline void _set_flag(struct Page *page, uint8_t flags)
{
	page->flags |= flags;
}

static inline int _is_last(struct Page *page)
{
	if (page->flags & PAGE_LAST) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * align the address to the border of page(4K)
 */
static inline uint32_t _align_page(uint32_t address)
{
	uint32_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}

void page_init()
{
	/* 
	 * We reserved 8 Page (8 x 4096) to hold the Page structures.
	 * It should be enough to manage at most 128 MB (8 x 4096 x 4096) 
	 */
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);
	
	struct Page *page = (struct Page *)HEAP_START;
	for (int i = 0; i < _num_pages; i++) {
		_clear(page);
		page++;	
	}

	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

	printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

/*
 * Allocate a memory block which is composed of contiguous physical pages
 * - npages: the number of PAGE_SIZE pages to allocate
 */
void *page_alloc(int npages)
{
	/* Note we are searching the page descriptor bitmaps. */
	int found = 0;
	struct Page *page_i = (struct Page *)HEAP_START;
	for (int i = 0; i < (_num_pages - npages); i++) {
		if (_is_free(page_i)) {
			found = 1;
			/* 
			 * meet a free page, continue to check if following
			 * (npages - 1) pages are also unallocated.
			 */
			struct Page *page_j = page_i;
			for (int j = i; j < (i + npages); j++) {
				if (!_is_free(page_j)) {
					found = 0;
					break;
				}
				page_j++;
			}
			/*
			 * get a memory block which is good enough for us,
			 * take housekeeping, then return the actual start
			 * address of the first page of this memory block
			 */
			if (found) {
				struct Page *page_k = page_i;
				for (int k = i; k < (i + npages); k++) {
					_set_flag(page_k, PAGE_TAKEN);
					page_k++;
				}
				page_k--;
				_set_flag(page_k, PAGE_LAST);
				return (void *)(_alloc_start + i * PAGE_SIZE);
			}
		}
		page_i++;
	}
	return NULL;
}

/*
 * Free the memory block
 * - p: start address of the memory block
 */
void page_free(void *p)
{
	/*
	 * Assert (TBD) if p is invalid
	 */
	if (!p || (uint32_t)p >= _alloc_end) {
		return;
	}
	/* get the first page descriptor of this memory block */
	struct Page *page = (struct Page *)HEAP_START;
	page += ((uint32_t)p - _alloc_start)/ PAGE_SIZE;
	/* loop and clear all the page descriptors of the memory block */
	while (!_is_free(page)) {
		if (_is_last(page)) {
			_clear(page);
			break;
		} else {
			_clear(page);
			page++;;
		}
	}
}

void page_test()
{
	void *p = page_alloc(2);
	printf("p = 0x%x\n", p);
	//page_free(p);

	void *p2 = page_alloc(7);
	printf("p2 = 0x%x\n", p2);
	page_free(p2);

	void *p3 = page_alloc(4);
	printf("p3 = 0x%x\n", p3);
}

/** 
 * malloc block descriptor: A double linked list
 * prev: previous list
 * next: next list
 * bp:	 start address of the block
 * size: size of the block
*/
struct Block 
{
	struct Block *prev;
	struct Block *next;
	void *bp;
	uint32_t size;
};

struct Block *alloc_head = NULL;
struct Block *alloc_tail = NULL;
struct Block *free_head = NULL;
struct Block *free_tail = NULL;

void init_malloc() 
{
	// allocate the first page for storing the linked list
	alloc_head = (struct Block *)page_alloc(1);
	alloc_head->prev = NULL;
	alloc_head->next = NULL;
	alloc_head->bp = NULL;
	alloc_head->size = 0;

	// allocate the free block linked list, half a page size;
	free_head = alloc_head + PAGE_SIZE/2;
	free_head->prev = NULL;
	free_head->next = NULL;
	free_head->bp = NULL;
	free_head->size = 0;
}

void *malloc(size_t size) 
{
	// insufficient allocated linked list
	if (alloc_tail + sizeof(struct Block) == alloc_tail + PAGE_SIZE/2)
	{
		printf("No enough free space for allocated linked list.\n");
		return NULL;
	}

	// first time alloc
	if (alloc_head->bp == NULL && free_head->bp == NULL) 
	{
		// allocate a page for initial free block
		int init_pages = size / PAGE_SIZE + 1;
		alloc_head->bp = page_alloc(init_pages);
		alloc_head->size = size;
		
		free_head->bp = alloc_head->bp + size;
		free_head->size = init_pages * PAGE_SIZE - size;

		alloc_tail = alloc_head;
		free_tail = free_head;

		return alloc_head->bp;
	}

	struct Block *sf = NULL;
	for (sf = free_head; sf != NULL;sf = sf->next)
	{
		if (sf->size >= size)
		{
			void *rp = sf->bp;

			alloc_tail->next = alloc_tail + sizeof(struct Block);
			alloc_tail->next->prev = alloc_tail;
			alloc_tail = alloc_tail->next;
			alloc_tail->bp = rp;
			alloc_tail->size = size;

			if(sf->size == size)
			{
				if (sf == free_head && free_head == free_tail) 
				{
					sf->bp = NULL;
					sf->size = 0;
				} else if(sf == free_head)
				{
					sf->next->prev = NULL;
					free_head = sf->next;
				} else if(sf == free_tail)
				{
					sf->prev->next = NULL;
					free_tail = sf->prev;
				}
			}
			else
			{
				sf->bp = rp + size;
				sf->size = sf->size - size;
			}

			return rp;
		}
	}

	// no free blocks that meet the size
	int new_page_nums = size / PAGE_SIZE + 1;
	alloc_tail->next = alloc_tail + sizeof(struct Block);
	alloc_tail->next->prev = alloc_tail;
	alloc_tail = alloc_tail->next;

	void *new_page = page_alloc(new_page_nums);
	if (new_page == NULL)
	{
		printf("Insufficient memory for allocating new page.\n");
		return NULL;
	}

	alloc_tail->bp = new_page;
	alloc_tail->size = size;

	if (new_page_nums * PAGE_SIZE > size) {
		// insufficient free linked list
		if (free_tail + sizeof(struct Block) == alloc_head + PAGE_SIZE)
		{
			printf("No enough free space for free linked list.\n");
			return NULL;
		}

		free_tail->next = free_tail + sizeof(struct Block);
		free_tail->next->prev = free_tail;
		free_tail = free_tail->next;
		free_tail->bp = new_page + size;
		free_tail->size = new_page_nums * PAGE_SIZE - size;
	}

	return new_page;
}

void free(void *ptr)
{
	struct Block *sa = NULL;

	// search for the allocated linked list;
	for (sa = alloc_head; sa != NULL; sa = sa->next)
	{
		if (sa->bp == ptr)
		{
			void *target_bp = sa->bp;
			uint32_t target_block_size = sa->size;

			if(sa == alloc_head && alloc_head == alloc_tail)
			{
				sa->bp = NULL;
				sa->size = 0;
			}else if(sa == alloc_head) 
			{
				sa->next->prev = NULL;
				alloc_head = sa->next;
			}else if(sa == alloc_tail)
			{
				sa->prev->next = NULL;
				alloc_tail = sa->prev;
			}

			struct Block *sf = NULL;
			for (sf = free_head; sf != NULL; sf = sf->next)
			{
				if (sf->bp + sf->size == target_bp)
				{
					sf->size = sf->size + target_block_size;
					return;
				}
			}

			// create new free block
			// 这里实现的比较粗糙，直接在链表尾添加，实际上可以维护好free链表的地址顺序，然后定期进行free blocks的merge
			// 清理碎片。清理可以是定期的，也可以在没有空间的时候做。这里由于时间关系暂时不做实现
			// insufficient free linked list
			if (free_tail + sizeof(struct Block) == alloc_head + PAGE_SIZE)
			{
				printf("No enough free space for free linked list.\n");
				return NULL;
			}
			free_tail->next = free_tail + sizeof(struct Block);
			free_tail->next->prev = free_tail;
			free_tail = free_tail->next;
			free_tail->bp = target_bp;
			free_tail->size = target_block_size;

			return;
		}
	}

	printf("Invalid ptr.\n");
	return;
}

void display_alloc_list()
{
	printf("Display alloc linked list: \n");
	struct Block *p = NULL;
	for (p = alloc_head; p != NULL; p = p->next)
	{
		printf("p->prev = 0x%d.\n", p->prev);
		printf("p->next = 0x%d.\n", p->next);
		printf("p->bp   = 0x%d.\n", p->bp);
		printf("p->size = %d.\n", p->size);
	}
}

void display_free_list()
{
	printf("Display free linked list: \n");
	struct Block *p = NULL;
	for (p = free_head; p != NULL; p = p->next)
	{
		printf("p->prev = 0x%d.\n", p->prev);
		printf("p->next = 0x%d.\n", p->next);
		printf("p->bp   = 0x%d.\n", p->bp);
		printf("p->size = %d.\n", p->size);
	}
}
