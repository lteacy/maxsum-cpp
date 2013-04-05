/** 
 * @file register.h
 * This Header defines functions for registering the set of all
 * variables on which maxsum::DiscreteFunction objects may depend.
 *  
 * Variables are uniquely identified by a key of type maxsum::VarID and each as
 * a specified domain size. The purpose of the functions defined in this file
 * are to register the domain size for each variable before it is used, and
 * ensure that this size remains fixed throughout a programs execution.
 * Variables can be registered multiple times, but in each case the domain size
 * must not change. Variables must always be registered before they are
 * referenced by any function.
 * 
 */
#ifndef MAX_SUM_REGISTER_H
#define MAX_SUM_REGISTER_H

#include "common.h"

namespace maxsum
{
   /**
    * Returns true if the specified variable is registered.
    * @param var id of the variable to search for.
    * @returns true if the specified variable is registered.
    */ 
   bool isRegistered(VarID var);

   /**
    * Returns true if all specified variables are registered.
    * Parameters are iterators over a list of variable ids of type
    * maxsum::VarID.
    * @param varBegin iterator to begining of variable list.
    * @param varEnd iterator to end of variable list.
    * @returns true if all registered, false otherwise.
    */
   template<class VarIt> bool allRegistered(VarIt varBegin, VarIt varEnd)
   {
      for(VarIt var=varBegin; var!=varEnd; var++)
      {
         if(!isRegistered(*var))
         {
            return false;
         }
      }
      return true;
   }

   /**
    * Returns the registered domain size for a specified variable.
    * @param var id of the variable to search for.
    * @returns domain size of var
    * @throws UnknownVariableException if the variable is not registered.
    */
   ValIndex getDomainSize(VarID var);

   /**
    * Returns the number of currently registered variables.
    * @returns the number of currently registered variables.
    */
   int getNumOfRegisteredVariables();

   /**
    * Registers a variable with a specified domain size.
    * Puts the specified variable in a global register, and stores its domain
    * size. Variables can be registered multiple times, but their domain size
    * must never change.
    * @throws InconsistentDomainException if this variable is already
    * registered, but with a different domain size.
    * @param var the unique id of this variable
    * @param siz the domain size of this variable
    * @see maxsum::registerVariables
    */
   void registerVariable(VarID var, ValIndex siz);

   /**
    * Register a list of variables with specified domain sizes.
    * This works in the same was as maxsum::registerVariable - but does so
    * for multiple variables at a time. The parameters varBegin and varEnd are
    * iterators to the beginning and end of a list of variable ids, while
    * sizBegin and sizEnd specify the start and end of a list of their
    * respective domain sizes. Both lists must be ordered such that
    * the kth element of the size list is the domain size for the kth variable
    * in the variable list.
    * @param varBegin iterator to the start of a list of maxsum::VarID
    * @param varEnd iterator to the end of a list of maxsum::VarID
    * @param sizBegin iterator to the start of a list of maxsum::ValIndex
    * @param sizEnd iterator to the start of a list of maxsum::ValIndex
    * @see maxsum::registerVariable
    */
   template<class VarIt, class IndIt> void registerVariables
      (VarIt varBegin, VarIt varEnd, IndIt sizBegin, IndIt sizEnd)
   {
      //************************************************************************
      // Create iterators for the variable and domain size lists
      //************************************************************************
      VarIt var = varBegin;
      IndIt siz = sizBegin;

      //************************************************************************
      // Register each variable with its corresponding size
      //************************************************************************
      while( (var!=varEnd) && (siz!=sizEnd) )
      {
         registerVariable(*var,*siz);
         ++var;
         ++siz;
      }

   } // function registerVariables

} // namespace maxsum

#endif // MAX_SUM_REGISTER_H
