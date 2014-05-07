/******************************************************************
 *
 * Filename    : BAPGREEDYSection.h
 * Author      : David Ong Tat-Wee
 *
 * Version     : 1.01b
 * Date        : 10 Jun 98
 *
 * Description : Implements the GREEDYSection class (?)
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

#ifndef __BAP_GREEDY_SECTION__
#define __BAP_GREEDY_SECTION__


#include "def.h"
#include "BAPGREEDYVessel.h"

using std::istream;
using std::ostream;
using leda::array;
using leda::set;

class GREEDYSection
{
public:
   GREEDYSection();
   GREEDYSection(int anID, int aMaxTimeZone, int aLength);
   GREEDYSection(const GREEDYSection& aSection);
   ~GREEDYSection();
   GREEDYSection& operator=(const GREEDYSection& aSection);
   void Print(const int& aWidth = 1, const int& aDetail = 0) const;

   friend istream& operator>>(istream& anIS, GREEDYSection& aSection);
   friend ostream& operator<<(ostream& anOS, const GREEDYSection& aSection);
   friend int compare(const GREEDYSection& aS1, const GREEDYSection& aS2);

   int         ID() const;
   int         Length() const;
   bool        CanAccommodate(const GREEDYVessel& aVessel) const;
   const set<int>&   Vessels() const;

   void        Add(const GREEDYVessel& aVessel);
   void        Remove(const GREEDYVessel& aVessel);

private:
   int         mID;
   int         mMaxTimeZone;
   int         mLength;
   array<int>  mCapacity;
   set<int>    mVessels;
};


#endif
