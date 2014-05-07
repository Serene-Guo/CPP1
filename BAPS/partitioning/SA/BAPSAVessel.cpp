/******************************************************************
 *
 * Filename    : BAPSAVessel.cpp
 * Author      : David Ong Tat-Wee
 *
 * Version     : 1.01b
 * Date        : 10 Jun 98
 *
 * Description : Implements the SAVessel class (?)
 *
 * Reference   : nil
 *
 * Notes       : 
 *
 * Changes     : nil
 *
 * Copyright   : Copyright (c) 1998
 *               All rights reserved by
 *               Resource Allocation and Scheduling Group
 *               Department of Information Systems and Computer Science
 *               National University of Singapore
 *
 ******************************************************************/

#include "BAPSAVessel.h"

using std::cout;
using std::endl;


SAVessel::SAVessel()
: mID(0), mLength(0), mStartTimeZone(0), mEndTimeZone(0), mSection(0),
  mImport(0), mExport(0), mArrival(0), mDeparture(0), mContainers(0),
  mTranshipment(0)
{
}


SAVessel::SAVessel(int anID, int aLength)
: mID(anID), mLength(aLength), mStartTimeZone(0), mEndTimeZone(0),
  mSection(0), mImport(0), mExport(0), mArrival(0), mDeparture(0),
  mContainers(0), mTranshipment(0)
{
}


SAVessel::SAVessel(const SAVessel& aVessel)
: mID(aVessel.mID), mLength(aVessel.mLength),
  mImport(aVessel.mImport), mExport(aVessel.mExport),
  mTranshipment(aVessel.mTranshipment),
  mContainers(aVessel.mContainers),
  mStartTimeZone(aVessel.mStartTimeZone),
  mEndTimeZone(aVessel.mEndTimeZone),
  mArrival(aVessel.mArrival),
  mDeparture(aVessel.mDeparture),
  mSection(aVessel.mSection),
  mNeighbours(aVessel.mNeighbours),
  mPotentialDestinations(aVessel.mPotentialDestinations)
{
}


SAVessel::~SAVessel()
{
}


SAVessel& SAVessel::operator=(const SAVessel& aVessel)
{
   if (this != &aVessel)
   {
      mID = aVessel.mID;
      mLength = aVessel.mLength;
      mImport = aVessel.mImport;
      mExport = aVessel.mExport;
      mTranshipment = aVessel.mTranshipment;
      mContainers = aVessel.mContainers;
      mStartTimeZone = aVessel.mStartTimeZone;
      mEndTimeZone = aVessel.mEndTimeZone;
      mArrival = aVessel.mArrival;
      mDeparture = aVessel.mDeparture;
      mSection = aVessel.mSection;
      mNeighbours = aVessel.mNeighbours;
      mPotentialDestinations = aVessel.mPotentialDestinations;
   }

   return (*this);
}


void SAVessel::Print(const int& aWidth, const int& aDetail) const
{
   cout  << "SAVessel" << setw(5) << mID
         << ", length =" << setw(4) << mLength
         << ", [" << setw(3) << mStartTimeZone
         << "," << setw(3) << mEndTimeZone
//         << "] In = " << setw(4) << mImport
//         << " Out = " << setw(4) << mExport
         << "], in section" << setw(3) << mSection << endl;

   if (aDetail > 1)
   {
      cout  << tab(3) << "neighbours =";

      int   v;

      forall(v, mNeighbours)
      {
         cout  << setw(5) << v;
      }

      cout << endl;

      cout  << tab(3) << "destinations =";

      forall(v, mPotentialDestinations)
      {
         cout  << setw(3) << v;
      }

      cout << endl;
   }

   cout << endl;
}


istream& operator>>(istream& anIS, SAVessel& aVessel)
{
   return anIS;
}


ostream& operator<<(ostream& anOS, const SAVessel& aVessel)
{
   return anOS << "vessel " << setw(3) << aVessel.mID;
}


int compare(const SAVessel& aV1, const SAVessel& aV2)
{
   return compare(aV1.mID, aV2.mID);
}


int SAVessel::ID() const
{
   return mID;
}


int SAVessel::Length() const
{
   return mLength;
}


int SAVessel::Import() const
{
   return mImport;
}


int SAVessel::Export() const
{
   return mExport;
}


int SAVessel::Transhipment() const
{
   return mTranshipment;
}


int SAVessel::Containers() const
{
   return mContainers;
}


int SAVessel::StartTimeZone() const
{
   return mStartTimeZone;
}


int SAVessel::EndTimeZone() const
{
   return mEndTimeZone;
}


int SAVessel::Arrival() const
{
   return mArrival;
}


int SAVessel::Departure() const
{
   return mDeparture;
}


int SAVessel::Section() const
{
   return mSection;
}


const set<int>& SAVessel::Neighbours() const
{
   return mNeighbours;
}


const set<int>& SAVessel::Destinations() const
{
   return mPotentialDestinations;
}


int SAVessel::Import(int aNumContainers)
{
   mContainers += aNumContainers;
   mImport = aNumContainers;
   return mImport;
}


int SAVessel::Export(int aNumContainers)
{
   mContainers += aNumContainers;
   mExport = aNumContainers;
   return mExport;
}


void SAVessel::AddTranshipment(int aNumContainers)
{
   mContainers += aNumContainers;
   mTranshipment += aNumContainers;
}


int SAVessel::StartTimeZone(int aTimeZone)
{
   mStartTimeZone = aTimeZone;
   return mStartTimeZone;
}


int SAVessel::EndTimeZone(int aTimeZone)
{
   mEndTimeZone = aTimeZone;
   return mEndTimeZone;
}


int SAVessel::Arrival(int aTime)
{
   mArrival = aTime;
   return mArrival;
}


int SAVessel::Departure(int aTime)
{
   mDeparture = aTime;
   return mDeparture;
}


int SAVessel::Section(int aSection)
{
   mSection = aSection;
   return mSection;
}


void SAVessel::AddNeighbour(int aVessel)
{
   assert(!mNeighbours.member(aVessel));
   mNeighbours.insert(aVessel);
}


void SAVessel::AddDestination(int aSection)
{
   mPotentialDestinations.insert(aSection);
}


void SAVessel::RemoveDestination(int aSection)
{
   assert(mPotentialDestinations.member(aSection));
   mPotentialDestinations.del(aSection);
}

