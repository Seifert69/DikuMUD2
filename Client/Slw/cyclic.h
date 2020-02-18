/* *********************************************************************** *
 * File   : cyclic.h                                  Part of Valhalla MUD *
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
#ifndef CYCLIC_H
#define CYCLIC_H

#include "essential.h"

template <class Type> class CyclicArray;

template <class Type>
class CyclicArray
{
  public:
   CyclicArray(int);
   ~CyclicArray();

   inline int is_empty(void);
   inline int is_full(void);
   inline int size(void);      // Total size of the array.
   inline int used(void);      // Currently used in the array.

   Type *insert(void);         // Make room for element and returns ref for it.
   void remove(void);          // Remove oldest element.
   Type *reference(void);      // Get reference for oldest element.

  protected:
   int nSize;   // Total size of the array
   int nElems;  // How many elements currently in the array
   int nIdx;    // Pointer to the oldest inserted element

   Type *pArray;
};




template <class Type>
CyclicArray<Type>::CyclicArray(int nSz)
{
   nSize  = nSz;
   nIdx   = 0;
   nElems = 0;

   pArray = new Type[nSize];
   assert(pArray != NULL);
}


template <class Type>
CyclicArray<Type>::~CyclicArray()
{
   delete pArray;
}


template <class Type>
int CyclicArray<Type>::is_empty(void)
{
   return nElems == 0;
}

template <class Type>
int CyclicArray<Type>::is_full(void)
{
   return nElems == nSize;
}

template <class Type>
int CyclicArray<Type>::size(void)
{
   return nSize;
}


template <class Type>
int CyclicArray<Type>::used(void)
{
   return nElems;
}


template <class Type>
Type * CyclicArray<Type>::insert(void)
{
   assert(!is_full());

   return &pArray[(nIdx + nElems++) % nSize];
}


template <class Type>
Type * CyclicArray<Type>::reference(void)
{
   assert(!is_empty());

   return &pArray[nIdx];
}


template <class Type>
void CyclicArray<Type>::remove(void)
{
   assert(!is_empty());

   nIdx = (nIdx + 1) % nSize;
   nElems--;
}

#endif
