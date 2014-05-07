class mykmeans
{
public:
	 
    // fully specifies the energy
    mykmeans(int k,int sourceLenght ,double* source);
 

   ~mykmeans() { }
    
     
    
	void  allroutine();
	void outputmeans();
  double* meansOfClusters;


 
private:
    int  k;  
	double* source;
	int sourceLength;
	
	int* clusterBitmap;
	void dontCacheSmoothnessCosts() {  }
 
    void initializeClusters();
	void calmeans();
     
    void calClusterforVector() ;
	void  optimize();
};