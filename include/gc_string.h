#ifndef GC_STRING_H
#define GC_STRING_H

#define GC_STRING_MAX 1024
typedef struct {
    int len;
    char string[];
} GC_String;

GC_String *GC_STR_ReadFile(const char* path);


size_t      GC_STR_strcpy_internal(char *dest, const char *src, unsigned int n, unsigned int max, const char* file, int line);
size_t      GC_STR_nstrcpy_internal(char *dest, const char *src, unsigned int i, unsigned int n, unsigned int max, const char* file, int line); 
size_t      GC_STR_GetLastChar_internals(const char *str, const char c, unsigned int n, unsigned int max, const char* file, int line);
int         gc_str_strcmp(const char *str1, const char *str2);
int         gc_str_partialMatch(const char *str1, const char *str2);
size_t      gc_str_strlen(char *str);

#define     GC_STR_strcpy(dest, src, n, max) GC_STR_strcpy_internal(dest, src, n, max, __FILE__, __LINE__)  
#define     GC_STR_GetLastChar(str, c, n, max) GC_STR_GetLastChar_internals(str, c, n, max, __FILE__, __LINE__)
#define     GC_STR_nstrcpy(dest, src, i, n, max) GC_STR_nstrcpy_internal(dest, src, i, n, max, __FILE__, __LINE__)


#endif
