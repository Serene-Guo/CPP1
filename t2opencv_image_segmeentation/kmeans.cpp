#include "kmeans.h"
#include <stdlib.h>

#include <iostream>
using namespace std;
#define random(x) (rand()%x)
 


mykmeans::mykmeans(int kcluster, int sourceLengthPara, double* sourceVector)
{
	k=kcluster;
	source=sourceVector;
	sourceLength=sourceLengthPara;

	if(sourceLength==0)
		cout<<"the size of source vector is zero."<<endl;
	clusterBitmap=new int[sourceLength];
	meansOfClusters= new double [k*3];
}

void mykmeans::initializeClusters()
{
	int index=0;
	
	 for(int i=0;i<k;i++)
			{
			index=random(sourceLength);
			//cout<<index<<" ";
			meansOfClusters[3*i]=source[index*3];
			meansOfClusters[3*i+1]=source[index*3+1];
			meansOfClusters[3*i+2]=source[index*3+2];
			}
	 
}

void  mykmeans::calClusterforVector()
	//assign initial clusters
{
	//cout<<"cal cluster: "<<endl;
	for(int i=0;i<sourceLength;i++)
	{// for every source , assign a cluster
		double dis=100000000000;
		 
		
		for(int j=0;j<k;j++)
		{	
			double temp= (meansOfClusters[j*3]-source[i*3])* (meansOfClusters[j*3]-source[i*3])+
		(meansOfClusters[j*3+1]-source[i*3+1])*(meansOfClusters[j*3+1]-source[i*3+1])+
		(meansOfClusters[j*3+2]-source[i*3+2])*(meansOfClusters[j*3+2]-source[i*3+2]);
	 
			if(temp<dis)
			{
				dis=temp;
		 
				clusterBitmap[i]=j;
				
			}
		}
 
	} //set the cluster 


}

void mykmeans::outputmeans()
{
	 
	for(int i=0;i<64;i++)
		cout<< meansOfClusters[i*3]<<" "<< meansOfClusters[i*3+1]<<" "<< meansOfClusters[i*3+2]<<endl;
}
void mykmeans::calmeans()
{
	double temp[3];  //// 之前 把 unsigned char 赋值给了 char 
	 
	for(int i=0;i<k;i++)
	{    
		int count=0;
		temp[0]=0;
		temp[1]=0;
		temp[2]=0;
		for(int j=0;j<sourceLength;j++)
		{  
			if(clusterBitmap[j] == i)
				{  
		  
						temp[0] = temp[0] + source[j*3];
						temp[1] = temp[1] + source[j*3+1];
						temp[2] = temp[2] + source[j*3+2];
						count = count + 1;
				}
		}

		// cout<<"cal means for "<<i<<" cluster "<<"the count is "<<count<<endl;
		 if(count!=0)
		{	
	    
		meansOfClusters[i*3] = temp[0]/count ;
		meansOfClusters[i*3+1] = temp[1]/count ;
		meansOfClusters[i*3+2] = temp[2]/count ;
		 }
		 else
			{
				 
				int index=rand()%sourceLength;  
				meansOfClusters[i*3]=source[3*index];
				meansOfClusters[i*3+1] =source[3*index+1];
				meansOfClusters[i*3+2] =source[3*index+2];
			}
	}
	//cout<<"after cal means"<<endl;
	//outputmeans();

}

void mykmeans::optimize()
{
	 
	int i=30;
	while(i>0)
	{
	 calClusterforVector();
     calmeans();    
			i--;
	}
}

void mykmeans::allroutine()
{
	initializeClusters();
	//cout<<"initialize the clusters :"<<endl;
	//outputmeans();
 
	optimize();
	//cout<<"after mykmeans, the means of clusters:"<<endl;
	//outputmeans();
}