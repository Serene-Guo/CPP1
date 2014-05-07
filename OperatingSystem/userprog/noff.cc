#include "noff.h"
#include "stdio.h"

NoffSegment::NoffSegment(){}
NoffSegment::~NoffSegment(){}
void
NoffSegment::Print()
{
	printf("\t\t%d,\t\t%d,\t\t%d\n",virtualAddr,inFileAddr,size);
}

NoffHeader::NoffHeader(){}
NoffHeader::~NoffHeader(){}



void
NoffHeader::Print()
{
	printf("\n");
	printf("\ttype,\t\tvAddr,\t\tinFAddr,\t\tsize\n");
	printf("\tcode,");
	code.Print();
	printf("\tindt,");
	initData.Print();
	printf("\tunidt,");
	uninitData.Print();
	printf("\n");
}
