
/********************************************************************
 *      locks.c
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 21 April 2014
 *
 *      Purpose:  Implement locks
 *
 *
 ********************************************************************/
#include "../include/locks.h"
void lock(struct lock *l)
{
    while(__sync_lock_test_and_set(l->lock, 1));
}
void unlock(struct lock *l)
{
    __sync_synchronize();
    *l->lock = 0;
}
