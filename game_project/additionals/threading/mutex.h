#ifndef MUTEX_H
#define MUTEX_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Mutex
{
public:
	Mutex( );
	~Mutex( );
	void lock( );
	bool trylock( );
	void unlock( );
	//private:

	CRITICAL_SECTION lck;
};
#endif