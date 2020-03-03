/* *********************************************************************** *
 * File   : arraylist.h                               Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */
#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <string.h>
#include "essential.h"

template <class Type> class ArrayList;


// This is a really wierd class. It is a FIFO in the sense that items
// are appended. However, items can be removed from anywhere within the
// list, besides the remove_head operation.
//
// The class is implemented as an array for performance reasons only, and
// was designed for use with my timer system.
//

template <class Type>
class ArrayList
{
  public:
   ArrayList(int i);
   virtual ~ArrayList();

   inline int is_empty(void);
   inline int is_full(void);
   inline int size(void);      // Total size of the array.
   inline int used(void);      // Currently used in the array.

   Type *insert(void);         // Make room for element and returns ref for it.
   void remove(Type *);        // Remove oldest element.

   Type *head(void);           // Makes this class obscure... what the hex.
   

   virtual Type *find(int i);  // Function to find an object. NULL if not found

  protected:
   int nSize;   // Total size of the array
   int nUsed;   // How many elements currently in the array

   Type *pArray;
   int *pIdx;

};




template <class Type>
ArrayList<Type>::ArrayList(int nSz)
{
   nSize = nSz;
   nUsed = 0;

   pArray = new Type[nSize];
   assert(pArray != NULL);

   pIdx = new int[nSize];
   assert(pIdx != NULL);

   for (int i=0; i < nSize; i++)
     pIdx[i] = i;
}


template <class Type>
ArrayList<Type>::~ArrayList()
{
   delete pArray;
   delete pIdx;
}


template <class Type>
int ArrayList<Type>::is_empty(void)
{
   return nUsed == 0;
}

template <class Type>
int ArrayList<Type>::is_full(void)
{
   return nUsed == nSize;
}

template <class Type>
int ArrayList<Type>::size(void)
{
   return nSize;
}


template <class Type>
int ArrayList<Type>::used(void)
{
   return nUsed;
}


template <class Type>
Type * ArrayList<Type>::insert(void)
{
   assert(!is_full());

   nUsed++;

   return &pArray[pIdx[nUsed - 1]];
}


template <class Type>
Type *ArrayList<Type>::head(void)
{
   assert(!is_empty());

   return &pArray[pIdx[0]];
}


template <class Type>
Type *ArrayList<Type>::find(int i)
{
   assert(!is_empty());
   assert(i >= 0 && i < nSize);

   if (i+1 > nUsed)
     return NULL;
   else
     return &pArray[pIdx[i]];
}


template <class Type>
void ArrayList<Type>::remove(Type *pRef)
{
   int i, tmp;

   assert(!is_empty());
   assert(pRef);

   for (i=0; i < nUsed; i++)
     if (&pArray[pIdx[i]] == pRef)
       break;

   assert(i < nUsed);

   if (i < nSize)
   {
      tmp = pIdx[i];
      memmove(&pIdx[i], &pIdx[i+1], sizeof(int) * (nSize - i - 1));
      pIdx[nSize-1] = tmp;
   }
   

   nUsed--;
}
#endif
