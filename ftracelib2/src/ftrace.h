#ifndef _FTRACE_API_
#define _FTRACE_API_

#include <stdint.h>
#include <SharedObjects.h>

enum Commands { CommandNone=0, CommandAttach=1, CommandDetach, CommandNewThread } ;

struct Attach
{
	__pid_t pid ;
	size_t shmsize ;
} ;

struct Detach
{
	__pid_t pid ;
} ;

struct NewThread
{
	__pid_t pid ;
	pthread_t tid ;

} ;

void sendCommand(int id, void* command, size_t size) ;

#endif
