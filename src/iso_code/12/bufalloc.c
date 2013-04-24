


#include <stdlib.h>




void *bufalloc(size_t *size, size_t minsize)
{
void *buffer;	
size_t bufsize;	


for (bufsize = *size; bufsize >= minsize && !(buffer = malloc(bufsize)); bufsize /= 2);


*size = bufsize;

return (buffer);
}
