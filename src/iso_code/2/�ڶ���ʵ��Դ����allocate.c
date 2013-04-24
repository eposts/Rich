/*
			   FILE: ALLOCATE.C
*/

#define	PRIVATE	static	/* Used to hide identifiers from BIND	*/

#define	BLKSIZ	80	/* Size of allocation block	*/
#define	STKSIZ	1000	/* Bytes reserved for stack	*/
#define	MLCSIZ	3000	/* Bytes reserved for malloc()	*/

typedef	union
	{
	char	*link ;
	char	data[BLKSIZ] ;
	} BLOCK ;

PRIVATE	char	*btm_of_heap ;
PRIVATE char	*top_of_heap ;
PRIVATE BLOCK	*heap ;

void Init_Heap()
	{
	unsigned *ptr ;
	unsigned size ;
	unsigned blk ;
	unsigned ttl_blks ;

	freeall(STKSIZ) ;		/* Reserve a stack of 500 words */
	ptr = _memory() + 1 ;		/* Get pointer to heap size	*/
	size = *ptr ;			/* Get size of heap		*/
	size -= MLCSIZ ;		/* Keep 3K for malloc()		*/
	ttl_blks = size/sizeof(BLOCK) ;	/* # available blocks		*/
	size = sizeof(BLOCK)*ttl_blks ;	/* Use an even multiple		*/
	btm_of_heap = malloc(size) ;	/* Reserve the blocks		*/

	heap = btm_of_heap ;		/* Form the free list		*/
	for (blk = 0 ; blk < ttl_blks - 1; blk++)
		{
		heap->link = heap + 1 ;
		heap++ ;
		}
	heap->link = 0 ;		/* Establish end marker		*/
	heap = btm_of_heap ;		/* Restore head pointer		*/
	top_of_heap = &heap[ttl_blks] ;	/* 1st malloc() block		*/
	}


void My_Free(ptr)
BLOCK *ptr ;
	{
	if (ptr >= btm_of_heap)
		{
		if (ptr < top_of_heap)	/* Block is from my heap	*/
			{
			ptr->link = heap ;
			heap = ptr ;
			return ;
			}

		else if (free(ptr))	/* Block is from C-Ware's heap	*/
			{
			return ;
			}
		}

	puts("\nAttempt to free unallocated block!\n\7") ;
	exit(1) ;
	}


char *Allocate(bytes)
unsigned bytes ;
	{
	BLOCK *ptr ;

	if (bytes <= BLKSIZ)		/* Block will fit in my size	*/
		{
		if (heap != 0)		/* Any free blocks?		*/
			{
			ptr = heap ;
			heap = heap->link ;
			return ptr ;
			}
		}

	if (ptr = malloc(bytes))	/* Won't fit or none left!	*/
		{
		return ptr ;
		}

	puts("Insufficient memory: malloc()\n\7") ;
	exit(1) ;
	}

