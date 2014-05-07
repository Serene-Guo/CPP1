// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
 
#include "bitmap.h"

BitMap *Mmbmp=new BitMap(NumPhysPages); // NumPhysPages   常量在 machine.h 里面定义了 


//BitMap *Mmbmp=new BitMap(NumPhysPages); //bitmap for allocating of physical pages in mainMemory.
BitMap *SwapBitmap = new BitMap(NumPhysPages); //bitmap for SWAP file,
         //assume the size of SWAP file is NumPhyPages.
OpenFile *SwapFile = fileSystem->Open("SWAP"); //stub in Nachos_Linux
        //在lab11的目录下建立文件SWAP

//extern BitMap *Mmbmp;     不行的话，在改到 progtest文件里面   －－－－－－－－－－－－－－－－－－－－－－－－－实验证明  可以！！！！！！！！！

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------




void
AddrSpace::SwapHandler(int badAddr)
{


unsigned int oldPage = vp_queue[que_pointer];
unsigned int newPage;   // 是虚拟页号
unsigned int temp;

Translate(badAddr, &newPage,&temp);   // 求得 虚拟页号 与 偏移量 

ASSERT(newPage < numPages);
printf("\n发生页错误的虚拟页号newPage is %d， type is %d\n",newPage,pageTable[newPage].type);
vp_queue[que_pointer]=newPage;    //加入队列
que_pointer = (que_pointer + 1) % EveryUserFrame;   

printf("\nswap vm page: 虚拟页%d==>%d,对应的物理内存页 %d\n",
   pageTable[oldPage].virtualPage,pageTable[newPage].virtualPage,pageTable[oldPage].physicalPage);






//    将  旧页  换出 ，  新页  换入－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
//  如果 oldPage 被修改过  则 先将 oldPage 写回磁盘  
if(pageTable[oldPage].dirty)
{
  switch(pageTable[oldPage].type)
   {
     case vmcode:
     case vminitData:
     ASSERT(pageTable[oldPage].type!=vmcode);
     ASSERT(pageTable[oldPage].type!=vminitData);
     executable->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage * PageSize]),PageSize, pageTable[oldPage].inFileAddr);
     break;
     case vmuninitData:
     case vmuserStack:
      pageTable[oldPage].inFileAddr = (SwapBitmap->Find()) * PageSize;
      //注意，此处正是前文所指，当vmuninitData和vmuserStack的inFileAddr>=0时，表明此页已经被修改过，再次读入时需要将其从SWAP中读取。
 //将 内存的 内容写 到 SWAP  中 。。。。。。。。
     SwapFile->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage * PageSize]),PageSize, pageTable[oldPage].inFileAddr);   
     break;
   }

   pageTable[oldPage].dirty = FALSE;
}

// 处理 交接过程－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－

pageTable[newPage].physicalPage = pageTable[oldPage].physicalPage;
pageTable[oldPage].physicalPage = -1;
pageTable[oldPage].valid = FALSE;

pageTable[newPage].valid = TRUE;
pageTable[newPage].use = TRUE;
pageTable[newPage].dirty = FALSE;



//  将 newPage 内容 装载到 内存－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
switch(pageTable[newPage].type)
{
   
   case vmcode:
   case vminitData:

   printf("copy from source file where :%d =====> mainMemorty[%d]\n", pageTable[newPage].inFileAddr, pageTable[newPage].physicalPage * PageSize);
   
    executable->ReadAt(&(machine->mainMemory[pageTable[newPage].physicalPage * PageSize]),PageSize, pageTable[newPage].inFileAddr);
    break;
   case vmuninitData:
   case vmuserStack:
   if(pageTable[newPage].inFileAddr >= 0)
    { //说明  该 页  之前 被用过 所以 要在 SWAP 中 取出来 
     printf("copy from swap file 地址是 :%d =====> mainMemorty[%d]\n", pageTable[newPage].inFileAddr, pageTable[newPage].physicalPage * PageSize);
     SwapFile->ReadAt(&(machine->mainMemory[pageTable[newPage].physicalPage * PageSize]),PageSize, pageTable[newPage].inFileAddr);
//从交换区读到内存，然后交换区则清零  
     SwapBitmap->Clear(pageTable[newPage].inFileAddr / PageSize);
     pageTable[newPage].inFileAddr = -1;
 
    }
     else 
     bzero(machine->mainMemory + pageTable[newPage].physicalPage * PageSize, PageSize);  
     //说明新页为 用户堆栈 但还没有被修改过，所以分配给他的物理内存 先清零
    //将mainMemory中分配给newPage的物理空间清零
    break;
}


Print();  //交换 完之后 打印 页表

}
void
AddrSpace::Translate(int addr, unsigned int* vpn, unsigned int *offset)
{ 
int page = -1;
int off = 0; 
if(addr >= numPages * PageSize - UserStackSize)
//addr位于pageTable的userStack段中
{
   int userPages = numPages - StackPages;
   page = userPages + (addr - userPages * PageSize) / PageSize;
   off = (addr - userPages * PageSize) % PageSize; 
}
else if(noffH.uninitData.size > 0 && addr >= noffH.uninitData.virtualAddr)
//addr位于uninitData
{
   page = divRoundUp(noffH.code.size, PageSize) + divRoundUp(noffH.initData.size, PageSize) + 
    (addr-noffH.uninitData.virtualAddr) / PageSize;
   off = (addr - noffH.uninitData.virtualAddr) % PageSize;
}
else if(noffH.initData.size> 0 && addr >= noffH.initData.virtualAddr)
{
//addr位于initData
   page = divRoundUp(noffH.code.size, PageSize) + (addr-noffH.initData.virtualAddr) / PageSize;
   off = (addr - noffH.initData.virtualAddr) % PageSize;
}
else
{
//addr位于code中
   page = addr / PageSize;
   off = addr % PageSize;
}
*vpn = page;
*offset = off;
// printf("vpn is %d, offset is %d\n",*vpn,*offset);
}




static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------



//may be need to add Print in ../usrprog/addrspace.h?!
void AddrSpace::Print(){

printf("page table dump:%d pages in total\n",numPages);
printf("===========================================================\n");
printf("\tvPage,\tmPage,\tinFile,\tvalid,\tuse,\tdirty\ttype\n");
for(int i=0;i<numPages;i++){
    printf("\t%d,\t%d,\t%d,\t%d,\t%d,\t%d\t%d\n",
		pageTable[i].virtualPage, pageTable[i].physicalPage,
		pageTable[i].inFileAddr, pageTable[i].valid, pageTable[i].use, pageTable[i].dirty,
		pageTable[i].type);
}
printf("===========================================================\n");
}

AddrSpace::AddrSpace(OpenFile *exe)
{
  
         unsigned int i, size;

         executable=exe;

        // noffH=new NoffHeader();


 // noffH 是 似有属性
    exe->ReadAt((char *)&noffH, sizeof(noffH), 0);

printf("\n");
//printf("noffH.code.size is %d initdata is %d uninitData is %d \n",noffH.code.size,noffH.initData.size,noffH.uninitData.size);
//代码 段  是 256  

    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))

    	SwapHeader(&noffH);

    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    //size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;	// we need to increase the size
						// to leave room for the stack
    size = divRoundUp(noffH.code.size,PageSize) * PageSize   //保证 code    initData都是 从 页的 开始 
            + divRoundUp(noffH.initData.size, PageSize) * PageSize 
            + noffH.uninitData.size 
            + UserStackSize;
  


    numPages = divRoundUp(size, PageSize);     //该用户程序 需要 numPages 个 物理桢
    size = numPages * PageSize;

 printf("code  %d \t initData %d \t uninitData %d \t StackPages is %d\n ",divRoundUp(noffH.code.size,PageSize),
                divRoundUp(noffH.initData.size, PageSize),
               divRoundUp(noffH.uninitData.size, PageSize),
               StackPages);

 printf("空闲物理侦数 %d\n\n",Mmbmp->NumClear());
// 故意弄得 
Mmbmp->Mark(0);
Mmbmp->Mark(10);
Mmbmp->Mark(11);
Mmbmp->Mark(28);
 printf("\n随意标识0、10、11、28之后，空闲物理侦数 %d\n",Mmbmp->NumClear());
    
//虚拟内存管理 不需要考虑 用户程序所需要的最大的 页数 是否  < 空闲物理页 
 
    DEBUG('a', "Initializing address space, num pages %d, size %d\n",numPages,size);

int zeroSize=EveryUserFrame*SectorSize+UserStackSize;
 bzero(machine->mainMemory,zeroSize); //  清空内存 中 该用户程序所需要的 物理桢


// first, set up the translation 
que_pointer=0;   //队列 指针


 
    pageTable = new TranslationEntry[numPages];
 

//  一个大循环   设置 除了 type 和 infileAddr之外的所有信息   如果 小于 EveryUserFrame  则分配 
 for (int i = 0; i < numPages; i++) 
    {
   pageTable[i].virtualPage = i; // for now, virtual page # = phys page #
   pageTable[i].use = FALSE;
   pageTable[i].dirty = FALSE;
   pageTable[i].readOnly = FALSE; // if the code segment was entirely on  a separate page, we could set its   pages to be read-only
   pageTable[i].inFileAddr = -1;      //具体 值  在 后面计算  ！！！！！！！！！！！！
 
   if(i >= numPages - StackPages)
    pageTable[i].type = vmuserStack;    // 一开始 注释掉了这句话    在进行页面替换的时候，报错
  
   if(i < EveryUserFrame)
   {
    vp_queue[que_pointer] = pageTable[i].virtualPage;//  其实就是 ==i    向  队列中  加入 虚拟页号 
    que_pointer = (que_pointer + 1) % EveryUserFrame;
    pageTable[i].physicalPage = Mmbmp -> Find ();
    pageTable[i].valid = TRUE;
   }
   else{
    pageTable[i].physicalPage = -1;
    pageTable[i].valid = FALSE;
   }


    }
 
 //下面 设置 infileAddr    type  并且 把以分配号的 虚拟页  写入内存-----------------------------------------------------------------------------------------------


   DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size); 
 if (noffH.code.size > 0) 
    {
   unsigned int numP = divRoundUp(noffH.code.size, PageSize);
  
   for (int i = 0; i < numP; i++)
   {
    pageTable[i].inFileAddr = noffH.code.inFileAddr + i * PageSize;
    pageTable[i].type = vmcode;
    if(pageTable[i].valid)
    {
   executable->ReadAt( &(machine->mainMemory[pageTable[i].physicalPage * PageSize]),PageSize, pageTable[i].inFileAddr);
    //If the page has been allocated with physical page, read into the mainMemory
    }
   }
}


//考虑 initData 
 
  
if (noffH.initData.size > 0)
    {
   unsigned int numP,firstP;   
   numP = divRoundUp(noffH.initData.size, PageSize);
   firstP = divRoundUp(noffH.initData.virtualAddr, PageSize);   //first=divRoundUp(noffH.code.size, PageSize); 
//因为是roundUp  所以firstP  指的是  initdata 所在的第一个完整的扇区。。。。
   for (int i = firstP; i < numP + firstP; i++)
   {
//printf("\n initData  is %d\n",noffH.initData.inFileAddr);
    pageTable[i].inFileAddr = noffH.initData.inFileAddr + (i - firstP) * PageSize;  
printf("pageTable[%d] infileAddr is %d\n",i,pageTable[i].inFileAddr);

    pageTable[i].type = vminitData;
    if(pageTable[i].valid)
    {
   executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]),PageSize, pageTable[i].inFileAddr);
 
    }
   }
    }


if(noffH.uninitData.size > 0)
{
   unsigned int numP,firstP;
   numP = divRoundUp(noffH.uninitData.size, PageSize);
   firstP = divRoundUp(noffH.uninitData.virtualAddr, PageSize);  //是否是 noffH.uninitData.inFileAddr   不对  因为 该段  没有 在 文件中 ，，，，，看../bin/noff.h 文件中定 

   for (int i = firstP; i < numP + firstP; i++)
   {
    pageTable[i].type = vmuninitData;
    if(pageTable[i].valid)
      {/*brzero();*/}
   
    }  //for 循环 结束 
  

}


 
 
//  装载 完了   3  个 物理页  每个用户进程 就 分配三个 
Print();


printf("\n装载完毕，下面开始run......\n");

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete [] pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
