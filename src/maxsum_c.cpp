/** 
 * @file maxsum_c.cpp
 * Provides C compatible interface to maxsum C++ library
 * 
 */

#include "maxsum_c.h"
#include "maxsum/common.h"
#include "maxsum/register.h"

/**
 * Returns true if the specified variable is registered.
 * @param var id of the variable to search for.
 * @returns true if the specified variable is registered.
 */ 
int isRegistered_ms(maxsum::VarID var)
{
   if(maxsum::isRegistered(var))
   {
      return 1;
   }
   return 0;
}

/**
 * Returns the registered domain size for a specified variable.
 * @param var id of the variable to search for.
 * @returns domain size of var
 * @throws UnknownVariableException if the variable is not registered.
 */
maxsum::ValIndex getDomainSize_ms(maxsum::VarID var)
{
   return maxsum::getDomainSize(var);
}

/**
 * Returns the number of currently registered variables.
 * @returns the number of currently registered variables.
 */
int getNumOfRegisteredVariables_ms()
{
   return maxsum::getNumOfRegisteredVariables();
}

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
void registerVariable_ms(maxsum::VarID var, maxsum::ValIndex siz)
{
   return maxsum::registerVariable(var,siz);
}

