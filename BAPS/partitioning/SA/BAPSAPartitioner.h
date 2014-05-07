#include "def.h"
#include "BAPPackage.h"
#include "BAPPartitioner.h"
#include "BAPSASection.h"
#include "BAPSAVessel.h"
#include <LEDA/core/list.h>
#include <LEDA/core/random_source.h>
#include <sys/time.h>
#include </usr/include/time.h>

#include <vector>
using std::vector;
using leda::random_source;

// Using LEDA's random source type
typedef random_source Random;

class BAPSAPartitioner : public BAPPartitioner
{
public:
   BAPSAPartitioner(BAPPackage& aPackage);
   ~BAPSAPartitioner();

   void Solve();
   // Debugging facility
   void Print(const int& aWidth = 1, const int& aDetail = 0) const;


private:
   // I/O methods
   string Date() const;
   void PrintSummary() const;
   void ReadParameterFile();
   void WriteSolutionFile() const;
   void WriteTraceFile(string aStr) const;
   void WriteTraceFile(long aLong) const;
   void WriteTraceFile(long double aDouble) const;

   // Command-line methods
   // Initial solution generation methods

   // Abstractions
   void Assign(SAVessel& v, SASection& s);
   void AssignVesselToRandomSection(SAVessel& v);
   void CalcInitialObjVal();
   unsigned long CalcObjVal() const;
   void ComputeObjVal(unsigned long& aTrans, unsigned long& aPenalty) const;
   void GenerateInitialSolution();
   void GenSolnRandom();
   void GenSolnGreedy();////////////
   int SelectVessel_earliestDeparture(int VesselsLeft,array<int>& V);/////////////////
   void SelectSection_minIncreaseTranship(SAVessel& v);//////////////////
   //////////////////////////////////////////////////////////////////////////
   void GenSolnSA();
   vector<unsigned long> ConvSoltoVec();
   void ConvVectoSol(vector<unsigned long> xx);
   void CalcObjValTemp(vector<unsigned long>& xx );
   //>>>>>>>>>>>>>>>>>>>>
   unsigned long CalcObjValByDelt(int vesi,int vesj,vector<unsigned long> curSolution);
   vector<unsigned long> SelectANeighorRanGeedy(vector<unsigned long>& curSolution);
   vector<unsigned long> SelectANeighorRandomTotally(vector<unsigned long>& curSolution);
   vector<unsigned long> SelectANeighbor_ChangeOneVessel(vector<unsigned long>& curSolution);//liu
   vector<unsigned long> SelectANeighborRandomly(vector<unsigned long>& curSolution);//liu
   vector<unsigned long> BAPSAPartitioner::SelectANeighbor_RandomTentoBestOne(vector<unsigned long>& curSolution);//liu
   vector<unsigned long> SelectANeighorOneVes(vector<unsigned long>& curSolution);
   vector<vector<unsigned long> > GenNeighborhood(vector<unsigned long> curSolution);
   vector<unsigned long> SelectRanFromNeighborhood(vector<vector<unsigned long> >& nei,vector<unsigned long>& curSolution);

   bool CanExchange(int vesi,int vesj);
   vector<vector<unsigned long> > RandomGenSubset(vector<vector<unsigned long> >& nei);
   vector<unsigned long> selectBestFromNei(vector<vector<unsigned long> >& neiSubset, vector<unsigned long> curSolution);
   void clearSoln();
void printSolu(vector<unsigned long>& curSolution);
   //////////////////////////////////////////////////////////////////////////////////////////////

   inline bool GotMoreMoves() const;
   void InitSolution();
   void ResetVesselDestinations();
   void UpdatePackage() const;

   unsigned int TotalFlow(const int& v1, const int& v2) const;
   unsigned int TotalFlow(const SAVessel& v1, const SAVessel& v2) const;
   unsigned int D(const int& s1, const int& s2) const;
   unsigned int D(const SASection& s1, const SASection& s2) const;

   // Constants
   static const unsigned int  LONGDISTANCE = INFINITY;
   static const int           UNASSIGNED = -1;

   // Debug control
   unsigned int   mOutput;
   unsigned int   mPrintBucket;
   unsigned int   mPrintSections;
   unsigned int   mPrintVessels;
   unsigned int   mRuntimeAnalyzer;
   unsigned int   mSummary;
   string         mTraceFile;

   // Algorithm control
   timeval        mStartTime, mEndTime;
   int            mRandomSeed;
   unsigned int   mMaxPasses;
   unsigned int   mInitialSolnGenerator;
   bool           mSolnExists;

   // Information
   unsigned long  mTranshipment;
   unsigned long  mPenalty;

   // Data structures
   const int      mNumVes;
   const int      mNumSect;
   array<SAVessel>  mVes;
   array<SASection> mSect;
   array2<int>    mTrans;
   array2<int>    mDist;
   set<int>       mUnallocVes;
   Random         mRandom;
};

