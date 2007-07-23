

#ifndef __UTILS_H
#define __UTILS_H


#define ALLOCTEST(x) if((!x)) utils_out_of_memory(__FILE__,__LINE__)

#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

//#define PDEBUG printf
#define PDEBUG  if(0) printf 


typedef void(ForeachFcn)(void *);
typedef void(ForeachPairFcn)(void *);
typedef int(CompareFcn)(void*, void *);

void utils_out_of_memory(char *filename, int lineno);

int utils_file_exist(char *filename);

char *utils_path_join(char *path, char *filename);

struct List;

int utils_file_exists_in_paths(struct List *list, char *filename);


#endif // __UTILS_H
