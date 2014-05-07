/* noff.h 
 *     Data structures defining the Nachos Object Code Format
 *
 *     Basically, we only know about three types of segments:
 *	code (read-only), initialized data, and unitialized data
 */

#define NOFFMAGIC	0xbadfad 	/* magic number denoting Nachos 
					 * object code file 
					 */
 #ifndef NOFF_H
#define NOFF_H
  

class NoffSegment
{
public:
int virtualAddr;     //在别的地方 需要 经常访问  所以 设为 public 
int inFileAddr;
int size;


NoffSegment();
~NoffSegment();
void Print();

};


class NoffHeader
{
public:
int noffMagic;          /* should be NOFFMAGIC */
NoffSegment code;       /* executable code segment */ 
NoffSegment initData;   /* initialized data segment */
NoffSegment uninitData;  /* uninitialized data segment --
	                  * should be zero'ed before use */
NoffHeader();
~NoffHeader();
void Print();

}; 

/*
  void
NoffSegment::Print()
{

 printf(" 该段 virtualAddr is %d ,inFileAddr is %d ; size is  %d \n;",virtualAddr,inFileAddr,size);


}	

 
NoffSegment::NoffSegment(int a,int b,int c)
{

 virtualAddr=a;     //在别的地方 需要 经常访问  所以 设为 public 
 inFileAddr=b;
 size=c;

}

 */
/*
NoffHeader::NoffHeader()
{
  noffMagic=NOFFMAGIC;               //  should be NOFFMAGIC  
 code=new NoffSegment(0,0,0);      //   executable code segment  
  initData=new NoffSegment(0,0,0);    //  initialized data segment  
  uninitData=new NoffSegment(0,0,0);   

}




  		
  void
NoffHeader::Print()  
{
   printf("code ::\n");
code.Print();

printf("initData::\n");
initData.Print();
printf("uninitData::\n");

uninitData.Print();
   

} 		
   
*/
 #endif	
 
