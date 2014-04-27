/** 
 * @file maxsum_c.cpp
 * Provides C compatible interface to maxsum C++ library
 * 
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include "maxsum.h"
#include "maxsum_c.h"

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
 * @returns domain size of var, or -1 if variable is unknown.
 */
maxsum::ValIndex getDomainSize_ms(maxsum::VarID var)
{
   try
   {
      return maxsum::getDomainSize(var);
   }
   catch(maxsum::UnknownVariableException& e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
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
 * @returns -1 if this variable is already registered, but with a different
 * domain size.
 * @param var the unique id of this variable
 * @param siz the domain size of this variable
 * @see maxsum::registerVariables
 */
int registerVariable_ms(maxsum::VarID var, maxsum::ValIndex siz)
{
   try
   {
      maxsum::registerVariable(var,siz);
      return 0;
   }
   catch(maxsum::InconsistentDomainException& e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
}

/**
 * C implementation of matlab ind2sub function.
 * @param idx the scalar linear index
 * @param nDims length of dimension size vector
 * @param pSiz input vector containing dimension sizes in order
 * @param pSub output vector in which to store sub indices
 * @return -1 on error, 0 otherwise.
 */
int ind2sub_ms
(
 const int idx,
 const int nDims,
 const int* const pSiz,
 int* const pSub
)
{
   if(0>=nDims)
   {
      return -1;
   }

   try
   {
      // create vectors to hold input and output
      std::vector<int> sizVec(nDims);
      std::vector<int> subVec(nDims);

      // copy input into vector
      std::copy(pSiz,pSiz+nDims,sizVec.begin());

      // call C++ version to do the hard work
      maxsum::ind2sub(sizVec, idx, subVec);

      // copy output to parent function
      std::copy(subVec.begin(),subVec.end(),pSub);

      // indicate that all went well
      return 0;

   }
   catch(maxsum::OutOfRangeException& e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
}

/**
 * C implementation of the matlab sub2ind function.
 * @param nDims length of dimension size vector
 * @param pSiz input vector containing dimension sizes in order
 * @param pSub input vector in which to store sub indices
 * @return -1 on error, or scalar linear index otherwise.
 */
int sub2ind_ms(const int nDims, const int* const pSiz, int* const pSub)
{
   if(0>=nDims)
   {
      return -1;
   }

   try
   {
      return maxsum::sub2ind(pSiz, pSiz+nDims, pSub, pSub+nDims);
   }
   catch(maxsum::OutOfRangeException& e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
}

/**
 * Create a new maxsum controller.
 */
void* newMaxSumController_ms
(
  int maxIterations,
  ValType maxnorm
)
{
   return new maxsum::MaxSumController(maxIterations,maxnorm);
}

/**
 * Delete maxsum controller.
 */
void deleteMaxSumController_ms
(
  void* pController
)
{
   delete static_cast<MaxSumController*>(pController);
}

/**
 * Set/Add factor to factor graph
 * returns -1 on error
 */
int setFactor_ms
(
 void* pController,
 const FactorID id,
 const int nVars,
 const VarID* const pDomain,
 const ValType* pData
)
{
   try
   {
      //***********************************************************************
      // Construct discrete function from variables
      //***********************************************************************
      maxsum::DiscreteFunction factor(pDomain,pDomain+nVars);

      //***********************************************************************
      // Copy data into function
      //***********************************************************************
      for(int k=0; k<factor.domainSize(); ++k)
      {
         factor(k) = *(pData++);
      }
   
      //***********************************************************************
      // set the factor
      //***********************************************************************
      static_cast<maxsum::MaxSumController*>(pController)->setFactor(id,factor);

   }
   // return -1 on error
   catch(maxsum::UnknownVariableException& e)
   {
      return -1;
   }

   return 0;
}

/**
 * Remove factor from factor graph.
 */
void removeFactor_ms(void* pController, FactorID id)
{
   static_cast<maxsum::MaxSumController*>(pController)->removeFactor(id);
}

/**
 * Remove all factors.
 */
void clearAll_ms(void* pController)
{
   static_cast<maxsum::MaxSumController*>(pController)->clear();
}

/**
 * Report number of factors in factor graph.
 */
int noFactors_ms(void* pController)
{
   return static_cast<maxsum::MaxSumController*>(pController)->noFactors();
}

/**
 * Report number of variables in factor graph.
 */
int noVars_ms(void* pController)
{
   return static_cast<maxsum::MaxSumController*>(pController)->noVars();
}

/**
 * Run the max-sum algorithm on factor graph.
 * @returns number of iterations performed.
 */
int optimise_ms(void* pController)
{
   return static_cast<maxsum::MaxSumController*>(pController)->optimise();
}

/**
 * After calling optimise_ms, returns the optimal value for each
 * variable in the factor graph.
 * @param[in] pController pointer to the max-sum controller
 * @param[out] pVars ordered list of all variable ids in graph
 * @param[out] pVals correspond optimal values for each variable in list
 * @returns number of values written
 */
int getValues_ms
(
 void* pController,
 VarID* pVars,
 ValIndex* pVals
)
{
   maxsum::MaxSumController* pTypedController =
      static_cast<maxsum::MaxSumController*>(pController);

   maxsum::MaxSumController::ConstValueIterator pCur = pTypedController->valBegin();
   maxsum::MaxSumController::ConstValueIterator pEnd = pTypedController->valEnd();

   int count = 0;

   while(pCur != pEnd)
   {
      (*pVars) = pCur->first;
      (*pVals) = pCur->second;

      ++count;
      ++pVars;
      ++pVals;
      ++pCur;
   }

   return count;
}


