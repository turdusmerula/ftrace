#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include <map>
#include <list>


#include <ftrace.h>

#include <boost/lexical_cast.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

using namespace boost::interprocess ;

/**
 * ftrace creates an named pipe ("ftrace").
 * Each process with monitoring capabilities connects to the pipe and give his shared memory id.
 */

int pipefd=-1 ;
managed_shared_memory shm ;

struct MonitorThreadData
{
	ThreadData* threadData ;
} ;

struct MonitorProcessData
{
	ProcessData* processData ;

	std::map<pthread_t, MonitorThreadData*> threads ;
} ;

//List of known processes
std::map<__pid_t, MonitorProcessData*> processes ;


void processCommand(int command) ;

void openPipe()
{
    //open pipe
    if((pipefd=open("/tmp/ftrace.pipe", O_RDONLY))<0)
    {
        perror("Could not open pipe");
        exit(-1) ;
    }
}

int main(int argc, char* argv[])
{
    //Create pipe
    if(mkfifo("/tmp/ftrace.pipe", 0666)<0)
    {
//        perror("ftrace pipe creation failed") ;
//        exit(-1) ;
    }

    //open pipe
    openPipe() ;

    while(true)
    {
    	Commands command=CommandNone ;
    	int nb ;
    	if((nb=read(pipefd, &command, sizeof(uint8_t)))<0)
    	{
    		perror("Error reading pipe") ;
    		exit(-1) ;
    	}

    	if(nb==0)
    	{
    		//When last client detach the read command quits immediately and reads 0 bytes
    		//Close the pipe and reopen it to avoid this problem
    		close(pipefd) ;
    		openPipe() ;
    	}
    	else
    		processCommand(command) ;
    }


    return 0 ;
}

void readCommand(void* data, int size)
{
	if(read(pipefd, data, size)<0)
	{
		perror("Error reading pipe") ;
		exit(-1) ;
	}
}

void commandAttachProcess(Attach &attach)
{
	MonitorProcessData* data=new MonitorProcessData ;
	int shmid ;

	ThreadDataList* threads ;


	std::string shmname="ftrace"+boost::lexical_cast<std::string>(attach.pid) ;
    try {
    	//Retrieve process shared memory, the key is the pid
    	shm = managed_shared_memory(open_only, shmname.c_str()) ;

		//Create process data
    	data->processData = shm.find<ProcessData>("processData").first ;

		//Create threads data
		//threads = shm.find<ThreadDataList>("threads").first ;
    } catch(...) {
        return ;
    }

    //Unlink shared memory, will be deleted when last process disconnect
//    	shared_memory_object::remove(shmname) ;
//	shm_unlink(shmname.c_str()) ;

	processes[attach.pid] = data ;

	std::cout << "Attached new process: " << attach.pid << std::endl ;
}

void commandDetachProcess(Detach &detach)
{
	std::cout << "Detached process: " << detach.pid << std::endl ;

	std::cout << processes[detach.pid]->processData->scopes->size() << std::endl ;

	for(auto& scope : *processes[detach.pid]->processData->scopes)
	{
		std::cout << "Scope: " << scope.second.address << std::endl ;
	}

}

void commandNewThread(NewThread &newthread)
{
	MonitorThreadData* data=new MonitorThreadData ;

	processes[newthread.pid]->threads[newthread.tid] = data ;
	std::cout << "New thread: (process:" << newthread.pid << ") " << newthread.tid << std::endl ;

	for(auto& th : *processes[newthread.pid]->processData->threads.get())
	{
		std::cout << "Shared memory thread: " << th.threadId << std::endl ;
	}
}

void processCommand(int command)
{
	Attach attach ;
	Detach detach ;
	NewThread newthread ;

	switch(command)
	{
	case CommandAttach:
		readCommand(&attach, sizeof(Attach)) ;
		commandAttachProcess(attach) ;
		break ;
	case CommandDetach:
		readCommand(&detach, sizeof(Detach)) ;
		commandDetachProcess(detach) ;
		break ;
	case CommandNewThread:
		readCommand(&newthread, sizeof(NewThread)) ;
		commandNewThread(newthread) ;
		break ;
	default:
		std::cout << "Unknown command: " << command << std::endl ;

	}
}
