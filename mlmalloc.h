#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifndef MLMALLOC_H
#define MLMALLOC_H
#include <malloc.h>

#define MLON() {__malloc_initialize_hook = ml_init_hook;}
static void ml_init_hook (void);

#endif /* #define MLMALLOC_H */
