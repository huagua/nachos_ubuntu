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
#include "elevatortest.h"

// testnum is set in main.cc
int testnum = 7;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

void
Lab1Exercise3Thread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d (uid=%d, tid=%d) looped %d times\n", 
which, currentThread->getUid(), currentThread->getTid(), num);
        currentThread->Yield();
    }
}

void
Lab1Exercise3()
{
    DEBUG('t', "Entering Lab1Exercise3");

    const int max_threads = 5;
    const int test_uid = 87;

    for (int i = 0; i < max_threads; i++) {
        // Generate a Thread object
        Thread *t = new Thread("forked thread");
        t->setUid(test_uid); // set uid
        // Define a new thread's function and its parameter
        t->Fork(Lab1Exercise3Thread, (void*)t->getTid());
    }

    Lab1Exercise3Thread(0);
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------


void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, (void*)1);
    SimpleThread(0);
}

void
Lab1Exercise4_1()
{
    DEBUG('t', "Entering Lab1Exercise4_1");

    const int max_threads = 130;

    for (int i = 0; i < max_threads; i++) {
        // Generate a Thread object
        Thread *t = new Thread("forked thread");
        printf("*** thread name %s (tid=%d)\n", t->getName(), t->getTid());
    }
}

void 
TS()
{
	    DEBUG('t', "Entering TS");

    printf("Threads list contents is as follows: \n");
    const char* TStoString[] = {"JUST_CREATED", "RUNNING", "READY", "BLOCKED"};

    printf("UID\tTID\tNAME\tPRI\tSTATUS\n");
    for (int i = 0; i < maxThreadsCount; i++) { // check pid flag
        if (threads[i]) {
            printf("%d\t%d\t%s\t%d\t%s\n", pointThreads[i]->getUid(), pointThreads[i]->getTid(), 
pointThreads[i]->getName(), pointThreads[i]->getPriority(), TStoString[pointThreads[i]->getStatus()]);
        }
    }
}

void
CustomThreadFunc(int which)
{
    printf("*** current thread (uid=%d, tid=%d, priority=%d, name=%s) => ",
 currentThread->getUid(), currentThread->getTid(),currentThread->getPriority(),currentThread->getName());
    IntStatus oldLevel; // for case 1 sleep (avoid cross initialization problem of switch case)
    switch (which)
    {
        case 0:
            printf("Yield\n");
            scheduler->Print();
            printf("\n\n");
            currentThread->Yield();
            break;
        case 1:
            printf("Sleep\n");
            oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
            currentThread->Sleep();
            (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
            break;
        case 2:
            printf("Finish\n");
            currentThread->Finish();
            break;
        default:
            printf("Yield (default)\n");
            currentThread->Yield();
            break;
    }
}

void
Lab1Exercise4_2()
{
    DEBUG('t', "Entering Lab1Exercise4_2");

    Thread *t1 = new Thread("fork 1");
    Thread *t2 = new Thread("fork 2");
    Thread *t3 = new Thread("fork 3");

    t1->Fork(CustomThreadFunc, (void*)0);
    t2->Fork(CustomThreadFunc, (void*)1);
    t3->Fork(CustomThreadFunc, (void*)2);

    Thread *t4 = new Thread("fork 4");


    CustomThreadFunc(0); // Yield the current thread (i.e. main which is defined in system.cc)

    printf("--- Calling TS command ---\n");
    TS();
    printf("--- End of TS command ---\n");
}

void
Lab2Exercise3_1()
{
    DEBUG('t', "Entering Lab2Exercise3_1");

    Thread *t1 = new Thread("with p", 87);

    Thread *t2 = new Thread("set p");
    t2->setPriority(100);

    Thread *t3 = new Thread("no p");

    t1->Fork(CustomThreadFunc, (void*)0);
    t2->Fork(CustomThreadFunc, (void*)0);
    t3->Fork(CustomThreadFunc, (void*)0);

    CustomThreadFunc(0); // Yield the current thread

    printf("--- Calling TS command ---\n");
    TS();
    printf("--- End of TS command ---\n");
}

void
Lab2Exercise3_2()
{
    DEBUG('t', "Entering Lab2Exercise3_2");

    Thread *t1 = new Thread("lower", 78);
    Thread *t2 = new Thread("highest", 87);
    Thread *t3 = new Thread("lowest", 38);

    t1->Fork(CustomThreadFunc, (void*)0);
    t2->Fork(CustomThreadFunc, (void*)0);
    t3->Fork(CustomThreadFunc, (void*)0);

    CustomThreadFunc(0);

    printf("--- Calling TS command ---\n");
    TS();
    printf("--- End of TS command ---\n\n");
}

void
ThreadWithTicks(int runningTime)
{
    int num;
    
    for (num = 0; num < runningTime * SystemTick; num++) {
        printf("*** thread with running time %d looped %d times (stats->totalTicks: %d)\n",
 runningTime, num+1, stats->totalTicks);
        interrupt->OneTick();
    }

    currentThread->Finish();
}

void
Lab2ChallengeRR()
{
    DEBUG('t', "Entering Lab2ChallengeRR");

    printf("\nSystem initial ticks:\tsystem=%d, user=%d, total=%d\n", 
stats->systemTicks, stats->userTicks, stats->totalTicks);

    Thread *t1 = new Thread("7");
    Thread *t2 = new Thread("2");
    Thread *t3 = new Thread("5");

    printf("\nAfter new Thread ticks:\tsystem=%d, user=%d, total=%d\n",
 stats->systemTicks, stats->userTicks, stats->totalTicks);

    t1->Fork(ThreadWithTicks, (void*)7);
    t2->Fork(ThreadWithTicks, (void*)2);
    t3->Fork(ThreadWithTicks, (void*)5);

    printf("\nAfter 3 fork() ticks:\tsystem=%d, user=%d, total=%d\n\n", 
stats->systemTicks, stats->userTicks, stats->totalTicks);

    scheduler->lastSwitchTick = stats->totalTicks;
    currentThread->Yield();
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {

    	case 1:
	ThreadTest1();
	break;

	case 2:
        printf("Lab1 Exercise3:\n");
        Lab1Exercise3();
        break;

	case 3:
        printf("Lab1 Exercise4-1:\n");
        Lab1Exercise4_1();
	break;

	case 4:
        printf("Lab1 Exercise4-2:\n");
        Lab1Exercise4_2();
        break;

	case 5:
	printf("lab2 Exercise3-1:\n");
	Lab2Exercise3_1();
	break;

	case 6:
	printf("lab2 Exercise3-2:\n");
	Lab2Exercise3_2();
	break;

	case 7:
        printf("Lab2 Challenge RR:\n");
        printf("(don't forget to add `-rr` argument to activate timer)\n");
        Lab2ChallengeRR();
        break;

    default:
	printf("No test specified.\n");
	break;
    }
}

