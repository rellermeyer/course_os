/********************************************************************
 *      locks.h
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 21 April 2014
 *
 *      Purpose: implement spin locks
 *
 *
 ********************************************************************/

#ifndef __locks_h_
#define __locks_h_

struct lock
{
    int *lock;
};

void lock(struct lock *l);
void unlock(struct lock *l);

#endif
