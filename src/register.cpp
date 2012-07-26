/**
 * @file register.cpp
 * Implementation of functions in register.h
 * @see register.h
 * @author Luke Teacy
 */

#include <map>
#include <string>
#include <sstream>
#include "register.h"

/**
 * Private Module Namespace.
 */
namespace
{

   /**
    * Hash map used to store registered variables are their domain size.
    */
   std::map<maxsum::VarID,maxsum::ValIndex> varRegister_m;


} // private namespace

/**
 * Returns true if the specified variable is registered.
 * @returns true if the specified variable is registered.
 */
bool maxsum::isRegistered(VarID var)
{
   return varRegister_m.end() != varRegister_m.find(var);
}

/**
 * Returns the domain size for a specified variable.
 * @param var id of the variable to search for.
 * @returns domain size of var
 * @throws UnknownVariableException if variable is not registered.
 */
maxsum::ValIndex maxsum::getDomainSize(VarID var)
{
   using namespace maxsum;
   
   //***************************************************************************
   // Lookup var in the variable register
   //***************************************************************************
   typedef std::map<VarID,ValIndex>::const_iterator It;
   It loc = varRegister_m.find(var);

   //***************************************************************************
   // If found, return its size.
   //***************************************************************************
   if(varRegister_m.end()!=loc)
   {
      return loc->second;
   }

   //***************************************************************************
   // Otherwise, if this variable is not found, return -1 to indicate that
   // the variable is not yet registered.
   //***************************************************************************
   std::stringstream msg;
   msg << "Attempt to get domain size for unregistered variable: " <<  var;
   throw UnknownVariableException("maxsum::getDomainSize",msg.str());

} // function getDomainSize

/**
 * Returns the number of currently registered variables.
 * @returns the number of currently registered variables.
 */
int maxsum::getNumOfRegisteredVariables()
{
   return varRegister_m.size();
}

/**
 * Registers a variable with a specified domain size.
 * Put the specified variable in a global register, and stores its domain
 * size. Variables can be registered multiple times, but their domain size
 * must never change.
 * @throws InconsistentDomainException if this variable is already
 * registered, but with a different domain size.
 * @param var the unique id of this variable
 * @param siz the domain size of this variable
 */
void maxsum::registerVariable(VarID var, ValIndex siz)
{
   using namespace maxsum;

   // function name used for generating errors
   const char* const functionName = "maxsum::registerVariable";

   //***************************************************************************
   // Ensure that domain size is valid. In particular, we don't allow
   // empty or singleton domains - since these are meaningless.
   //***************************************************************************
   if(2>siz)
   {
         std::stringstream msg;

         msg << "Tried to register variable " << var << " with invalid domain "
             << "size of " << siz;

         throw OutOfRangeException(functionName,msg.str());
   }
   
   //***************************************************************************
   // Lookup var in the variable register
   //***************************************************************************
   typedef std::map<VarID,ValIndex>::const_iterator It;
   It loc = varRegister_m.find(var);

   //***************************************************************************
   // If found, check that its size is consistent. Otherwise throw an
   // exception.
   //***************************************************************************
   if(varRegister_m.end()!=loc)
   {
      if(loc->second != siz)
      {
         std::stringstream msg;

         msg << "Tried to register variable " << var << " again with "
             << "inconsistent domain size.";

         throw InconsistentDomainException(functionName,msg.str());
      }
   }

   //***************************************************************************
   // Otherwise, if this variable is not found, return -1 to indicate that
   // the variable is not yet registered.
   //***************************************************************************
   varRegister_m[var] = siz;

} // function registerVariable


