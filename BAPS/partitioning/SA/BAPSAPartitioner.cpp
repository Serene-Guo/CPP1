#include "BAPSAPartitioner.h" 

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::setprecision;


BAPSAPartitioner::BAPSAPartitioner(BAPPackage& aPackage)
        :  BAPPartitioner(aPackage),           // init base class
        mNumVes(aPackage.NumVessels()),
        mVes(array<SAVessel>(1, mNumVes)),
        mNumSect(aPackage.NumSections()),
        mSect(array<SASection>(1, mNumSect)),
        mTrans(aPackage.Transhipments()),
        mDist(aPackage.Distances()),
        mTraceFile(mPackage.PartitioningTraceFilename()),
        mRuntimeAnalyzer(0),
        mSolnExists(false)
{
        gettimeofday(&mStartTime, NULL);
        array<int>  temp;

        // Init vessel array with vessel objects
        temp = aPackage.VesselLengths();

        for (int i = 1; i <= mNumVes; i++)
        {
                SAVessel v(i, temp[i]);
                v.StartTimeZone(aPackage.StartTimeZone(i));
                v.EndTimeZone(aPackage.EndTimeZone(i));
                v.Arrival(aPackage.Arrival(i));
                v.Departure(aPackage.Departure(i));

                // find all neighbouring vessels
                for (int j = 1; j <= mNumVes; j++)
                {
                        if (j == i) continue;         // ignore self loop

                        if (mTrans(i, j) > 0  ||  mTrans(j, i) > 0)
                        {
                                v.AddNeighbour(j);
                                v.AddTranshipment(mTrans(i, j));
                                v.AddTranshipment(mTrans(j, i));
                        }
                }

                v.Export(mTrans(0, i));
                v.Import(mTrans(i, 0));
                mVes[i] = v;
        }

        // Init section array with section objects
        temp = aPackage.SectionLengths();

        for (int i = 1, t = aPackage.NumTimeZones(); i <= mNumSect; i++)
                mSect[i] = SASection(i, t, temp[i]);

        ReadParameterFile();
}


BAPSAPartitioner::~BAPSAPartitioner()
{
        // nothing to do
}


void BAPSAPartitioner::Print(const int& aWidth,
        const int& aDetail) const
{
        cout  << "--== BAPSAPartitioner ==--" << endl
                << "  ID = " << ID() << endl
                << "  Name = " << Name() << endl
                << "  Number of sections = " << mNumSect
                << ", Number of vessels = " << mNumVes << endl
                << "  Random seed = " << mRandomSeed << endl
                << "  Transhipment = " << mTranshipment
                << ", Penalty = " << mPenalty << endl << endl;

        for (int i = 1; i <= mNumVes; i++)
                if (aDetail > 0)
                        mVes[i].Print(aWidth, aDetail);
                else
                        cout  << "ves " << setw(4) << i
                        << " in sect " << setw(2) << mVes[i].Section() << endl;

        for (int k = 1; k <= mNumSect; k++)
                if (aDetail > 0)
                        mSect[k].Print(aWidth, aDetail);
                else
                {
                        cout  << "sect " << setw(2) << k << " with vessels: " << endl;
                        int   i;
                        const set<int>& V = mSect[k].Vessels();
                        forall(i, V)
                        {
                                cout << i << " ";
                        }
                        cout << endl;
                }
}


// The Solve method is called to perform
// the vessel partitioning algorithm.
//   this version calls GenerateInitialSolution
//    (which creates a random initial solution)
//
void BAPSAPartitioner::Solve()
{

        GenerateInitialSolution();
        CalcInitialObjVal();

        gettimeofday(&mEndTime, NULL);

        PrintSummary();
        WriteSolutionFile();
        UpdatePackage();
}


void BAPSAPartitioner::PrintSummary() const
{
        cout  << mPackage.ProjectFilename() << " --- "
                << mNumVes << " vessels, " << mNumSect << " sections" << endl
                << "_NUM_UNALLOCATED = " << mUnallocVes.size() << endl
                << "Transhipment = " << mTranshipment << endl
                << "Penalty      = " << mPenalty << endl
                << "Obj Value    = " << CalcObjVal() << endl
                << "Time taken (sec) = "
                << (mEndTime.tv_sec - mStartTime.tv_sec) +
                (mEndTime.tv_usec - mStartTime.tv_usec) / 1000000.0
                << endl
                << endl;
}



void BAPSAPartitioner::InitSolution()
{
        // Init working variables
        mTranshipment = 0;
        mPenalty = 0;
        mUnallocVes.clear();   // set<int>
        for (int i = 1; i <= mNumVes; i++)
        {
                mVes[i].Section(UNASSIGNED);   // all vessels unassigned
                mUnallocVes.insert(i);
        }

        ResetVesselDestinations(); 
}

void BAPSAPartitioner::ResetVesselDestinations()
{// add all the section into the mPotentialDestinations 
        // Add potential destination sections to each vessel
        for (int i = 1; i <= mNumVes; i++)
                for (int k = 1; k <= mNumSect; k++)
                        if (k != mVes[i].Section()) // return the section #
                                mVes[i].AddDestination(k);  //mPotentialDestinations is set<int>
}

//********************************************************************************************************** liu qing 
//****
// This method actually create the vessel partitioning solution.
//  using Simulated Annealing algo
void BAPSAPartitioner::GenSolnSA()
{
        unsigned long costx,costxx, bestcost;
        int  deltcost;
        vector<unsigned long> x,bestsol; //length equal to num of vessels+1
        double T , prob;

        GenSolnGreedy();
        //GenSolnRandom();

        CalcInitialObjVal();  // ���� transshipment  and penalty 
        costx = CalcObjVal(); // get value 
        bestcost = costx;
        x=ConvSoltoVec(); /////////add  �ں��������Ѿ��� x[0]��ֵ���ˡ�������������ֻ����һ������ �������
        bestsol = x;//
        T=8000;
     /*   int minT=15;
        int maxT=150;
        T=mNumVes>maxT?maxT:mNumVes;
        T=T<minT?minT:T;*/

      //  cout<<"initial  "<<T<<endl;

        srand((unsigned)time(0));//randseeds based on time

        //cout<<"initial solution:"<<endl;
        //printSolu(x);
        //cout<<"initial obj value:"<<mTranshipment+mPenalty <<endl;
        int temperature=0;
        while(T>80)
        {
                //temperature++;
                //cout<<"temperature "<<temperature<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;
                int times=50;
                while(times--)
                {

                        //cout<<10-times<<"th  neighbor:"<<endl;
                       // vector<unsigned long> xx = SelectANeighorRanGeedy(x); // xx means x'
			//cout<<"before select a neighor"<<endl;
		        //  vector<unsigned long> xx = SelectANeighorOneVes(x);
                        // vector<unsigned long> xx = SelectANeighbor_ChangeOneVessel(x);
                         vector<unsigned long> xx = SelectANeighbor_RandomTentoBestOne(x);
                        //��ʱ��xx�� ��һ��Ԫ�ص�ֵ �Ѿ���  cost�� 
                        //printSolu(xx); // ����print�ˣ���ѡneighbor�ĺ���������print����������
			//cout<<"after select a neighbor;"<<endl;
                        costxx = xx[0];
                        deltcost = costxx - costx;
                        //cout<<"deltCost "<<deltcost<<endl;
                        prob= (rand() % 10)*1.0/10;//rand a num (0~1)
                        if( deltcost<0 || prob<=exp(-deltcost/T) )//
                        {// ���� xx���� 
                                //cout<<"accept neighour:"<<endl;
                                ConvVectoSol(xx);//���� ����//x=x';ÿ��ѭ�����涼��convert��Ч���Ƿ�̫���ˣ�
                                x=xx;  //    ��������䣬�´���neighbour û�취�ж��ܲ��ܽ���
                                costx = costxx;
                                if( costx < bestcost)
                                {
                                        //cout<<"change best solution>>>>>>>>"<<endl;
                                        bestcost = costx;
                                        bestsol = xx; //  
                                }
                        }
                }
                T=T*0.9;
        }

        ConvVectoSol(bestsol);//��bestsoln ������ǰsolution
}

vector<unsigned long> BAPSAPartitioner::ConvSoltoVec()
{       //this function is to convert current solution to a vetcor(lenght = #of vessels)

        vector<unsigned long> vv(mNumVes+1);//��1��ʼ��,�ó�ʼ�� һ�� ��С�����ܸ�ֵ������͵�  push_back

        for (int i = 1; i <= mNumVes; i++)
        {
                vv[i] = mVes[i].Section(); //.Section��int
        }
		vv[0]= mTranshipment + mPenalty;
        return vv;
}
void BAPSAPartitioner::ConvVectoSol(vector<unsigned long> xx)
{       //this function is to convert a vetcor(lenght = #of vessels) to current solution 
        // ȫ�� ���
        clearSoln();
        for(int i = 1; i <= mNumVes; i++)
        {
                if(xx[i]!=-1)
                        Assign(mVes[i],mSect[xx[i]]);   
        }
        //////// ����������ģ���һ������Ǹ�ֵ����Ӧ�� �� resetһ�£�Ȼ���� �� Assign(vessel,sec) ������� ����ֵ��������
}

unsigned long BAPSAPartitioner::CalcObjValByDelt(int vesi,int vesj,vector<unsigned long> curSolution)
{// return object value��  curSolution ��0��ֵ���Ѿ�������  objectvalue
	int seci=curSolution[vesi];
	int secj=curSolution[vesj];

	assert(vesi!=vesj);
	assert(seci!=secj);
	long deltCost=0;
	unsigned long obj=curSolution[0];
	//cout<<"cur obj"<<obj<<endl;

	int temp_ves,temp_sec;
	if(secj==-1)
	{
	//����һ�� seci �� secj��ֵ �Լ�  vesi �� vesj��ֵ��������֤  seci=-1, secj��Ϊ-1; �����ô��� ���÷ֱ����� seci==-1��secj==-1����
		temp_ves=vesi;
		vesi=vesj;
		vesj=temp_ves;
		temp_sec=seci;
		seci=secj;
		secj=temp_sec;
	}

	if(seci==-1)
	{//����������� ����֮����һ�� section�� unassigned 
	// first consider the import and export 
		deltCost-=(mVes[vesi].Import() + mVes[vesi].Export()-mVes[vesj].Import()-mVes[vesj].Export()) * LONGDISTANCE; // vesi, vesj   penalty  �仯
		deltCost+=(mVes[vesi].Import() + mVes[vesi].Export()-mVes[vesj].Import()-mVes[vesj].Export()) *D(secj,0); //vesi ������ transhipment 
	//consider neibbour of vesi and vesj
		 int v2;
		for(int i=0;i<2;i++)
		{ // ����ѭ���� Ҫ������ֻ�� i  j ,�ڵ�һ��ѭ�� ֮�󣬽������ߵ�ֵ
			  const set<int>&   neighbours = mVes[vesi].Neighbours();       
                // Calculate inter-vessel transhipment costs
                forall(v2, neighbours)
                {
					if(v2>0&&v2!=vesj&&curSolution[v2]!=UNASSIGNED)
					{// V2 >0 ��Ϊ�� ������neighbor vessel 0(֮ǰ�Ѿ����ǹ���)��
						//v2=vesj, ��i��j֮���flow��Ӱ��cost�� v2����������� unassigned costҲ����
						deltCost-=TotalFlow(v2,vesi)*LONGDISTANCE; // vesi neighour  penalty ��С
						if(curSolution[v2]!=secj) //vesi��Ŀ��section sej ��nei��section��ͬ��ʱ�� Ҫ��һ�� transhipment 
							deltCost+=TotalFlow(v2,vesi)*D(curSolution[v2],secj);
					
					}
				}
//���� vesi  vesj,,seci  secj
		temp_ves=vesi;
		vesi=vesj;
		vesj=temp_ves;
		temp_sec=seci;
		seci=secj;
		secj=temp_sec;
		
		}

	}
	else
	{//���� ��������-1
	
		//����seci�� tranship �ı仯 
		deltCost-=(mVes[vesi].Import() + mVes[vesi].Export()-mVes[vesj].Import()-mVes[vesj].Export()) * D(seci,0); 
		//secj ��tranship�ı仯
		deltCost+=(mVes[vesi].Import() + mVes[vesi].Export()-mVes[vesj].Import()-mVes[vesj].Export()) *D(secj,0);  

		 int v2;
		for(int i=0;i<2;i++)
		{ // ����ѭ���� Ҫ������ֻ�� i  j ,�ڵ�һ��ѭ�� ֮�󣬽������ߵ�ֵ
			  const set<int>&   neighbours = mVes[vesi].Neighbours();       
                // Calculate inter-vessel transhipment costs
                forall(v2, neighbours)
                {
					if(v2>0&&v2!=vesj&&curSolution[v2]!=UNASSIGNED)
					{// V2 >0 ��Ϊ�� ������neighbor vessel 0(֮ǰ�Ѿ����ǹ���)��
						//v2=vesj, ��i��j֮���flow��Ӱ��cost�� v2����������� unassigned costҲ����

						if(curSolution[v2]==seci)
						 // tranship ����, ��������vessel��һ��section  ��� ȥ����section
							deltCost+=TotalFlow(v2,vesi)*D(seci,secj);
						 
						else if(curSolution[v2]==secj)
							deltCost-=TotalFlow(v2,vesi)*D(seci,secj);
						else
							deltCost+=TotalFlow(v2,vesi)*(D(curSolution[v2],secj)-D(curSolution[v2],seci));
						//deltCost-=TotalFlow(v2,vesi)*LONGDISTANCE; // vesi neighour  penalty ��С
						//if(curSolution[v2]!=secj) //vesi��Ŀ��section sej ��nei��section��ͬ��ʱ�� Ҫ��һ�� transhipment 
							//deltCost+=TotalFlow(v2,vesi)*D(curSolution[v2],secj);
					
					}
				}
//���� vesi  vesj,,seci  secj
		temp_ves=vesi;
		vesi=vesj;
		vesj=temp_ves;
		temp_sec=seci;
		seci=secj;
		secj=temp_sec;
		
		}

	}
	obj+=deltCost;
	 
	//cout<<"after change: obj"<<obj<<endl;
	return obj;
}
void BAPSAPartitioner::CalcObjValTemp(vector<unsigned long>& xx )
{

        unsigned long aTrans = 0;
        unsigned long aPenalty = 0;
        unsigned long ObjVal;


        for (int v1 = 1; v1 <= mNumVes; v1++)
        {
                const int&        s1 = xx[v1];//gai
                const set<int>&   neighbours = mVes[v1].Neighbours();
                int               v2;
                // Calculate inter-vessel transhipment costs
                forall(v2, neighbours)
                {
                        if (v2 < v1) continue;
                        const int&     s2 = xx[v2];//gai

                        if (s1 == UNASSIGNED  ||  s2 == UNASSIGNED)
                                aPenalty += TotalFlow(v1, v2) * LONGDISTANCE;
                        else
                        {
                                aTrans += TotalFlow(v1, v2) * D(s1, s2);
                        }
                }

                // Calculate import,export costs
                if (s1 == UNASSIGNED)
                        aPenalty += (mVes[v1].Import() + mVes[v1].Export()) * LONGDISTANCE;
                else
                {
                        aTrans += (mVes[v1].Import() + mVes[v1].Export()) * D(s1, 0);
                }
        }

        ObjVal = aTrans + aPenalty; 
        assert(ObjVal >= aTrans);
        assert(ObjVal >= aPenalty);
		xx[0]=ObjVal;
        //return ObjVal;
}

//**********************************************************************************************************
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>guo fang
void BAPSAPartitioner::printSolu(vector<unsigned long>& curSolution)//
{
        for(int i=0;i<curSolution.size();i++)
        {
                cout<<(int)curSolution[i]<<" ";
        }
        cout<<endl;
}
vector<unsigned long> BAPSAPartitioner::SelectANeighbor_RandomTentoBestOne(vector<unsigned long>& curSolution)
{
        int i,j;
        unsigned long temp;
        int times=10;
        vector<unsigned long> best = curSolution;
        vector<unsigned long> nei = curSolution;
        CalcObjValTemp(best);
        nei = curSolution;
        times=1000;
        while(times--){
                i = mRandom(1, mNumVes);
                j = mRandom(1, mNumVes);
                if( curSolution[i]!=curSolution[j] && CanExchange(i,j) ){
                        best[i] = nei[j];
                        best[j] = nei[i];
                        CalcObjValTemp(best);
                        break;
                }
        }
        times=10;
        while(times)
        {
                nei = curSolution;
                i = mRandom(1, mNumVes);
                j = mRandom(1, mNumVes);
                if( curSolution[i]!=curSolution[j] && CanExchange(i,j) ){
                        temp=nei[j];
                        nei[j]=nei[i];
                        nei[i]=temp;
                        CalcObjValTemp(nei);
                        if(nei[0]<best[0])
                                best = nei;
                        times--;
                }
        }
        return best;//
}
vector<unsigned long> BAPSAPartitioner::SelectANeighbor_ChangeOneVessel(vector<unsigned long>& curSolution)
{
        int i,j;
        vector<unsigned long> vv = curSolution;
        set<int>  tempUna=mUnallocVes; 
        int times=10000;
        while(times--)
        {
                i = mRandom(1, mNumVes);
                j = mRandom(1, mNumSect);
                if (mSect[j].CanAccommodate(mVes[i]))
                {
                        //cout<<"����,�ҵ�һֻ��"<<i<<"ԭ��section��"<<vv[i]<<"���Ի���section��"<<j<<endl;
                        mSect[vv[i]].Remove(mVes[i]);
                        for(int k=1;k<=mNumVes;k++){
                                if( vv[k] == -1 && mSect[vv[i]].CanAccommodate(mVes[k])){ //k is unassigned
                                        vv[k]=vv[i]; 
                                        break;
                                }
                        }
                        mSect[vv[i]].Add(mVes[i]); 
                        //����������κ��м������ݻ�������
                        vv[i]=j;
                        break;
                }
        }
         CalcObjValTemp(vv);
        return vv;
}
vector<unsigned long> BAPSAPartitioner::SelectANeighborRandomly(vector<unsigned long>& curSolution)//������vector��x
{///��� ������vessel ���ܲ��ܽ�����
        int i,j;
      //  unsigned long obj;
        vector<unsigned long> vv = curSolution;
        int times=10000;
        while(times--)
        {
                i = mRandom(1, mNumVes);
                j = mRandom(1, mNumVes);

                if( curSolution[i]!=curSolution[j] && CanExchange(i,j) ){
                        //cout<<"change i,j:"<<i<<" "<<j<<endl;
                        long temp = vv[i];
                        vv[i] = vv[j];
                        vv[j] = temp;
                        break;
                }
        }
     //   obj = CalcObjValTemp(vv);
		 unsigned long obj2=CalcObjValByDelt(i,j,curSolution);
		// cout<<"obj="<<obj<<" and obj2="<<obj2<<endl;
        vv[0] = obj2;
        //cout<<"obj value"<<obj<<endl;
        return vv;
}
vector<unsigned long> BAPSAPartitioner::SelectANeighorRanGeedy(vector<unsigned long>& curSolution)
{
        //find a neighour from current solution 
        vector<vector<unsigned long> > nei=GenNeighborhood(curSolution); 
        vector<vector<unsigned long> > neiSub=RandomGenSubset(nei);
        vector<unsigned long> aNeighbor=selectBestFromNei(neiSub,curSolution);
        return aNeighbor;
}

vector<unsigned long> BAPSAPartitioner::SelectANeighorRandomTotally(vector<unsigned long>& curSolution)
{
        vector<vector<unsigned long> > nei=GenNeighborhood(curSolution); 
        vector<unsigned long> aNeighbor=SelectRanFromNeighborhood(nei,curSolution);
        return aNeighbor;
}

vector<unsigned long> BAPSAPartitioner::SelectANeighorOneVes(vector<unsigned long>& curSolution)
{    
        vector<unsigned long> aNeighbor=curSolution;
        int i,j=1;
        int preSec;
        int vess_move;
        bool findAsect=false;
        set<int>  tempVessels=mUnallocVes; 
        int temp;
        while(!tempVessels.empty()){
                temp=tempVessels.choose(); 
                for(j=1;j<=mNumSect;j++){ 
                        if(mSect[j].CanAccommodate(mVes[temp])){ 
                                aNeighbor[temp]=j;
                                findAsect=true;
                                cout<<"һ��ʼ���ҵ��������unasigned�Ĵ���"<<endl;
                                break;
                        }
                }
                if(findAsect)
                        break; 
                tempVessels.del(temp);
        }

        if(findAsect)
        {
                //cout<<"find a section for the unassgined vessel:"<<endl;
                CalcObjValTemp(aNeighbor);
                return aNeighbor;
        }
        ///���û���ҵ� ���������
        findAsect=false; // ����
        for(i=1;i<=mNumVes;i++)
        {
                if(curSolution[i]==-1)
                {
                        continue;
                }
                for(j=1;j<=mNumSect;j++)
                {
                        if(j!=curSolution[i]&&mSect[j].CanAccommodate(mVes[i]))
                        { 
                                vess_move=i;
                                preSec=curSolution[i];
                                //cout<<"preSec"<<preSec<<endl;
                                aNeighbor[i]=j;
                                findAsect=true;
                                cout<<"��Ȼһ��ʼ���ҵ��������unasigned�Ĵ��������ҵ�һֻ�����滻�Ĵ�"<<endl;
                                break;
                        }
                }
                if(findAsect)
                        break;
        }
        //�ƶ�һ��vessel֮��
        tempVessels=mUnallocVes; //  ���� ��һ��ֵ 

        while(!tempVessels.empty()) {
                temp=tempVessels.choose(); //ѡ��һ�������� ���section �ܲ��ܰ� �����£�������
                mSect[preSec].Remove(mVes[vess_move]); //��ʱ remove  a vessel
                if(mSect[preSec].CanAccommodate(mVes[temp]))
                {
                        aNeighbor[temp]=preSec;
                        mSect[preSec].Add(mVes[vess_move]);
                        cout<<"�����滻һֻ������������һֻunassigned�Ĵ��ˣ�"<<endl;
                        break;
                }
                mSect[preSec].Add(mVes[vess_move]);  // ������ӣ��´�ѭ�� ��remove��ʱ�򣬳���� assert ʧ�ܣ� stop
                tempVessels.del(temp);
        }
        CalcObjValTemp(aNeighbor);
        return aNeighbor;
}

vector<unsigned long> BAPSAPartitioner::SelectRanFromNeighborhood(vector<vector<unsigned long> >& nei,vector<unsigned long>& curSolution)
{// �� neighborhood�� ��� ѡ��������һ������������ neighbor
        int j = mRandom(1, nei.size());   //using leda::random_source;   // Using LEDA's random source type
        vector<unsigned long> temp(mNumVes+1); 


        /////�� nei[j]   ת��Ϊ  vector_solu��return
        int vess1=nei[j][0];
        int vess2=nei[j][1];
        temp=curSolution;
        int sec1=curSolution[vess1];   //��unsigned long ��ֵ���� һ��int�� ���ܻ���-1��û�� ������ �ְ�-1 ��ֵ���� unsigned long
        int sec2=curSolution[vess2];
        //cout<<"exchange vessels: "<<vess1<<" and "<<vess2<<endl;
        temp[vess1]=sec2;
        temp[vess2]=sec1; 
        printSolu(temp);
       // unsigned long obj=CalcObjValTemp(temp);
		 unsigned long obj2=CalcObjValByDelt(vess1,vess2,curSolution);
		// cout<<"obj="<<obj<<" and obj2="<<obj2<<endl;
        temp[0]=obj2;
        //cout<<"after exchange, obj value is : "<<obj<<endl;
        return temp;

}


vector<vector<unsigned long> > BAPSAPartitioner::GenNeighborhood(vector<unsigned long> curSolution)
{
        // given a solution, find all its neighbors, each neighbor just store the two vessel num which will exchange
        // solution length = mNumVes+1
        vector<vector<unsigned long> > nei;
        vector<unsigned long> aNei(2);
        int i,j;
        for(i=1;i<=mNumVes;i++)
                for(j=i+1;j<=mNumVes;j++)
                        if(curSolution[i]!=curSolution[j]&&CanExchange(i,j))
                        {aNei[0]=i;
        aNei[1]=j;
        //nei.insert(aNei);//  insert  ��  push_back �� ����
        nei.push_back(aNei);
        }

        return nei;


}

bool BAPSAPartitioner::CanExchange(int vesi,int vesj)
{//////ֻ�ǿ� �ܲ��� ����ʵ���� ��û�л�����������
        int si = mVes[vesi].Section();
        int sj=mVes[vesj].Section();
        // ����һ��  ����Ϊ -1��   ���� �϶�����ͬ���������� ���ǲ�ͬ�ġ�
        SAVessel&   vi = mVes[vesi];
        SAVessel&   vj = mVes[vesj];

        if(si==-1)
        {
                //��sj �а� vesj remove, test  sj.CanAllocate(vessel i)
                mSect[sj].Remove(vj);  // �����޸� v�� section value ��Ϊ�����������ĸ�ֵ��ֻ�� test �ɲ����Խ�����������
                if(mSect[sj].CanAccommodate(vi))
                { 
                        mSect[sj].Add(vj);  //�ָ� ԭ���� solution X
                        return true; }
                else
                {
                        mSect[sj].Add(vj);
                        return false;}
        }
        else if(sj==-1)
        {
                mSect[si].Remove(vi);
                if(mSect[si].CanAccommodate(vj))
                { 
                        mSect[si].Add(vi);  //�ָ� ԭ���� solution X
                        return true; }
                else
                {
                        mSect[si].Add(vi);
                        return false;}
        }
        else
        {
                mSect[sj].Remove(vj);
                mSect[si].Remove(vi);
                if(mSect[si].CanAccommodate(vj)&&mSect[sj].CanAccommodate(vi))
                {
                        mSect[sj].Add(vj);
                        mSect[si].Add(vi);
                        return true;
                }
                else
                {

                        mSect[sj].Add(vj);
                        mSect[si].Add(vi);
                        return false;
                }


        }
        //Add(const SAVessel& aVessel);
        //   Remove(const SAVessel& aVessel);
}
vector<vector<unsigned long> > BAPSAPartitioner::RandomGenSubset(vector<vector<unsigned long> >& nei){

        int neiSize=nei.size();

        if (neiSize<=10)
        {
                //ֱ�ӽ� neighbor����
                //cout<<"neighoor num is "<<neiSize<<endl;
                return nei;
        }
        else{

                int i,j,num=10;  // �� 10��Ԫ�ص� �Ӽ�������




                // ��ֹ ����random �ظ�������һ�� index����

                int index[neiSize]; //��֪�� �����ɲ����� �������ԵĻ� ���� array
                for (int i = 0; i < neiSize; i++)
                        index[i] = i;  
                int k,neiLeft  = neiSize-1;

                vector<vector<unsigned long> > neiSubset(num);
                for (i=0;i<num;i++) 
                {// every time choose  a neighor
                        j = mRandom(0, neiLeft);   //using leda::random_source;   // Using LEDA's random source type
                        //typedef random_source Random;
                        // partitioner ��һ������  Random         mRandom;
                        k=index[j]; //ѡ�� ��(k+1)�� neighbour
                        index[j] = index[neiLeft]; 
                        neiLeft--;

                        /////�� nei[k]   ת��Ϊ  vector_solu�� push_back to neiSubset


                        //����ֵ����Ȼֻ�ǣ�����������vessel�� id
                        neiSubset[i]=nei[k];   //��Ϊ��ʼ��size,����� push, ����ԭ����ĩβ�����µ�Ԫ�ء���size���ϱ��
                }

                return neiSubset;
        }


}


/*vector<unsigned long> BAPSAPartitioner::selectBestFromNei(vector<vector<unsigned long> >& neiSubset, vector<unsigned long> curSolution)
{
        //�� ����� Ŀ�꺯��ֵ���浽 aNeighor[0]~~~~~
        unsigned long minObj=-1;  //  4294967295
        int minObjIdx=-1;
        unsigned long obj;
        int num=neiSubset.size(); //neighbor �ĸ��� 
        //cout<<"the number of pairs which can exchange:"<<num<<endl;

        vector<unsigned long> virtualreturn(mNumVes+1); 
        vector<unsigned long> temp;
        vector<vector<unsigned long> > neiSubsetVector(num); 
        int ves1,ves2,sec1,sec2;
        for(int i=0;i<num;i++)
        {//�Ȱ� neiSubset ��Ԫ�ظ�Ϊһ�� solutionVector

                temp=curSolution;
                ves1=neiSubset[i][0];
                ves2=neiSubset[i][1];
                sec1=curSolution[ves1];   //��unsigned long ��ֵ���� һ��int�� ���ܻ���-1��û�� ������ �ְ�-1 ��ֵ���� unsigned long
                sec2=curSolution[ves2];

                temp[ves1]=sec2;
                temp[ves2]=sec1;   // exchange

                neiSubsetVector[i]=temp;
                obj=CalcObjValTemp(temp);
                neiSubsetVector[i][0]=obj;
                if(obj<minObj)
                {
                        minObj=obj;
                        minObjIdx=i;

                }
        }

        if(minObjIdx>=0)
        {
                //print exchange ������
                ves1=neiSubset[minObjIdx][0];
                ves2=neiSubset[minObjIdx][1];
                temp=neiSubsetVector[minObjIdx];
                //cout<<"exchange vessels: "<<ves1<<" and "<<ves2<<endl;
                //cout<<"before change: the section is "<<curSolution[ves1]<<"and "<<curSolution[ves2]<<endl;
                //cout<<"after  change: the section is "<<temp[ves1]<<"and "<<temp[ves2]<<endl;
                //cout<<"obj value is "<<temp[0]<<endl;



                return temp;
        }
        else
        {cout<<"initializing minObj should change to a more large number..!!!!!! selectBestFromNei()"<<endl;
        assert(1>2);
        return virtualreturn;
        }
}

*/

vector<unsigned long> BAPSAPartitioner::selectBestFromNei(vector<vector<unsigned long> >& neiSubset, vector<unsigned long> curSolution)
{
//�� ����� Ŀ�꺯��ֵ���浽 aNeighor[0]~~~~~
	int num=neiSubset.size(); //neighbor �ĸ��� 
	 //cout<<"the number of pairs which can exchange:"<<num<<endl;
	 int k,ves1,ves2,sec1,sec2;
	// unsigned long obj;
	 vector<unsigned long> temp=curSolution;
	  unsigned long obj2;
	if(num<10)
	{// Randomly choose one neighbor

		k=mRandom(0, num-1);
		//cout<<"random select neighbor "<<k<<endl;
		ves1=neiSubset[k][0];
		ves2=neiSubset[k][1];
		sec1=curSolution[ves1];   //��unsigned long ��ֵ���� һ��int�� ���ܻ���-1��û�� ������ �ְ�-1 ��ֵ���� unsigned long
		sec2=curSolution[ves2];
		temp[ves1]=sec2;
		temp[ves2]=sec1;   // exchange
		//obj=CalcObjValTemp(temp);
		obj2=CalcObjValByDelt(ves1,ves2,curSolution);
		// cout<<"obj="<<obj<<" and obj2="<<obj2<<endl;
		temp[0]=obj2;
		//cout<<"exchange vessels: "<<ves1<<" and "<<ves2<<endl;
		//cout<<"before change: the section is "<<curSolution[ves1]<<" and "<<curSolution[ves2]<<endl;
		//cout<<"after  change: the section is "<<temp[ves1]<<" and "<<temp[ves2]<<endl;
		//cout<<"obj value is "<<temp[0]<<endl;
		return temp;
	}
	else
	{unsigned long minObj=-1;  //  4294967295
	int minObjIdx=-1;
	


	vector<unsigned long> virtualreturn(mNumVes+1); 
	
	vector<vector<unsigned long> > neiSubsetVector(num); 
	
	for(int i=0;i<num;i++)
	{//�Ȱ� neiSubset ��Ԫ�ظ�Ϊһ�� solutionVector

		temp=curSolution;// �൱�� �ָ� curSolution
		ves1=neiSubset[i][0];
		ves2=neiSubset[i][1];
		sec1=curSolution[ves1];   //��unsigned long ��ֵ���� һ��int�� ���ܻ���-1��û�� ������ �ְ�-1 ��ֵ���� unsigned long
		sec2=curSolution[ves2];

		temp[ves1]=sec2;
		temp[ves2]=sec1;   // exchange

		neiSubsetVector[i]=temp;
		//obj=CalcObjValTemp(temp);
		obj2=CalcObjValByDelt(ves1,ves2,curSolution);
		// cout<<"obj="<<obj<<" and obj2="<<obj2<<endl;
		neiSubsetVector[i][0]=obj2;
		if(obj2<minObj)
		{
			minObj=obj2;
			minObjIdx=i;

		}
	}

	if(minObjIdx>=0)
	{
		//print exchange ������
		ves1=neiSubset[minObjIdx][0];
		ves2=neiSubset[minObjIdx][1];
		temp=neiSubsetVector[minObjIdx];
		//cout<<"exchange vessels: "<<ves1<<" and "<<ves2<<endl;
	//	cout<<"before change: the section is "<<curSolution[ves1]<<" and "<<curSolution[ves2]<<endl;
		//cout<<"after  change: the section is "<<temp[ves1]<<" and "<<temp[ves2]<<endl;
		//cout<<"obj value is "<<temp[0]<<endl;



		return temp;
	}
	else
	{cout<<"initializing minObj should change to a more large number..!!!!!! selectBestFromNei()"<<endl;
	assert(1>2);
	return virtualreturn;
	}


	}
}


void BAPSAPartitioner::clearSoln(){

        InitSolution();
        set<int>  tempVessels; //
        int temp;
        for(int i=1;i<=mNumSect;i++)
        {
                tempVessels=mSect[i].Vessels();  //set<int> ����� vessel�� id��  ����  remove 
                while(!tempVessels.empty())
                {
                        temp=tempVessels.choose();
                        mSect[i].Remove(mVes[temp]); // remove  a vessel
                        tempVessels.del(temp);
                }

        }
}

////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void BAPSAPartitioner::CalcInitialObjVal()
{
        ComputeObjVal(mTranshipment, mPenalty);
}


void BAPSAPartitioner::ComputeObjVal(unsigned long& aTrans,
        unsigned long& aPenalty) const
{
        aTrans = aPenalty = 0;

        for (int v1 = 1; v1 <= mNumVes; v1++)
        {
                const int&        s1 = mVes[v1].Section();
                const set<int>&   neighbours = mVes[v1].Neighbours();
                int               v2;

                // Calculate inter-vessel transhipment costs
                forall(v2, neighbours)
                {
                        if (v2 < v1) continue;

                        const int&     s2 = mVes[v2].Section();

                        if (s1 == UNASSIGNED  ||  s2 == UNASSIGNED)
                                aPenalty += TotalFlow(v1, v2) * LONGDISTANCE;
                        else
                        {
                                aTrans += TotalFlow(v1, v2) * D(s1, s2);
                                //            cout  << setw(4) << v1 << setw(5) << v2
                                //                  << setw(10) << TotalFlow(v1, v2) << " * "
                                //                  << setw(5) << D(s1, s2) << " = "
                                //                  << TotalFlow(v1, v2) * D(s1, s2)
                                //                  << "     Trans = " << aTrans << endl;
                        }
                }

                // Calculate import,export costs
                if (s1 == UNASSIGNED)
                        aPenalty += (mVes[v1].Import() + mVes[v1].Export()) * LONGDISTANCE;
                else
                {
                        aTrans += (mVes[v1].Import() + mVes[v1].Export()) * D(s1, 0);
                        //            cout  << setw(4) << v1 << " ("
                        //                  << setw(5) << mVes[v1].Import() << " + "
                        //                  << setw(5) << mVes[v1].Export() << ") * "
                        //                  << setw(5) << D(s1, 0) << " = "
                        //                  << (mVes[v1].Import() + mVes[v1].Export()) * D(s1, 0)
                        //                  << "     Trans = " << aTrans << endl;
                }
        }
}


unsigned long BAPSAPartitioner::CalcObjVal() const
{
        unsigned long  ObjVal = mTranshipment + mPenalty;

        // Safeguards against overflow
        assert(ObjVal >= mTranshipment);
        assert(ObjVal >= mPenalty);

        return ObjVal;
}

unsigned int BAPSAPartitioner::TotalFlow(const int& v1, const int& v2) const
{
        return (unsigned int) (mTrans(v1, v2) + mTrans(v2, v1));
}

unsigned int BAPSAPartitioner::TotalFlow(const SAVessel& v1, const SAVessel& v2) const
{
        return (unsigned int) (mTrans(v1.ID(), v2.ID()) + mTrans(v2.ID(), v1.ID()));
}

unsigned int BAPSAPartitioner::D(const int& s1, const int& s2) const
{
        if (s1 >= 0  &&  s2 >= 0)           // take port into account also
                return (unsigned int) mDist(s1, s2);
        else
                return LONGDISTANCE;
}

unsigned int BAPSAPartitioner::D(const SASection& s1, const SASection& s2) const
{
        if (s1.ID() >= 0  &&  s2.ID() >= 0) // take port into account also
                return (unsigned int) mDist(s1.ID(), s2.ID());
        else
                return LONGDISTANCE;
}


void BAPSAPartitioner::UpdatePackage() const
{
        for (int i = 1; i <= mNumVes; i++)
                mPackage.SectionAssignedTo(i, mVes[i].Section());

        mPackage.TranshipmentCost(mTranshipment);

        // What happened to the penalty?!?
}


void BAPSAPartitioner::ReadParameterFile()
{
        string   paramFile = mPackage.ParamFilename();
        char     buf[255];
        string   token, mode = "nil";
        ifstream ParamFile(paramFile);

        if (!ParamFile)
        {
                cerr << "Cannot open parameter file: " << paramFile << endl;
                return;
        }

        while (!ParamFile.eof())
        {
                ParamFile.getline(buf,80);

#ifdef _DEBUG
                cout << "tokenizing: " << buf << endl;
#endif

                for (int i = 0; i < 80; i++)     // Convert carriage-return to space
                        if (13 == (int) buf[i])
                                buf[i] = ' ';

                if (buf[0] == ' ' || buf[0] == 0)
                        continue;

                token = strtok(buf, " ");

#ifdef _DEBUG
                cout << "token = " << token << endl;
#endif

                if (token.length() == 0)         // empty line, ignore
                        continue;
                if (token == "#")                // comment line, ignore
                        continue; 
                if (token[0] == '_')             // keyword, change mode
                        mode = token;
                else                             // process data based on mode
                {
                        if (mode == "_RANDOM_SEED")
                        {
                                mRandomSeed = atoi(token);
                                mRandom.set_seed(mRandomSeed);
                        }  
                        else if (mode == "_PRINT_SECTIONS")
                        {
                                mPrintSections = atoi(token);
                        }
                        else if (mode == "_PRINT_VESSELS")
                        {
                                mPrintVessels = atoi(token);
                        }
                        else if (mode == "_RUNTIME_ANALYZER")
                        {
                                mRuntimeAnalyzer = atoi(token);
                        }
                        else if (mode == "_SUMMARY")
                        {
                                mSummary = atoi(token);
                        }
                        // else nothing
                }  
        }
}


void BAPSAPartitioner::WriteSolutionFile() const
{
        string   solutionFile = mPackage.PartitioningFilename(),
                paramFile = mPackage.ParamFilename();

        ofstream SolFile(solutionFile);

        if (!SolFile)
        {
                cerr << "Cannot create solution file: " << solutionFile << endl;
                return;
        }

        // Write comments
        // ( Date() auto-inserts a carriage-return! )
        SolFile  << "#" << endl
                << "# Solution File created by BAPSAPartitioner" << endl
                << "# Date = " << Date()
                << "# Parameter File = " << paramFile << endl
                << "#" << endl << endl;

        SolFile  << "_NUM_UNALLOCATED" << endl
                << mUnallocVes.size() << endl << endl;

        // Reads from the BAP package
        const int V = mNumVes;

        // Write solution information
        SolFile.setf(ios::fixed);
        SolFile  << "_NUM_VESSELS" << endl
                << V + 1 << endl           // n+1 to account for port
                << endl
                << "_OBJECTIVE_VALUE" << endl
                << setprecision(2) << mTranshipment + mPenalty << endl
                << endl
                << "_TRANSHIPMENT_VALUE" << endl
                << setprecision(2) << mTranshipment << endl
                << endl
                << "_PENALTY_VALUE" << endl
                << setprecision(2) << mPenalty << endl
                << endl
                << "_TIME_TAKEN" << endl
                << setprecision(6)
                << (mEndTime.tv_sec - mStartTime.tv_sec) +
                (mEndTime.tv_usec - mStartTime.tv_usec) / 1000000.0
                << endl
                << endl;
        SolFile.setf(ios::fixed, ios::floatfield);

        // Write allocated vessels
        array<int>  Arrivals = mPackage.Arrivals(),
                Departures = mPackage.Departures();

        SolFile  << "# Final Allocation Solution" << endl
                << "# <ves#> <sect> <wharf> <berth time> <departure time>"
                << endl << endl
                << "_ALLOCATION" << endl;

        SolFile  << "0 0 0 0 0" << endl;    // Write the sea port

        for (int i = 1; i <= V; i++)
                if (mVes[i].Section() > 0)
                        SolFile  << i << " " << mVes[i].Section() << " " << -1
                        << " " << Arrivals[i] << " " << Departures[i]
                << endl;

                // Write unallocated vessels, if any
                if (mUnallocVes.size() > 0)
                {
                        SolFile  << endl << "_UNALLOCATED_VESSELS" << endl;

                        for (int i = 1; i <= V; i++)
                                if (mVes[i].Section() < 0)
                                        SolFile  << i << " " << -1 << " " << -1
                                        << " " << Arrivals[i] << " " << Departures[i]
                                << endl;
                }

                SolFile << endl << endl;

                SolFile.close();
}


void BAPSAPartitioner::WriteTraceFile(string aStr) const
{
        ofstream TraceFile(mTraceFile, ios::app);
        TraceFile  << aStr << endl;
        TraceFile.close();
}

void BAPSAPartitioner::WriteTraceFile(long aLong) const
{
        ofstream TraceFile(mTraceFile, ios::app);
        TraceFile   << aLong << endl;
        TraceFile.close();
}

void BAPSAPartitioner::WriteTraceFile(long double aDouble) const
{
        ofstream TraceFile(mTraceFile, ios::app);
        TraceFile.setf(ios::fixed);
        TraceFile  << setprecision(2) << aDouble << endl;
        TraceFile.close();
}


string BAPSAPartitioner::Date() const
{
        //
        // Query date and time
        //
        struct tm   *TimePtr;
        time_t      Time;

        time(&Time);
        TimePtr = localtime(&Time);

        return string(asctime(TimePtr));
}

// GenerateInitialSolution creates a vessel partitioning solution. 
// At the moment, it creates a random initial solution.
//   Called by: Solve();
//
void BAPSAPartitioner::GenerateInitialSolution()
{
        InitSolution();
        //GenSolnRandom();
        // GenSolnGreedy();
        GenSolnSA();
}


// This method actually create the random vessel partitioning
// solution. Each vessel is assigned a random section
// At the moment, it creates a random initial solution.
//   Called by: Solve();
void BAPSAPartitioner::GenSolnRandom()
{
        // List of candidate vessels for allocation
        array<int>  V(1, mNumVes);

        for (int i = 1; i <= mNumVes; i++)
                V[i] = i;                        // set content to vessel ID // ���index

        // Loop to do actual allocation
        int   i, VesselsLeft = mNumVes;
        // ����� random �㷨�У�VesselsLeft ����������������ģ�V���������index�ģ���greedy �㷨������������ ���ܶ�����Ҫ������
        while (VesselsLeft > 0)
        {
                i = mRandom(1, VesselsLeft);    // randomly pick a vessel  

                int         vIndex = V[i];
                SAVessel&   v = mVes[vIndex];
                AssignVesselToRandomSection(v);  
                // SelectSection_minIncreaseTranship(v);
                // remove assigned vessel by replacing it with last vessel
                V[i] = V[VesselsLeft]; // if i==vesselsLeft then,v[i] doesn't change, never be considered in next while(because vesselsLeft decrease)
                // else 1--9 choose 3, next time v[3]=9; if next time random(1,8)=3, then consider vessel 9;;
                //3 will not be consiered again

                VesselsLeft--;
        }
}

void BAPSAPartitioner::GenSolnGreedy()
{
        // List of candidate vessels for allocation
        array<int>  V(1, mNumVes);
        for (int i = 1; i <= mNumVes; i++)
                V[i] = i;
        int   i,VesselsLeft = mNumVes;  //ѭ������������ unallocated vessels, ��Ϊ��ʱ�㷨������֮��unallocated Ҳ���ܲ�Ϊ�ա���

        while (VesselsLeft>0)
        { // ÿ�� ѭ������һֻ vessel
                // i = mRandom(1, VesselsLeft);  //!!!!!   // randomly pick a vessel  !!!! change the rule of selection of vessels
                i=SelectVessel_earliestDeparture(VesselsLeft,V);
                int         vIndex = V[i];
                SAVessel&   v = mVes[vIndex];
                //AssignVesselToRandomSection(v);  
                SelectSection_minIncreaseTranship(v);
                // remove assigned vessel by replacing it with last vessel
                V[i] = V[VesselsLeft]; // if i==vesselsLeft then,v[i] doesn't change, never be considered in next while(because vesselsLeft decrease)
                // else 1--9 choose 3, next time v[3]=9; if next time random(1,8)=3, then consider vessel 9;;
                //3 will not be consiered again

                VesselsLeft--;
        }
}
int BAPSAPartitioner::SelectVessel_earliestDeparture(int VesselsLeft,array<int>& V) // ���û��� ��ֵ����������
{
        int smallestDepTime = INFINITY;
        int smallestDepV = -1;  // �������������� ���� V ���±꣬����ֵ
        int vIndex;
        //SAVessel&   v;  //�˴��������ñ����ʼ��������
        // ֻ���� V��ǰ VesselsLeft ��Ԫ��
        for(int i=1; i<=VesselsLeft; i++)//select a Departure Time smallest vessel
        {
                vIndex = V[i];
                SAVessel&   v = mVes[vIndex];
                if( v.Departure() < smallestDepTime )
                {
                        smallestDepV = i;
                        smallestDepTime = v.Departure();
                }

        }
        return smallestDepV;
}


void BAPSAPartitioner::SelectSection_minIncreaseTranship(SAVessel& v)
{// ���� ���� �� ���ã���Ϊ ������һ������Assign��v,section��, �� v�޸��ˡ����� 


        // List of candidate sections to be assigned to v
        set<int> sectSet = v.Destinations();   // Copy, preserve original
        array<int> S(1, sectSet.size());       // Array of destination sections
        int   i = 1, k;

        while (!sectSet.empty())
        {
                k = sectSet.choose();
                sectSet.del(k);
                S[i++] = k;
        }

        const set<int>&   neighbours = v.Neighbours();
        int v2;
        // Loop to do actual assignment
        int SectionsLeft = S.size();
        int increaseTranship=0;
        int min=INFINITY;
        int minIndex=0;////��
        i=1;

        // S�д�����±�index, ��� ���е� section �Ƿ� canAccommodate (v).
        while (i<=SectionsLeft)
        {
                k = S[i];

                if (mSect[k].CanAccommodate(v))
                { 
                        const int& sv=mSect[k].ID();
                        // Calculate inter-vessel transhipment costs
                        increaseTranship=0;
                        forall(v2, neighbours)
                        {
                                const int&     s2 = mVes[v2].Section();
                                if (s2 != UNASSIGNED )
                                        increaseTranship += TotalFlow(v.ID(), v2) * D(sv, s2); // TotalFlow �������� ��int����Ҳ����

                                //            cout  << setw(4) << v1 << setw(5) << v2
                                //                  << setw(10) << TotalFlow(v1, v2) << " * "
                                //                  << setw(5) << D(s1, s2) << " = "
                                //                  << TotalFlow(v1, v2) * D(s1, s2)
                                //                  << "     Trans = " << aTrans << endl;
                        }

                        if (increaseTranship<min)
                        {  
                                min=increaseTranship;
                                minIndex=i;
                        }
                }

                i++;

        }
        if (minIndex!=0)
        {
                k = S[minIndex];
                Assign(v, mSect[k]);
        }

}
// Assigns Vessel v to a random section
//   Alg: save the candidate destination sections
//        for each randomly selected section
//        assign to it if there is room for it
//        else remove the section.
//   Note:vessel is unassigned if all sections fails.
//
void BAPSAPartitioner::AssignVesselToRandomSection(SAVessel& v)
{
        // List of candidate sections to be assigned to v
        set<int> sectSet = v.Destinations();   // Copy, preserve original
        array<int> S(1, sectSet.size());       // Array of destination sections
        int   i = 1, k;

        while (!sectSet.empty())
        {
                k = sectSet.choose();
                sectSet.del(k);
                S[i++] = k;
        }

        // Loop to do actual assignment
        int   SectionsLeft = S.size();
        // S�д�����±�index, ��� ���е� section �Ƿ� canAccommodate (v).
        while (SectionsLeft > 0  &&  v.Section() == UNASSIGNED)
        {
                i = mRandom(1, SectionsLeft);  // randomly pick a section
                int      k = S[i];

                if (mSect[k].CanAccommodate(v))
                        Assign(v, mSect[k]);
                else
                        S[i--] = S[SectionsLeft--]; // remove from further consideration
        }
}


void BAPSAPartitioner::Assign(SAVessel& v, SASection& s)
{
        v.Section(s.ID());
        v.RemoveDestination(s.ID());
        s.Add(v);
        mUnallocVes.del(v.ID());
}


