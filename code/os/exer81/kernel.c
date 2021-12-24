#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */
extern void uart_init(void);
extern void page_init(void);
extern void init_malloc(void);
extern void display_alloc_list();
extern void display_free_list();

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	page_init();

	init_malloc();

	int *test = malloc(sizeof(int));
	char *test2 = malloc(sizeof(char));
	*test = 100;
	*test2 = 'c';
	printf("test = %d, test2 = %c.\n", *test, *test2);
	free(test);
	printf("test2 = %c.\n", *test2);
	free(test2);

	long *test3 = malloc(sizeof(long));
	*test3 = -12;
	printf("test3 = %d.\n", *test3);
	free(test3);

	display_alloc_list();
	display_free_list();

	while (1) {}; // stop here!
}

