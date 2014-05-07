/******************************************************************
 *
 * Filename    : BAPSASection.h
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

#ifndef __BAP_SA_SECTION__
#define __BAP_SA_SECTION__


#include "def.h"
#include "BAPSAVessel.h"

using std::istream;
using std::ostream;
using leda::array;
using leda::set;

class SASection
{
public:
   SASection();
   SASection(int anID, int aMaxTimeZone, int aLength);
   SASection(const SASection& aSection);
   ~SASection();
   SASection& operator=(const SASection& aSection);
   void Print(const int& aWidth = 1, const int& aDetail = 0) const;

   friend istream& operator>>(istream& anIS, SASection& aSection);
   friend ostream& operator<<(ostream& anOS, const SASection& aSection);
   friend int compare(const SASection& aS1, const SASection& aS2);

   int         ID() const;
   int         Length() const;
   bool        CanAccommodate(const SAVessel& aVessel) const;
   const set<int>&   Vessels() const;

   void        Add(const SAVessel& aVessel);
   void        Remove(const SAVessel& aVessel);

private:
   int         mID;
   int         mMaxTimeZone;
   int         mLength;
   array<int>  mCapacity;
   set<int>    mVessels;
};


#endif
