#ifndef MEMORY_H
#define MEMORY_H

void* pd_malloc(size_t size);
void* pd_calloc(size_t count, size_t size);
void* pd_realloc(void* ptr, size_t size);
void  pd_free(void* ptr);

#endif
