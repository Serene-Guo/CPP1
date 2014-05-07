/******************************************************************
 *
 * Filename    : BAPSASection.cpp
 * Author      : David Ong Tat-Wee
 *
 * Version     : 1.01b
 * Date        : 10 Jun 98
 *
 * Description : Implements the SASection class (?)
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

#include "BAPSASection.h"

using std::cout;
using std::endl;

SASection::SASection()
: mID(0), mMaxTimeZone(0), mLength(0)
{
}


SASection::SASection(int anID, int aMaxTimeZone, int aLength)
: mID(anID), mMaxTimeZone(aMaxTimeZone), mLength(aLength),
  mCapacity(array<int>(1, aMaxTimeZone))
{
   for (int i = 1; i <= aMaxTimeZone; i++)
   {
      mCapacity[i] = aLength;
   }
}


SASection::SASection(const SASection& aSection)
: mID(aSection.mID),
  mLength(aSection.mLength),
  mMaxTimeZone(aSection.mMaxTimeZone),
  mCapacity(array<int>(aSection.mCapacity.low(),
                       aSection.mCapacity.high())),
  mVessels(aSection.mVessels)
{
   for (int i = 1; i <= mMaxTimeZone; i++)
   {
      mCapacity[i] = aSection.mCapacity[i];
   }
}


SASection::~SASection()
{
}


SASection& SASection::operator=(const SASection& aSection)
{
   if (this != &aSection)
   {
      mID = aSection.mID;
      mLength = aSection.mLength;
      mMaxTimeZone = aSection.mMaxTimeZone;
      mCapacity = aSection.mCapacity;
      mVessels = aSection.mVessels;
   }

   return (*this);
}


void SASection::Print(const int& aWidth, const int& aDetail) const
{
   cout  << "SASection " << mID << ", length = " << mLength
         << ", max time zone = " << mMaxTimeZone << endl;

   if (aDetail > 1)
   {
      cout  << "capacity:" << endl;

      for (int i = 1; i <= mMaxTimeZone; i++)
      {
         cout  << setw(8) << mCapacity[i];
      }

      cout << endl;
   }

   cout << "vessels in section =" << endl;

   int   v;

   forall(v, mVessels)
   {
      cout  << setw(5) << v;
   }

   cout << endl << endl;
}


istream& operator>>(istream& anIS, SASection& aSection)
{
   return anIS;
}


ostream& operator<<(ostream& anOS, const SASection& aSection)
{
   return anOS << "section " << setw(2) << aSection.mID;
}


int compare(const SASection& aS1, const SASection& aS2)
{
   return compare(aS1.mID, aS2.mID);
}

int SASection::ID() const
{
   return mID;
}


int SASection::Length() const
{
   return mLength;
}


bool SASection::CanAccommodate(const SAVessel& aVessel) const
{
   int   VesLen = aVessel.Length();

   for (int i = aVessel.StartTimeZone(); i <= aVessel.EndTimeZone(); i++)
   {
      if (mCapacity[i] < VesLen)
         return false;
   }

   return true;
}


const set<int>& SASection::Vessels() const
{
   return mVessels;
}


void SASection::Add(const SAVessel& aVessel)
{
   assert(!mVessels.member(aVessel.ID()));

   int   VesLen = aVessel.Length();

   for (int i = aVessel.StartTimeZone(); i <= aVessel.EndTimeZone(); i++)
   {
      mCapacity[i] -= VesLen;
      assert(mCapacity[i] >= 0);
   }

   mVessels.insert(aVessel.ID());
}


void SASection::Remove(const SAVessel& aVessel)
{
   assert(mVessels.member(aVessel.ID()));

   int   VesLen = aVessel.Length();

   for (int i = aVessel.StartTimeZone(); i <= aVessel.EndTimeZone(); i++)
   {
      mCapacity[i] += VesLen;
      assert(mCapacity[i] <= mLength);
   }

   mVessels.del(aVessel.ID());
}

