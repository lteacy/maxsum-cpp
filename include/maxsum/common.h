/**
 * @file common.h
 * Common types and functions used by Max-Sum library.
 * In particular, this file defines common typedefs, and functions for
 * calculating subindices and linear indices for referencing the contents
 * of N-D arrays. These resemble similar functions defined in the Matlab
 * environment.
 * @author Luke Teacy
 */
#ifndef MAX_SUM_COMMON_H
#define MAX_SUM_COMMON_H

#include <cfloat>
#include <string>
#include <vector>
#include "exceptions.h"
#include "types.h"

/**
 * Namespace for all public types and functions defined by the Max-Sum library.
 */
namespace maxsum
{
   /**
    * Default tolerance used for comparing values of type maxsum::ValType.
    * This is the default value used by the maxsum::equalWithinTolerance
    * function, when comparing to maxsum::DiscreteFunction objects for equality.
    * Note, if ValType is ever redefined, then this value should be changed
    * appropriately also.
    * @see maxsum::equalWithinTolerance
    */
   const ValType DEFAULT_VALUE_TOLERANCE = DBL_EPSILON * 1000.0;

   /**
    * C++ Implementation of Matlab ind2sub function.
    * Main difference here is that indices start from 0. In the special case
    * where siz is empty, ind will also be empty.
    * @param[in] siz of N-D array
    * @param[in] ind linear index
    * @param[out] sub vector in which we will put the sub indices.
    * @post previous contents of sub will be overwritten.
    */
   template<class VecType> void ind2sub
   (
    const VecType& siz,
    const typename VecType::value_type ind,
    VecType& sub
   )
   {
      //************************************************************************
      // Ensure subindex vector has correct size
      //************************************************************************
      const int ndims = siz.size();
      sub.resize(ndims);

      //************************************************************************
      // At this stage, if the number of dimensions is zero, then we have
      // and empty set of subindices. This is correct, we we're done.
      //************************************************************************
      if(0==ndims)
      {
         return;
      }

      //************************************************************************
      // Calculate the cumulative product of the size vector
      //************************************************************************
      typedef typename VecType::value_type IntType;
      std::vector<IntType> cumProd(ndims);
      IntType nextProd = 1;
      for(int k=0; k<ndims; k++)
      {
         cumProd[k] = nextProd;
         nextProd *= siz[k];
      }

      //************************************************************************
      // Validate range of linear index
      //************************************************************************
      int maxInd = cumProd[ndims-1] * siz[ndims-1] - 1;
      if( (ind<0) || (ind > maxInd) )
      {
         throw OutOfRangeException("bad linear index","maxsum::ind2sub");
      }

      //************************************************************************
      // Calculate the subindices. This is done by repeatedly dividing by
      // the cumulative dimension sizes, and recording the remainder.
      // Have to start at the largest dimension and work backwards.
      //************************************************************************
      IntType remainder = ind;
      for(int k=ndims-1; k>=0; k--)
      {
         sub[k] = remainder / cumProd[k];
         remainder = remainder % cumProd[k];
      }
      
   } // ind2sub

   /**
    * C++ Implementation of Matlab sub2ind function.
    * Main difference here is that indices start from 0.
    * @param[in] sizFirst iterator to first element of size array.
    * @param[in] sizEnd iterator to end element of size array.
    * @param[in] subFirst iterator to first element of subindex array.
    * @param[in] subEnd iterator to first element of subindex array.
    * @returns linear index
    */
   template<class SizIt, class SubIt> ValIndex
      sub2ind(SizIt sizFirst, SizIt sizEnd, SubIt subFirst, SubIt subEnd)
   {
      ValIndex skipSize = 1;
      ValIndex result = 0;

      SizIt siz = sizFirst;
      SubIt sub = subFirst;

      while( (siz!=sizEnd) && (sub!=subEnd) )
      {
         if( (0>*sub) || (*siz<=*sub) )
         {
            throw OutOfRangeException("bad subindex","maxsum::sub2ind");
         }
         result += (*sub) * skipSize;
         skipSize *= *siz;

         ++siz;
         ++sub;
      }

      return result;

   } // sub2ind

   /**
    * C++ Implementation of Matlab sub2ind function.
    * Main difference here is that indices start from 0.
    * @param[in] siz of N-D array
    * @param[in] sub empty vector in which we will put the sub indices.
    * @returns linear index
    */
   template<class VecType> typename VecType::value_type
      sub2ind(const VecType& siz, const VecType& sub)
   {
      return sub2ind(siz.begin(),siz.end(),sub.begin(),sub.end());
   }

} // namespace maxsum

#endif // MAX_SUM_COMMON_H
