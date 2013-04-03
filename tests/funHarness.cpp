
#include <vector>
#include <iostream>
#include "maxsum/DiscreteFunction.h"

int comparisionTestOne()
{

   using namespace maxsum;

   int exitValue = 0; // return sucess unless something fails.

   //***************************************************************************
   // Register some variables for test purposes
   //***************************************************************************
   VarID vars[] = {1,2,3};
   ValIndex sizes[] = {15,10,5};
   registerVariables(vars,vars+3,sizes,sizes+3);

   //***************************************************************************
   // Create a couple of larger functions
   //***************************************************************************
   DiscreteFunction a, b(10), c(1.2);
   VarID xDomain[] = {1,2};
   VarID yDomain[] = {2,3};
   VarID zDomain[] = {3};
   DiscreteFunction x(xDomain,xDomain+2,29.1), y(yDomain,yDomain+2,2.5);
   DiscreteFunction z(zDomain,zDomain+1,123);

   //***************************************************************************
   // Create a list of all test objects, so that we can iterator over all
   // objects when necessary.
   //***************************************************************************
   std::vector<DiscreteFunction > allFuncs;
   allFuncs.reserve(6);
   allFuncs.push_back(a);
   allFuncs.push_back(b);
   allFuncs.push_back(c);
   allFuncs.push_back(x);
   allFuncs.push_back(y);
   allFuncs.push_back(z);

   //***************************************************************************
   // Check comparison operators are valid and consistent
   //***************************************************************************
   std::cout << "Testing comparison operators...";
   bool comparisonFailed = false;
   for(int k=0; k<allFuncs.size(); k++)
   {
      for(int j=0; j<allFuncs.size(); j++)
      {
         if( (k==j) != (allFuncs[k]==allFuncs[j]) )
         {
            std::cout << std::endl << " (" << k << "==" << j << ") Failed.";
            exitValue = 1;
            comparisonFailed = true;
         }

         if( (k==j) == (allFuncs[k]!=allFuncs[j]) )
         {
            std::cout << std::endl << " (" << k << "!=" << j << ") Failed.";
            exitValue = 1;
            comparisonFailed = true;

         }
      }
   }

   //***************************************************************************
   // Check that functions with different domains can be equal, provided
   // that they are equal across the cartesian product of their domains
   //***************************************************************************
   DiscreteFunction r(9), s(xDomain,xDomain+2,9), t(xDomain,xDomain+1,9);

   bool   equalityCheck = (r==s) && (r==t) && (s==t);
   bool inEqualityCheck = (r!=s) || (r!=t) || (s!=t);

   if(!equalityCheck)
   {
      std::cout << "\nVariables not equal across their combined domain.";
      comparisonFailed = true;
      exitValue = 1;
   }

   if(inEqualityCheck)
   {
      std::cout << "\nVariables unequal across their combined domain.";
      comparisonFailed = true;
      exitValue = 1;
   }

   if(!comparisonFailed)
   {
      std::cout << "OK" << std::endl;
   }
   std::cout << std::endl;

   return exitValue;;
   
} // comparision test 1

int comparisionTestTwo()
{
   using namespace maxsum;

   //***************************************************************************
   // Register some variables for test purposes
   //***************************************************************************
   VarID vars[] = {101,102,103};
   ValIndex sizes[] = {15,10,5};
   registerVariables(vars,vars+3,sizes,sizes+3);

   //***************************************************************************
   // Create some scalar functions
   //***************************************************************************
   DiscreteFunction a(0), b(2), c(3);

   //***************************************************************************
   // Create some functions that depend on variables
   //***************************************************************************
   VarID vars2[] = {101,103};
   DiscreteFunction d(vars+0,vars+1), e(vars+0,vars+2), f(vars+0,vars+3);
   DiscreteFunction x(vars2,vars2+2), y(vars+1,vars+2), z(vars+2,vars+3);
   
   //***************************************************************************
   // Stick functions in a list for easy manipulation
   //***************************************************************************
   std::vector<DiscreteFunction > allFuncs;
   allFuncs.push_back(a); // 0
   allFuncs.push_back(b); // 1
   allFuncs.push_back(c); // 2
   allFuncs.push_back(d); // 3
   allFuncs.push_back(e); // 4
   allFuncs.push_back(f); // 5
   allFuncs.push_back(x); // 6
   allFuncs.push_back(y); // 7
   allFuncs.push_back(z); // 8

   //***************************************************************************
   // Initialise values of non-scalar functions
   //***************************************************************************
   const double SCALE = 1.2; // arbitary scale factor for testing purposes
   for(int k=3; k<allFuncs.size(); k++)
   {
      DiscreteFunction& curFun = allFuncs[k];
      for(int j=0; j<curFun.domainSize(); j++)
      {
         curFun(j) = static_cast<double>(j) * SCALE;
      }
   }

   //***************************************************************************
   // Check comparison operators are valid and consistent
   //***************************************************************************
   std::cout << "Testing comparison operators...";
   int exitValue = 0;
   bool comparisonFailed = false;
   for(int k=0; k<allFuncs.size(); k++)
   {
      for(int j=0; j<allFuncs.size(); j++)
      {
         if( (k==j) != (allFuncs[k]==allFuncs[j]) )
         {
            std::cout << std::endl << " (" << k << "==" << j << ") Failed.";
            exitValue = 1;
            comparisonFailed = true;
         }

         if( (k==j) == (allFuncs[k]!=allFuncs[j]) )
         {
            std::cout << std::endl << " (" << k << "!=" << j << ") Failed.";
            exitValue = 1;
            comparisonFailed = true;

         }
      }
   }

   if(!comparisonFailed)
   {
      std::cout << "OK" << std::endl;
   }

   //***************************************************************************
   // Check for equality before and after changing a single coefficient
   //***************************************************************************
   DiscreteFunction t(x);

   std::cout << "\nChecking equality...";
   comparisonFailed = false;
   if(t!=x)
   {
      std::cout << "failed before change." << std::endl;
      comparisonFailed = true;
      exitValue = 1;
   }

   ValIndex midPoint = t.domainSize() / 2;
   t(midPoint) = 2.19841;

   if(t==x)
   {
      std::cout << "failed after change." << std::endl;
      comparisonFailed = true;
      exitValue = 1;
   }

   if(!comparisonFailed)
   {
      std::cout << "OK" << std::endl;
   }
   std::cout << std::endl;

   return exitValue;

} // comparision test 2

int main()
{
   std::cout << "******************************************\n";
   std::cout << "Comparision Test 1\n";
   std::cout << "******************************************\n";
   int exitStatus = comparisionTestOne();
   if(0!=exitStatus)
   {
      return exitStatus;
   }

   std::cout << "******************************************\n";
   std::cout << "Comparision Test 2\n";
   std::cout << "******************************************\n";
   exitStatus = comparisionTestTwo();
   if(0!=exitStatus)
   {
      return exitStatus;
   }

} // function main
