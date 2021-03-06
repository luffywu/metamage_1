// Nucleus/ReferenceTraits.h
// -------------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_REFERENCETRAITS_H
#define NUCLEUS_REFERENCETRAITS_H

namespace Nucleus
  {
   template < class T > struct ReferenceTraits;
   
   template < class T > struct ReferenceTraits< T& >
     {
      typedef T Value;
      typedef T *Pointer;
      typedef T& Reference;
      typedef const T *ConstPointer;
      typedef const T& ConstReference;
     };
   
   template < class T > struct ReferenceTraits< const T& >
     {
      typedef T Value;
      typedef const T *Pointer;
      typedef const T& Reference;
      typedef const T *ConstPointer;
      typedef const T& ConstReference;
     };
   
   template < class T > struct ReferenceTraits< volatile T& >
     {
      typedef T Value;
      typedef volatile T *Pointer;
      typedef volatile T& Reference;
      typedef const volatile T *ConstPointer;
      typedef const volatile T& ConstReference;
     };
   
   template < class T > struct ReferenceTraits< const volatile T& >
     {
      typedef T Value;
      typedef const volatile T *Pointer;
      typedef const volatile T& Reference;
      typedef const volatile T *ConstPointer;
      typedef const volatile T& ConstReference;
     };
  }

#endif
