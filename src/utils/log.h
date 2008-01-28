
#ifndef __KX_UTILS_LOG
#define __KX_UTILS_LOG

#include "../../config.h"

#ifdef KX_LOG

#define kxmalloc(size) \
        kx_log_malloc((size), __FILE__, __LINE__)

#define kxcalloc(count, size) \
	    kx_log_calloc((count),(size), __FILE__, __LINE__)

#define kxrealloc(old, new_size) kx_log_realloc((old), (new_size))
#define kxfree(ptr) kx_log_free(ptr)


void kx_log_init();
void kx_log_write_raw(char *string);
void kx_log_write(char *action, void *id);
void kx_log_write1(char *action, void *id, char *param);
void kx_log_write_id_i(char *action, void *id, int param);

void * kx_log_malloc(int size, char *file, int linenum);
void * kx_log_calloc(int size, int count, char *file, int linenum);
void kx_log_free(void *ptr);
void * kx_log_realloc(void *ptr, int new_size);

#define KX_LOG_WRITE(action, id) \
		kx_log_write(action, id)

#define KX_LOG_WRITE1(action, id, param) \
		kx_log_write1(action, id, param)

#define KX_LOG_WRITE_ID_I(action, id, param) \
		kx_log_write_id_i(action, id, param)

#else 
// --- not KX_LOG ---

#define KX_LOG_WRITE(action, id)
#define KX_LOG_WRITE1(action, id, param)
#define KX_LOG_WRITE_ID_I(action, id, param)

#define kxmalloc(size) malloc(size)
#define kxcalloc(count,size) calloc((count), (size))
#define kxrealloc(old, new_size) realloc((old), (new_size))
#define kxfree(ptr) free(ptr)


#endif // KX_LOG


#endif // __KX_UTILS_LOG
