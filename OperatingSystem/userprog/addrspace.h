// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
 #include "noff.h"

#define UserStackSize		1024 	// increase this as necessary!
#define StackPages               UserStackSize/SectorSize
#define EveryUserFrame           3       

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code
    void Print();
    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
    void Translate(int addr,unsigned int* vpn, unsigned int *offset); 
    void SwapHandler(int badAddr);



  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space

//添加一个 队列  存储 该用户程序在内存中的 虚拟页号
   int vp_queue[EveryUserFrame];     // 里面存的  是 虚拟页号  
   int que_pointer;
   OpenFile *executable;

   NoffHeader  noffH;



};

#endif // ADDRSPACE_H
