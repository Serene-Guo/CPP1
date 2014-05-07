// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#include "noff.h"

 #include "bitmap.h"   //必须得有-------------------------------
 extern BitMap *Mmbmp;  // 在 Addrspace.cc 中 定义的 
extern BitMap *SwapBitmap; //bitmap for SWAP file,
         //assume the size of SWAP file is NumPhyPages.
extern OpenFile *SwapFile; //stub in Nachos_Linux
//extern char *fn;

extern void
StartProcess(char *filename);//自己加的  在 progtest.cc 文件中   

//BitMap *Mmbmp=new BitMap(NumPhysPages); // NumPhysPages   常量在 machine.h 里面定义了 
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    printf("\n exception type is %d\n",type);
if (which == SyscallException) 
{
     if((type == SC_Halt))
      {DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
          }
       else if(type == SC_Exec)
       { //处理 系统调用 Exec
        printf("\nExpected Exec exception %d %d\n", which, type); 
        int paraddr = machine->ReadRegister(4); 
        printf("Parameter address is %d\n",paraddr);
        char fn[40];
        for(int i=0;i<10;i++){
//printf("@@try to read %d\n",paraddr+4*i);
            machine->ReadMem(paraddr+4*i,4,(int *)(fn+4*i));  // 在 mechine.h 文件中  在translate.cc   中实现 
        }
 
//string str="../test/exec.noff";
//strcpy(fn,str.c_str());
      
                printf("Exec file is %s\n",fn); 
//               StartProcess(str.c_str());
                   StartProcess(fn); 
                 machine->registers[PCReg] = machine->registers[NextPCReg];
      } 

}


else if(which==PageFaultException)
{ //  添加  处理 页错误的 过程
  
    int vaddr2=machine->registers[BadVAddrReg];    /// 好像 这一句 就够了   不用读 寄存器 ！！  看一下 machine 的 readRegster  其实函数里面  封装了 这个  呵呵呵呵呵呵～～～～～～ 

  printf(" \n\nbadAddr is %d\n \n\n",vaddr2);
 // int vpn=(unsigned)vaddr2/PageSize;     // #define PageSize     SectorSize     －－－－－－－－－－－－－－－－－－－－ machine.h 文件中 
//这样 并不对  
//  必须 将 下面的代码  写道 addrSpace 里面 不然 很多 属性 是没法用的  用的不方便 

currentThread->space->SwapHandler(vaddr2); // 处理 交换 //这部分是 重头戏 ！！！！！！！！！！！！！！！！！！！！！！！

    stats->numPageFaults++;
   machine->registers[NextPCReg] = machine->registers[PCReg];
   machine->registers[PCReg]-=4;
   printf("PCReg = %d, NextPCReg = %d\n",machine->registers[PCReg],machine->registers[NextPCReg]);

//  如何 将 内容 写到 内存里！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//下面是 之前的 分配号 物理页之后 写道内存里去    translate.cc 文件里 有个 writeMem 但 只能 写 几个 BYTE 所以不行！！！！！
  //executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize, noffH.code.inFileAddr + i * PageSize);
 
//   OpenFile *executable = fileSystem->Open("halt.noff");
 //  NoffHeader noffH2;
    // executable->ReadAt((char *)&noffH2, sizeof(noffH2), 0);
 //printf("Exec file is  ?????????????????????????---------------------------------from  exception.cc\n" ); 
//没有 碎片
//executable->ReadAt(&(machine->mainMemory[machine->pageTable[vpn].physicalPage * PageSize]), PageSize, vpn * PageSize)

}
  

else   {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
      }

   
}
