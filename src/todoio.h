
#ifndef GC_TODOIO_H
#define GC_TODOIO_H

#include <stdio.h>

#ifdef __cplusplus
#define RESTRICT
#else
#define RESTRICT restrict
#endif

#define fwrite_align8(str, size, count, file) aligned_fwrite(str, size, count, 8, file)
#define fread_align8(str, size, count, file) aligned_fread(str, size, count, 8, file)
#define fwrite_pad8(bytes, file) fwrite_padding(bytes, 8, file)
#define fread_pad8(bytes, file) fread_padding(bytes, 8, file)

void aligned_fwrite(const void * restrict str, size_t size, size_t count, size_t alignment, FILE *file);
void aligned_fread(void * restrict str, size_t size, size_t count, size_t alignment, FILE *file);

void fwrite_padding(size_t bytesWritten, size_t alignment, FILE *file);
void fread_padding(size_t bytesRead, size_t alignment, FILE *file);

char *todo_getinput(char *buffer, int maxCount);

#endif // GC_TODO_H included
