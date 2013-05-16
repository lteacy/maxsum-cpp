/**
 * @file DomainIterator.h
 * This file defines the maxsum::DomainIterator class.
 * The maxsum::DomainIterator class provides methods for iterating over the
 * Cartesian product for a set of variable domains.
 */
#ifndef MAXSUM_DOMAIN_ITERATOR_H
#define MAXSUM_DOMAIN_ITERATOR_H

#include <algorithm>
#include "StaticVector.h"
#include "common.h"
#include "register.h"

namespace maxsum
{
   // This declaration is required for constructor declaration.
   // Can't simply include DiscreteFunction.h due to circular reference.
   class DiscreteFunction; 

   /**
    * This class provides methods for iterating over the Cartesian product for
    * a set of variable domains.
    */
   class DomainIterator
   {
   public: 

      /**
       * Type of list returned by DomainIterator::getVars()
       */
      typedef StaticVector<VarID> VarList;

      /**
       * Type of list returned by DomainIterator::getSubInd()
       */
      typedef StaticVector<ValIndex> IndList;

      /**
       * Utility method that throws an exception if we try to access beyond the
       * end of the domain.
       * @throws maxsum::OutOfRangeException if we've reached the end of the
       * list.
       */
      void validateRange() const
      {
         if(finished_i)
         {
            throw OutOfRangeException("DomainIterator",
                  "Iterator is out of range.");
         }
      }

   private:

      /**
       * Domain variables to interator over.
       */
      VarList vars_i;

      /**
       * Current sub indices for this domain.
       */
      IndList subInd_i;

      /**
       * Current linear index for this domain.
       */
      ValIndex ind_i;

      /**
       * Set of flags indicating which variables to iterate.
       * If fixed_i[k] is true, then DomainIterator::operator++ will not change
       * the value of subInd_i[k]
       */
      StaticVector<bool> fixed_i;

      /**
       * Cache of domain sizes for each variable.
       */
      IndList sizes_i;

      /**
       * This flag is set to finished when we have iterated over the last
       * element.
       */
      bool finished_i;

   public:

      /**
       * Default Constructor.
       * Creates Iterator over empty domain. Unless DomainIterator::addVars()
       * is subsequently called DomainIterator::getVars() and
       * DomainIterator::getSubInd() will return empty lists, and
       * DomainIterator::operator++() will throw an exception.
       */
      DomainIterator() : vars_i(), subInd_i(), ind_i(0), fixed_i(),
                         sizes_i(), finished_i(false) {}

      /**
       * Construct Domain iterator with initial list of variables.
       * @tparam type of list iterator used for arguments
       * @param begin iterator to first variable in list
       * @param end iterator to end of variable list
       */
      template<class It> DomainIterator(It begin, It end)
         : vars_i(begin,end),        // copy variables
           subInd_i(end-begin,0),    // set all subindices to zero
           ind_i(0),                 // set linear index to zero
           fixed_i(end-begin,false), // all variables are initially free
           sizes_i(end-begin,0),     // allocate space for variable sizes
           finished_i(false)         // iterator is not done yet
      {
         //*********************************************************************
         // Ensure that the variable list is sorted
         //*********************************************************************
         std::sort(vars_i.begin(),vars_i.end());

         //*********************************************************************
         // Most of the work here is done by the initialiser list.
         // All we have to do now is populate the size array.
         //*********************************************************************
         for(int k=0; k<vars_i.size(); ++k)
         {
            sizes_i[k] = getDomainSize(vars_i[k]);
         }

      } // constructor

      /**
       * Construct Domain Iterator using domain of a given function.
       * This is more efficient that creating from sratch.
       * @param[in] fun Function whose domain should be copied.
       */
      DomainIterator(const DiscreteFunction& fun);

      /**
       * Copy constructor.
       */
      DomainIterator(const DomainIterator& it);

      /**
       * Copy assignment.
       */
      DomainIterator& operator=(const DomainIterator& it);

      /**
       * Returns true if next call to DomainIterator::operator++() will not
       * throw an exception.
       * This is true provided all free (unconditioned) variables are less than
       * their corresponding maximum: maxsum::getDomainSize(maxsum::VarID) - 1.
       * @see maxsum::getDomainSize(maxsum::VarID)
       * @see DomainIterator::condition()
       */
      bool hasNext() const { return !finished_i; }

      /**
       * Accessor method for current sub indices.
       * @throws maxsum::OutOfRangeException if iterator is out of range.
       */
      const IndList& getSubInd() const
      {
         validateRange();
         return subInd_i;
      }
      
      /**
       * Accessor method for linear index.
       * @throws maxsum::OutOfRangeException if iterator is out of range.
       */
      ValIndex getInd() const
      {
         validateRange();
         return ind_i;
      }

      /**
       * Accessor method for variables in domain.
       */
      const VarList& getVars() const { return vars_i; }

      /**
       * Returns true if this function is conditioned on the specified
       * variable.
       */
      bool isFixed(VarID var) const;

      /**
       * Returns the number of conditioned variables for this function.
       */
      int fixedCount() const;

      /**
       * Add the specified variables to the domain of this iterator.
       * @tparam It iterator type for input variables.
       * @param begin iterator to start of variable list to add.
       * @param end iterator to end of variable list to add.
       * @post DomainIterator::getVars() will return set union of old variable
       * set and input variable set.
       * @post DomainIterator::hasNext() will return true.
       * @post Existing conditioned variables will remain conditioned and will
       * have their values preserved. All other variables will have subindex 0.
       */
      template<class It> void addVars(It begin, It end)
      {
         //*********************************************************************
         // Construct the new variable list, ensuring that it is sorted and
         // does not contain duplicates.
         //*********************************************************************
         StaticVector<VarID> newVars = vars_i;
         int maxSize = (end - begin) + vars_i.size();
         newVars.reserve(maxSize);
         newVars.insert(newVars.end(),begin,end);
         std::sort(newVars.begin(),newVars.end());

         StaticVector<VarID>::iterator newEnd =
            std::unique(newVars.begin(),newVars.end());

         newVars.resize(newEnd-newVars.begin());

         //*********************************************************************
         // Expand the indice, fixed and size arrays with initial values for
         // all new variables.
         //*********************************************************************
         int pos = 0; // position in old variable list
         StaticVector<ValIndex> newSizes(newVars.size());
         StaticVector<ValIndex> newSubInd(newVars.size());
         StaticVector<bool> newFixed(newVars.size());

         for(int k=0; k<newVars.size(); k++)
         {
            //******************************************************************
            // If the current variable is already in the domain, then
            // copy its original state.
            //******************************************************************
            if( (pos<vars_i.size()) && (vars_i[pos] == newVars[k]) )
            {
               newSizes[k] = sizes_i[pos];
               newSubInd[k] = subInd_i[pos];
               newFixed[k] = fixed_i[pos];
               ++pos;
            }
            //******************************************************************
            // Otherwise give it a new initial state
            //******************************************************************
            else
            {
               newSizes[k] = getDomainSize(newVars[k]);
               newSubInd[k] = 0;
               newFixed[k] = false;
            }

         } // for loop

         //*********************************************************************
         // Set the linear index to its appropriate value based on the
         // updated subindices and sizes.
         //*********************************************************************
         ind_i = sub2ind( sizes_i.begin(),  sizes_i.end(),
                         subInd_i.begin(), subInd_i.end() );

         //*********************************************************************
         // Since we've returned to the first element in the free part of the
         // domain, DomainIterator::hasNext() should return true.
         //*********************************************************************
         finished_i = false;

         //*********************************************************************
         // Finally, we swap in the new variable list.
         //*********************************************************************
         vars_i.swap(newVars);
         sizes_i.swap(newSizes);
         subInd_i.swap(newSubInd);
         fixed_i.swap(newFixed);

      } // function addVars

      /**
       * Increment this iterator to the next element in the domain.
       * This is the prefix version: increment first, then return.
       * @throws OutOfRangeException if we are already at the last element.
       * @returns iterator to next element
       */
      DomainIterator& operator++();

      /**
       * Increment this iterator to the next element in the domain.
       * This is the postfix version: return current value, then increment.
       * @throws OutOfRangeException if we are already at the last element.
       * @returns iterator to next element
       */
      DomainIterator operator++(int)
      {
         DomainIterator copy(*this);
         ++(*this);
         return copy;
      }

      /**
       * Condition domain on specified variable values.
       * Changes this iterator so that the specified set of variables have fixed
       * values, and are not incremented. The parameters indBegin and indEnd
       * specify the list of values that the corresponding variables should be
       * set to instead. Any variables pointed to by varBegin and varEnd that
       * are not it this iterators domain will be ignored.
       * @pre varBegin and varEnd are iterators to a \em sorted list of
       * maxsum::VarID.
       * @pre Values in list specified by indBegin and indEnd must be between
       * 0, and their respective variable domain size.
       * @post All free indices are set back to 0, and
       * DomainIterator::hasNext() returns true.
       * @post Previously conditioned variables have their values perserved.
       * @throws OutOfRangeException is specified condition values are out
       * of range.
       * @param[in] varBegin iterator to start of variable list.
       * @param[in] varEnd iterator to end of variable list.
       * @param[in] indBegin iterator to start of value list.
       * @param[in] indEnd iterator to end of value list.
       */
      template<class VarIt, class IndIt> void condition
      (
       const VarIt varBegin,
       const VarIt varEnd,
       const IndIt indBegin,
       const IndIt indEnd
      )
      {
         //*********************************************************************
         // Set the specified conditions for any variables that are in
         // this domain.
         //*********************************************************************
         VarIt pInVar = varBegin;
         IndIt pInd = indBegin;
         while( (varEnd != pInVar) && (indEnd != pInd) )
         {
            //******************************************************************
            // If the current variable is in our domain, condition its value
            // as specified.
            //******************************************************************
            StaticVector<VarID>::const_iterator pMyVar =
               std::find(vars_i.begin(),vars_i.end(),*pInVar);

            if(vars_i.end() != pMyVar)
            {
               int position = pMyVar - vars_i.begin();
                fixed_i[position] = true;
               subInd_i[position] = *pInd;
            }

            //******************************************************************
            // Move to next variable in argument list
            //******************************************************************
            ++pInVar;
            ++pInd;

         } // loop

         //*********************************************************************
         // Set free indices back to zero. (This is a much easier policy to 
         // implement that trying to pick up were we left off.)
         //*********************************************************************
         for(int k=0; k<subInd_i.size(); ++k)
         {
            if(!fixed_i[k])
            {
               subInd_i[k]=0;
            }
         }

         //*********************************************************************
         // Since the free variables have been reset, the iterator is not
         // finished. Notice, even if there are now free variables, we
         // still intend to return the conditioned values at least once before
         // finishing.
         //*********************************************************************
         finished_i = false;

         //*********************************************************************
         // Set the linear index to its apropriate value, based on the
         // conditioned variables.
         //*********************************************************************
         ind_i = sub2ind( sizes_i.begin(),  sizes_i.end(),
                         subInd_i.begin(), subInd_i.end() );

      } // function condition

      /**
       * Condition domain on specified variable values.
       * Changes this iterator so that the specified set of variables have fixed
       * values, and are not incremented. 
       * @pre Values in vars map must be between 0, and their respective
       * variable domain size.
       * @post All free indices are set back to 0, and
       * DomainIterator::hasNext() returns true.
       * @post Previously conditioned variables have their values perserved.
       * @throws OutOfRangeException is specified condition values are out
       * of range.
       * @param[in] varBegin iterator to start of variable list.
       * @param[in] varEnd iterator to end of variable list.
       * @param[in] indBegin iterator to start of value list.
       * @param[in] indEnd iterator to end of value list.
       */
      template<class VarMap> void condition(const VarMap& vars)
      {
         //*********************************************************************
         // Set the specified conditions for any variables that are in
         // this domain.
         //*********************************************************************
         StaticVector<VarID>::const_iterator pMyVar = vars_i.begin();
         for(typename VarMap::const_iterator it=vars.begin();
               it!=vars.end(); ++it)
         {
            //******************************************************************
            // If the current variable is in our domain, condition its value
            // as specified.
            //******************************************************************
            pMyVar = find(vars_i.begin(),vars_i.end(),it->first);

            if(vars_i.end() != pMyVar)
            {
               int position = pMyVar - vars_i.begin();
                fixed_i[position] = true;
               subInd_i[position] = it->second;
            }
            
         } // loop

         //*********************************************************************
         // Set free indices back to zero. (This is a much easier policy to 
         // implement that trying to pick up were we left off.)
         //*********************************************************************
         for(int k=0; k<subInd_i.size(); ++k)
         {
            if(!fixed_i[k])
            {
               subInd_i[k]=0;
            }
         }

         //*********************************************************************
         // Since the free variables have been reset, the iterator is not
         // finished. Notice, even if there are now free variables, we
         // still intend to return the conditioned values at least once before
         // finishing.
         //*********************************************************************
         finished_i = false;

         //*********************************************************************
         // Set the linear index to its apropriate value, based on the
         // conditioned variables.
         //*********************************************************************
         ind_i = sub2ind( sizes_i.begin(),  sizes_i.end(),
                         subInd_i.begin(), subInd_i.end() );

      } // function condition

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
      void condition(const DomainIterator& it);

   }; // class DomainIterator

} // namespace maxsum


#endif // MAXSUM_DOMAIN_ITERATOR_H
