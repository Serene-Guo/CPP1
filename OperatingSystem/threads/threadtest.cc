// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(_int which)
{
    int num;
    
    for (num = 0; num < 1; num++) {
         printf("\n");
	printf("*** thread %d looped %d times ***priority is %d ", (int) which, num,currentThread->getPri());
//threadtest.cc:30: error: request for member ‘priority’ in ‘currentThread’, which is of non-class type ‘Thread*’
         printf("\n");
        currentThread->Yield();
    }
// printf("-----------------线程%d结束,-----priority is %d；\n",(int) which,currentThread->getPri());
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");
 
    Thread *t1 = new Thread("thread1",10);
    Thread *t2 = new Thread("thread2",1);
    Thread *t3=new Thread("thread3",5);
    Thread *t4=new Thread("thread4",3);
    Thread *t5=new Thread("thread5",7);

   t1->Fork(SimpleThread, 1);
   t2->Fork(SimpleThread, 2);
   t3->Fork(SimpleThread, 3);
   t4->Fork(SimpleThread, 4);
   t5->Fork(SimpleThread, 5);


 //SimpleThread(0);
//SimpleThread(3);

    
}

