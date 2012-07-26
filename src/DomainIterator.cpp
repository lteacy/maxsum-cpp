/**
 * @file DomainIterator.cpp
 * This file implments the maxsum::DomainIterator class.
 * The maxsum::DomainIterator class provides methods for iterating over the
 * Cartesian product for a set of variable domains.
 */
#include "register.h"
#include "DomainIterator.h"
#include "DiscreteFunction.h"

using namespace maxsum;

/**
 * Convenience function for conditioning one iterator on the current
 * value of another. This equalivalent to the following code
 * <pre>
 * DomainIterator it1(...), it2(...);
 * DomainIterator::VarList vars = it1.getVars();
 * DomainIterator::IndList inds = it1.getSubInd();
 * it2.condition(vars.begin(),vars.end(),inds.begin(), inds.end());
 * </pre>
 * @see DomainIterator::condition(const VarIt,const VarIt,const IndIt,const IndIt)
 */
void DomainIterator::condition(const DomainIterator& it)
{
   VarList vars = it.getVars();
   IndList inds = it.getSubInd();
   condition(vars.begin(),vars.end(),inds.begin(), inds.end());
}

/**
 * Returns true if this function is conditioned on the specified
 * variable.
 */
bool DomainIterator::isFixed(VarID var) const
{
   std::vector<VarID>::const_iterator pos = 
   find(vars_i.begin(),vars_i.end(),var);
   if(vars_i.end()==pos)
   {
      return false;
   }  
   return fixed_i[pos-vars_i.begin()];
}

/**
 * Returns the number of conditioned variables for this function.
 */
int DomainIterator::fixedCount() const
{
   int count = 0;
   for(int k=0; k<fixed_i.size(); ++k)
   {
      if(fixed_i[k])
      {
         ++count;
      }
   }
   return count;
}

/**
 * Construct Domain Iterator using domain of a given function.
 * This is more efficient that creating from sratch.
 * @param[in] fun Function whose domain should be copied.
 */
DomainIterator::DomainIterator(const DiscreteFunction& fun)
   : vars_i(fun.varBegin(),fun.varEnd()),    // copy variables
     subInd_i(fun.noVars(),0),               // set all subindices to zero
     ind_i(0),                               // set linear index to zero
     fixed_i(fun.noVars(),false),            // all variables are initially free
     sizes_i(fun.sizeBegin(),fun.sizeEnd()), // copy variable sizes
     finished_i(false)                       // iterator is not done yet
   {}                   // nothing left to do in constructor body

/**
 * Copy constructor.
 */
DomainIterator::DomainIterator(const DomainIterator& it)
   : vars_i(it.vars_i.begin(),it.vars_i.end()),       // copy everything
     subInd_i(it.subInd_i.begin(),it.subInd_i.end()),
     ind_i(it.ind_i),
     fixed_i(it.fixed_i.begin(),it.fixed_i.end()),
     sizes_i(it.sizes_i.begin(),it.sizes_i.end()),
     finished_i(it.finished_i)
   {}                   // nothing left to do in constructor body

/**
 * Copy assignment.
 */
DomainIterator& DomainIterator::operator=(const DomainIterator& it)
{
   vars_i = it.vars_i;
   subInd_i = it.subInd_i;
   ind_i = it.ind_i;
   fixed_i = it.fixed_i;
   sizes_i = it.sizes_i;
   finished_i = it.finished_i;
   return *this;

} // operator=

/**
 * Increment this iterator to the next element in the domain.
 * This is the prefix version: increment first, then return.
 * @throws maxsum::OutOfRangeException if we are already at the last element.
 * @returns iterator to next element
 */
DomainIterator& DomainIterator::operator++()
{
   //***************************************************************************
   // Until proven otherwise, we assume that, by incrementing this iterator,
   // we will run out of elements.
   //***************************************************************************
   finished_i = true;

   //***************************************************************************
   // Iterator through all subindices least significant first order.
   // As in Matlab, we consider the first index to be least significant,
   // and the last to be most significant.
   //***************************************************************************
   for(int k=0; k<vars_i.size(); ++k)
   {
      //************************************************************************
      // Ignore conditioned variables
      //************************************************************************
      if(fixed_i[k])
      {
         continue;
      }

      //************************************************************************
      // Increment current variable to its next value.
      //************************************************************************
      subInd_i[k] = (subInd_i[k]+1) % sizes_i[k];

      //************************************************************************
      // If the new value is non-zero, then we've completed this incremenet,
      // and we've still got values yet to visit.
      //************************************************************************
      if(0 < subInd_i[k])
      {
         finished_i = false;
         break;
      }

   } // for loop

   //***************************************************************************
   // Update linear index to be consistent with current subindices.
   //***************************************************************************
   ind_i = sub2ind( sizes_i.begin(),  sizes_i.end(),
                   subInd_i.begin(), subInd_i.end() );

   //***************************************************************************
   // Return reference to this iterator
   //***************************************************************************
   return *this;

} // prefix ++


