#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <sys/types.h>
#include <sys/param.h>

//Global variable used to deactivate the memory callback
extern bool SystemAllocator ;

class Allocator ;
extern Allocator* allocator ;

#endif
