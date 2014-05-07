// main.cpp : Defines the entry point for the console application.
//

#include <iostream>
//#include <vector>
#include "bitmap.h"
#include "mrf.h"
#include "GCoptimization.h"
#include "kmeans.h"
#include <math.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int numLabels = 2;
const MRF::CostVal Max=1000000;
unsigned char* inImage;
unsigned char* markupImage;
unsigned char* prob;
double* prob_double;


unsigned char* pOut;

int wIn = 0, hIn = 0;
int wMarkup = 0, hMarkup = 0;
int wProb=0, hProb=0,channel=0;
 

int lenBackground=0;
int lenForeground=0;
double* background =new double[22000];   ///    用 数组 简直太傻了
double* foreground=new double[22000]; //  
double* backgroundAfterCluster;
double* foregroundAfterCluster;



void calDis( int pix, double& disfore,double& disback)
{

	 disfore=Max;
	 disback=Max;
	double tempdis=0;
	unsigned char* tempix=inImage+pix*3;
	double redtemp=(double)tempix[0];
	double greentemp=(double)tempix[1];
	double bluetemp=(double)tempix[2];
	double a=0,b=0,c=0,b1=0,b2=0,b3=0,f1=0,f2=0,f3=0;
	 

	 
	for(int i=0;i<64;i++)
	{	 

		// blue cal the dis from background
		b1=backgroundAfterCluster[i*3];
		b2=backgroundAfterCluster[i*3+1];
		b3=backgroundAfterCluster[i*3+2];
	//	cout<<b1 <<" "<<redtemp<<" "<<b2<< " "<<greentemp<<"  "<<b3<<"  "<<bluetemp<<endl;
		a=b1-redtemp;
		b=b2-greentemp;
		c=b3-bluetemp;

		//if(a==0&&b==0&&c==0)
	 //	cout<<a<<"  "<<b<<"  "<<c<<endl;
		tempdis=sqrt(a*a+ b*b+ c*c);
			 
		//if(tempdis==0)
		// cout<<"dis from background is 0 !!!!!!!"<<endl;
		if(tempdis< disback)
			 disback=tempdis;
	}

	for(int i=0;i<64;i++)
	{
		//  cal the dis from foreground
		f1=foregroundAfterCluster[i*3];
		f2=foregroundAfterCluster[i*3+1];
		f3=foregroundAfterCluster[i*3+2];
		a=f1-redtemp;
		b=f2-greentemp;
		c=f3-bluetemp;

		//cout<<a<<"  "<<b<<"  "<<c<<endl;
		tempdis=sqrt(a*a+ b*b+ c*c);
		 
	//	if(tempdis==0)
		// cout<<"dis from foreground is 0 !!!!!!!"<<endl;
		if(tempdis< disfore)
			 disfore=tempdis;
	}

}

void getForeBackground()
{
	unsigned char* pIn=inImage;

	unsigned char* pMarkup=markupImage;
	lenBackground=0;
	lenForeground=0;

	for (int y = 0; y < hIn; y++) { 
		for (int x = 0; x < wIn; x++, pIn+=3, pMarkup+=3) {

			if (pMarkup[0]==255 && pMarkup[1]==0 && pMarkup[2]==0) {
				// blue, make the pix in Image into  background vectoers
				//cout<<"blue background"<<endl;
				background[ lenBackground*3]=pIn[0];
				background[ lenBackground*3+1]=pIn[1];
				background[ lenBackground*3+2]=pIn[2];
				lenBackground++;

			}
			else if(pMarkup[0]==0 && pMarkup[1]==0 && pMarkup[2]==255)
			{// red  fore ground
				//cout<<"red  foreground, [2] is 255"<<endl;
				foreground[ lenForeground*3]=pIn[0];
				foreground[ lenForeground*3+1]=pIn[1];
				foreground[ lenForeground*3+2]=pIn[2];
			//	cout<<"foreground length"<<lenForeground<<endl;
				lenForeground++;
			}
		}
	}


}


MRF::CostVal dCost(int pix, int i)
{

	//unsigned char*  pIn= inImage+pix*3;

	unsigned char* pMarkup=markupImage+pix*3;

	if (pMarkup[0]==0 && pMarkup[1]==0 && pMarkup[2]==255) 
	{ // in opencv  this is red. red foreground 
		if(i==0)  // opencv    r, fore ground 
			return  (MRF::CostVal)100; // (MRF::CostVal)
		else
		{
			// cout<<"markup Blue background, cost is "<<(MRF::CostVal) 0<<endl;
			return (MRF::CostVal) 0;
		}
	}
	else if(pMarkup[0]==255 && pMarkup[1]==0 && pMarkup[2]==0)
	{
		//blue  background  in opencv 
		if(i==0)
			return (MRF::CostVal) 0;
		else
		{
			 
			return (MRF::CostVal)100;
		}
	}
	else
	{
		double disfore;
		double disback;
		calDis(pix,disfore,disback);  ///参数是  引用 
		// cout<<"uncertain disFore is "<<disfore<<"  disback is "<<disback<<endl;
		MRF::CostVal dataCost=0;

	 	 

		if(i==1)
		{

			dataCost=(MRF::CostVal)(disfore/(disfore+disback+0.0001)); //
			//cout<<"uncertain  is labeled as 1 foreground , dataCost is  "<< dataCost<<endl;
		 
		}
		else
		{
			dataCost=(MRF::CostVal)(disback/(disfore+disback+0.0001)); //+0.0001
			 //	cout<<"uncertain  is labeled as 0 background,cost is   "<<dataCost<<endl;
			
		}

		return dataCost;

	}


}


MRF::CostVal fnCost(int pix1, int pix2, int i, int j)
{
	if (pix2 < pix1) { // ensure that fnCost(pix1, pix2, i, j) == fnCost(pix2, pix1, j, i)
		int tmp;
		tmp = pix1; pix1 = pix2; pix2 = tmp;
		tmp = i; i = j; j = tmp;
	}
	int d = pix2-pix1;
	if(i!=j &&(d==1||d==wIn))
	{
		unsigned char* pix1Pointer=inImage+pix1*3;
		unsigned char* pix2Pointer=inImage+pix2*3;
		double* prob1p=prob_double+pix1;
		double* prob2p=prob_double+pix2;
		double cordis=(pix1Pointer[0]-pix2Pointer[0])*(pix1Pointer[0]-pix2Pointer[0])+(pix1Pointer[1]-pix2Pointer[1])*(pix1Pointer[1]-pix2Pointer[1])+(pix1Pointer[2]-pix2Pointer[2])*(pix1Pointer[2]-pix2Pointer[2]);
		
		//cordis=sqrt(cordis);

		///if(cordis>300)
	 //cout<<cordis<< "	";
		

 /* if(prob1p[0]!=0)
		cout<<"p1= " <<prob1p[0]<<endl;
		if(prob2p[0]!=0)
		cout<<"p2= " <<prob2p[0]<<endl;
	*/	 
 
		return  (MRF::CostVal)(1500/((cordis+1)*(1-prob1p[0]+0.0000001)*(1-prob2p[0]+0.0000001)));  //  *(1-prob1p[0]+0.000001)*(1-prob2p[0]+0.000001)


	}
	else 
		return (MRF::CostVal)0;

	 
}


EnergyFunction* generate_DataFUNCTION_SmoothGENERAL_FUNCTION()
{
	DataCost *data         = new DataCost(dCost);
	SmoothnessCost *smooth = new SmoothnessCost(fnCost);
	EnergyFunction *energy = new EnergyFunction(data,smooth);

	return energy;
}

int main(int argc, char* argv[])
{
	MRF* mrf;
	EnergyFunction *energy;
	float t,tot_t;
	int iter;
	MRF::EnergyVal E;
	unsigned char* pIn;
	unsigned char* pMarkup;
	unsigned char* pProb;

	/////////////////////////////////////////////////// assign these variables:::   pIn=inImage, pMarkup=markupImage, prob, prob_doubole 
	/*inImage = readBMP(argv[1], wIn, hIn);

	if(inImage==NULL)
		cout<<"can not read  input image "<<endl;
	pIn=inImage;

	markupImage = readBMP(argv[2], wMarkup, hMarkup);
	pMarkup=markupImage;
 
	if(argc==5)
	{
		cout<<argv[4]<<endl;
		prob=readBMP(argv[4], wProb, hProb);  ////// image  must be in 3-Dimension 
		 
 
		prob_double=new double[wIn*hIn];
		// transfer unsigned char into double //   *prob_double
		 cout<<"prob size: "<<wProb<<"   "<<hProb<<endl;
		int i=0;
		for(;i<wProb*hProb;i++)
			{
				 
		prob_double[i]=prob[i*3];
	 
		 prob_double[i]=prob_double[i]/255;
		 
		}
	
	}
	else if(argc==4)
	{  // prob =0; 
	////prob=new unsigned char[];
	//// 
	////cout<<"prob[0]: "<<double(prob[0])<<endl;  //13    186
	////cout<<"prob[3]: "<<double(prob[3])<<endl;
		//double[] temp = new double[10];
	prob_double=new double[wIn*hIn];  // initialize 00000
	for(int j=0;j<wIn*hIn;j++)
	 
		prob_double[j]=0;
 
	cout<<"parameter is 4"<<endl;
 
	}
	*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	char *imagename,  *markup,  *result,  *prob_name;
	char *names[] = { "bird","owl","boy","image_House256rgb","cessna","redberry", "fullicewater", "leo"};

	 
	
	if(argc<=1)
	{
		imagename=new char[20];
		markup=new char[25];
		result=new char[25];
		
		int  i=0;

		imagename=strcpy(imagename,names[i]);
		markup =strcpy(markup,names[i]);
		result=strcpy(result,names[i]);

		imagename=strcat(imagename,"30.jpg");
		markup=strcat(markup,"_markup.bmp");
		result=strcat(result,"30_res.bmp");
	
	    //  prob_name=new char[25];
		//  prob_name=strcpy(prob_name,names[i]);
		// prob_name=strcat(prob_name,"20prob3.bmp"); // cessna10prob2_noprior
   prob_name="  ";  
		 cout<<imagename<<"  "<<markup<<"  "<<result<<"  "<<prob_name<< endl;
	//imagename ="redberry10.jpg"; // "dog.bmp";
	//markup = "redberry_markup.bmp";
	//result = "redberry10_res.bmp";
	//prob_name="owl210prob_new2.bmp";
	  //prob_name="cessna20binprob3.bmp";  // if do not assign value for prob_name, imread return a Mat with not null data!!!!
	  
	}
	else if(argc==4)
	{
	 imagename=argv[1];
	 markup = argv[2];
	 result  = argv[3];
	 prob_name="  ";
	}
	else if(argc==5)
	{
	 imagename=argv[1];
	 markup = argv[2];
	 result  = argv[3];
	 prob_name=argv[4];
	}


	Mat img = imread(imagename);
	
	
	Mat markMap = imread(markup);
	Mat probMat=imread(prob_name,0);

	inImage = img.data;
	markupImage=markMap.data;
	wIn = img.cols;
	hIn = img.rows;



	  if(! probMat.data )                                                      // Check for invalid input
    {
        cout <<  "Could not open or find the probability!" <<endl ;

		 //probability  is 0000

		prob_double=new double[wIn*hIn];  // initialize 00000
	
		for(int j=0;j<wIn*hIn;j++)
		prob_double[j]=0;
    }
	 else
	 {
		cout<<"the probability has been read in. "<<endl;
	 	prob=probMat.data;

		wProb = probMat.cols;
	    hProb = probMat.rows;

		cout<<"prob array cols & rows: "<<wProb<<"   "<<hProb<<endl;
		 
		channel=probMat.channels();
		cout<<wProb<<"  "<<hProb<<" "<<"   "<<channel<<endl;
		prob_double=new double[wProb*hProb];
		int i=0;
		for(;i<wProb*hProb;i++)
			{
				 
		prob_double[i]=double(prob[i]);
	 
		 prob_double[i]=prob_double[i]/255;

		/* if( prob_double[i]!=0)
		 cout<<prob_double[i]<<endl;
		 */
		}
		
	//	cout<<i<<endl; 
	 
	  
	 }

	////////////////////////////////////////////

	  	if(!inImage) {
			cout << " Error loading the input image" << endl;
			double a=0;
		cin>>a;
			return 0;
		}
		else if(!markupImage)
		{
		cout << " Error loading the markup image" << endl;
			double a=0;
		cin>>a;
			return 0;
		}

		else{
	getForeBackground();  // get the foreground and background 


	cout<<"background length is : "<< lenBackground<<endl;  //// input image is jpg format.... 
	cout<<"foreground length is : "<< lenForeground<<endl;
	mykmeans* mykmeans1 = new mykmeans(64, lenBackground,background);  
	mykmeans1->allroutine();

	backgroundAfterCluster=mykmeans1->meansOfClusters;
	 
	mykmeans* mykmeans2= new mykmeans(64, lenForeground,foreground);  
	mykmeans2->allroutine();
	foregroundAfterCluster=mykmeans2->meansOfClusters;
	 
	 
	
		energy = generate_DataFUNCTION_SmoothGENERAL_FUNCTION();
		cout<<"after get the energy function "<<endl;
		 
		mrf = new Expansion(wIn, hIn,numLabels,energy);  
		mrf->initialize(); 
		mrf->clearAnswer();
		E = mrf->totalEnergy();
		cout<<"after cal the totalEnergy"<<endl;
		printf("Energy at the Start= %g  \n", (float)E);
		 
	 mrf->optimize(3, t);

		cout<<"after optimize "<<endl;

		MRF::EnergyVal E_smooth = mrf->smoothnessEnergy();
		MRF::EnergyVal E_data   = mrf->dataEnergy();
	 	 printf("Total Energy = %.1f (Smoothness energy %.1f, Data Energy %.1f)\n", E_smooth+E_data,E_smooth,E_data);
		 
 
		Mat out;//=Mat(sizeY,sizeX,CV_8UC3);
	img.copyTo(out);
 
		for (int pix =0; pix < wIn*hIn; pix++)
		{ 
			MRF::Label pixLabel=mrf->getLabel(pix);
		


			if(pixLabel==0) {
				// label is background 
				//printf("Label of pixel %d is %d ",pix,pixLabel);
				 
				out.data[3*pix]=255;
				out.data[3*pix+1]=0;
				out.data[3*pix+2]=0;
			} 

		}

		// save the output image
		//cout<<"output_file_name:  "<<argv[3]<<endl;
	//	writeBMP(result, wIn, hIn, outImage);

		imwrite(result, out);
		imshow("image",out);
	waitKey();

		 
		delete [] backgroundAfterCluster;
 
		delete [] foregroundAfterCluster;
	 
		delete [] prob_double;
 

		double a=0;
		cin>>a;
	}
	/*else {
		cout << " Usage: compute.exe image.bmp markup.bmp out.bmp" << endl;
	}*/
}

