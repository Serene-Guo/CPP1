/******************************************************************
 *
 * Filename    : BAPGREEDYVessel.cpp
 * Author      : David Ong Tat-Wee
 *
 * Version     : 1.01b
 * Date        : 10 Jun 98
 *
 * Description : Implements the GREEDYVessel class (?)
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

#include "BAPGREEDYVessel.h"

using std::cout;
using std::endl;


GREEDYVessel::GREEDYVessel()
: mID(0), mLength(0), mStartTimeZone(0), mEndTimeZone(0), mSection(0),
  mImport(0), mExport(0), mArrival(0), mDeparture(0), mContainers(0),
  mTranshipment(0)
{
}


GREEDYVessel::GREEDYVessel(int anID, int aLength)
: mID(anID), mLength(aLength), mStartTimeZone(0), mEndTimeZone(0),
  mSection(0), mImport(0), mExport(0), mArrival(0), mDeparture(0),
  mContainers(0), mTranshipment(0)
{
}


GREEDYVessel::GREEDYVessel(const GREEDYVessel& aVessel)
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


GREEDYVessel::~GREEDYVessel()
{
}


GREEDYVessel& GREEDYVessel::operator=(const GREEDYVessel& aVessel)
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


void GREEDYVessel::Print(const int& aWidth, const int& aDetail) const
{
   cout  << "GREEDYVessel" << setw(5) << mID
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


istream& operator>>(istream& anIS, GREEDYVessel& aVessel)
{
   return anIS;
}


ostream& operator<<(ostream& anOS, const GREEDYVessel& aVessel)
{
   return anOS << "vessel " << setw(3) << aVessel.mID;
}


int compare(const GREEDYVessel& aV1, const GREEDYVessel& aV2)
{
   return compare(aV1.mID, aV2.mID);
}


int GREEDYVessel::ID() const
{
   return mID;
}


int GREEDYVessel::Length() const
{
   return mLength;
}


int GREEDYVessel::Import() const
{
   return mImport;
}


int GREEDYVessel::Export() const
{
   return mExport;
}


int GREEDYVessel::Transhipment() const
{
   return mTranshipment;
}


int GREEDYVessel::Containers() const
{
   return mContainers;
}


int GREEDYVessel::StartTimeZone() const
{
   return mStartTimeZone;
}


int GREEDYVessel::EndTimeZone() const
{
   return mEndTimeZone;
}


int GREEDYVessel::Arrival() const
{
   return mArrival;
}


int GREEDYVessel::Departure() const
{
   return mDeparture;
}


int GREEDYVessel::Section() const
{
   return mSection;
}


const set<int>& GREEDYVessel::Neighbours() const
{
   return mNeighbours;
}


const set<int>& GREEDYVessel::Destinations() const
{
   return mPotentialDestinations;
}


int GREEDYVessel::Import(int aNumContainers)
{
   mContainers += aNumContainers;
   mImport = aNumContainers;
   return mImport;
}


int GREEDYVessel::Export(int aNumContainers)
{
   mContainers += aNumContainers;
   mExport = aNumContainers;
   return mExport;
}


void GREEDYVessel::AddTranshipment(int aNumContainers)
{
   mContainers += aNumContainers;
   mTranshipment += aNumContainers;
}


int GREEDYVessel::StartTimeZone(int aTimeZone)
{
   mStartTimeZone = aTimeZone;
   return mStartTimeZone;
}


int GREEDYVessel::EndTimeZone(int aTimeZone)
{
   mEndTimeZone = aTimeZone;
   return mEndTimeZone;
}


int GREEDYVessel::Arrival(int aTime)
{
   mArrival = aTime;
   return mArrival;
}


int GREEDYVessel::Departure(int aTime)
{
   mDeparture = aTime;
   return mDeparture;
}


int GREEDYVessel::Section(int aSection)
{
   mSection = aSection;
   return mSection;
}


void GREEDYVessel::AddNeighbour(int aVessel)
{
   assert(!mNeighbours.member(aVessel));
   mNeighbours.insert(aVessel);
}


void GREEDYVessel::AddDestination(int aSection)
{
   mPotentialDestinations.insert(aSection);
}


void GREEDYVessel::RemoveDestination(int aSection)
{
   assert(mPotentialDestinations.member(aSection));
   mPotentialDestinations.del(aSection);
}

