/**
 * @file maxsumHarness.cpp
 * Test harness for MaxSumController.
 * Tests MaxSumController on a few simple graph colouring problems.
 */

#include "MaxSumController.h"
#include<iostream>
#include <ctime>
#include <iomanip>
#include <set>
using namespace maxsum;

/**
 * Scale factor used to divid random numbers in [0,1] to generate
 * a small random utility bias.
 */
const double BIAS_SCALE_M = 10000;

/**
 * Enumeration that represents the colours used in the graph colouring
 * problems generated by this test harness.
 */
enum Colour_m
{
   RED = 0,
   GREEN = 1,
   BLUE = 2,
   NO_COLOURS = 3
};

/**
 * Convenience typedef for a map of factors.
 */
typedef std::map<FactorID,DiscreteFunction> FactorMap_m;

/**
 * Turns a function into a graph colouring utility function, based on its
 * domain. Each variable is functions domain is taken to be a colour variable.
 * The utility is defined as small random bias, minus the number of variables
 * that have the same colour.
 * @param factor the function that will be populated with the graph colour
 * utilities, based on its domain.
 */
void genColourUtil_m(DiscreteFunction& factor)
{
   //***************************************************************************
   // For each possible set of action values in the function's domain
   //***************************************************************************
   for(DomainIterator it(factor); it.hasNext(); ++it)
   {
      //************************************************************************
      // Count the number of conflicts (i.e. actions with the same value)
      //************************************************************************
      std::set<ValIndex> uniqueVal(it.getSubInd().begin(),it.getSubInd().end());
      double noConflicts = it.getSubInd().size();
      noConflicts -= uniqueVal.size();

      //************************************************************************
      // Set the utility for the current position to be some small random
      // bias - the number of conflicts
      //************************************************************************
      double divider = BIAS_SCALE_M * RAND_MAX;
      double util = static_cast<double>(std::rand()) / divider;
      util -= noConflicts;
      factor(it) = util;

   } // for loop

} // function genColourUtil

/**
 * Function that generates a ring factor graph with a specified number of
 * factors.
 * @param[in] noFactors number of factors to generate
 * @param[out] factors map in which to store the generated factors.
 * @post any previous contents of <code>factors</code> will be destroyed.
 */
void genRingGraph_m(int noFactors, FactorMap_m& factors)
{
} // function genRingGraph

/**
 * Function that generates a fully connected factor graph with a specified
 * number of factors.
 * @param[in] noFactors number of factors to generate
 * @param[out] factors map in which to store the generated factors.
 * @post any previous contents of <code>factors</code> will be destroyed.
 */
void genFullGraph_m(int noFactors, FactorMap_m& factors)
{
} // function genFullGraph

/**
 * Function that generates a tree graph with a specified depth and branching
 * factor.
 * @param[in] depth the depth of the tree
 * @param[in] branchFactor the branching factor of the tree
 * @param[out] factors map in which to store the generated factors.
 * @post any previous contents of <code>factors</code> will be destroyed.
 */
void genTreeGraph_m(int depth, int branchFactor, FactorMap_m& factors)
{

} // function genTreeGraph

/**
 * Validates that the factor graph associated with a MaxSumController is
 * consistent with a specified factor map.
 * @returns number of errors encountered
 */
int isConsistent_m
(
 const MaxSumController& controller,
 const FactorMap_m& factors
)
{
   std::set<VarID> vars; // set of all variables
   int noEdges=0; // total number of edges
   int errorCount=0;

   //***************************************************************************
   // For each factor
   //***************************************************************************
   for(FactorMap_m::const_iterator fIt=factors.begin();
         fIt!=factors.end(); ++fIt)
   {
      //************************************************************************
      // Ensure that the factor value matches the one recored by the controller.
      //************************************************************************
      if(fIt->second != controller.getFactor(fIt->first))
      {
         std::cout << "Value mismatch for factor: " << fIt->first << '\n';
         ++errorCount;
      }

      //************************************************************************
      // Ensure the controller has an edge between this factor and each of
      // the variables in its domain.
      //************************************************************************
      for(DiscreteFunction::VarIterator vIt=fIt->second.varBegin();
            vIt!=fIt->second.varEnd(); ++vIt)
      {
         //*********************************************************************
         // Ensure this edge exists
         //*********************************************************************
         if(!controller.hasEdge(fIt->first,*vIt))
         {
            std::cout << "Missing edge between factor " << fIt->first
               << " and variable " << *vIt << std::endl;
            ++errorCount;
            return errorCount;
         }
         ++noEdges;

         //*********************************************************************
         // Store the id of this variable for future reference
         //*********************************************************************
         vars.insert(*vIt);

      } // inner for loop (over variables)

   } // outer for loop (over factors)

   //***************************************************************************
   // Make sure that all counts are consistent 
   //***************************************************************************
   if(factors.size() != controller.noFactors())
   {
      std::cout << "Inconsistent number of factors reported: " <<
         controller.noFactors() << " should be " << factors.size() << '\n';
      ++errorCount;
   }

   if(vars.size() != controller.noVars())
   {
      std::cout << "Inconsistent number of variables reported: " <<
         controller.noVars() << " should be " << vars.size() << '\n';
      ++errorCount;
   }

   if(controller.noEdges() != noEdges)
   {
      std::cout << "Inconsistent number of edges reported: " <<
         controller.noEdges() << " should be " << noEdges << '\n';
      ++errorCount;
   }

   return errorCount;

} // function isConsistent

/**
 * Tests ability to remove or clear factors in various ways.
 */
int testRemoval_m(MaxSumController& controller, const FactorMap_m& factors)
{
   //***************************************************************************
   // Remove every other factor by key
   //***************************************************************************
   int errorCount=0;
   int originalSize = controller.noFactors();
   int noRemoved=0;
   int count=0;
   for(FactorMap_m::const_iterator it=factors.begin(); it!=factors.end(); ++it)
   {
      if(0==count%2)
      {
         controller.removeFactor(it->first);
         ++noRemoved;
      }

      if(controller.hasFactor(it->first))
      {
         std::cout << "Failed to remove factor by key.\n";
         ++errorCount;
      }
      ++count;
   }

   //***************************************************************************
   // Check we now have the correct number of factors
   //***************************************************************************
   if(controller.noFactors() != originalSize-noRemoved)
   {
      std::cout << "Wrong number of factors after removal: " << 
         controller.noFactors() << " should be " << (originalSize-noRemoved)
         << std::endl;
      ++errorCount;
   }

   //***************************************************************************
   // Now clear the remaining factors
   //***************************************************************************
   controller.clear();
   if(0!=controller.noFactors())
   {
      std::cout << "Nonzero factors after clear.\n";
      ++errorCount;
   }

   if(0==errorCount)
   {
      std::cout << "No errors encountered during removal.\n";
   }

   return errorCount;

} // testRemoval_m function

/**
 * Returns true if two variables are connected via at least one factor.
 */
bool areVarsConnected_m
(
 const MaxSumController& controller,
 VarID var1,
 VarID var2
)
{
   //***************************************************************************
   // For each factor
   //***************************************************************************
   for(MaxSumController::ConstFactorIterator it=controller.factorBegin();
         it!=controller.factorEnd(); ++it)
   {
      //************************************************************************
      // If the current factor is related to both variables, return true
      //************************************************************************
      if(it->second.dependsOn(var1) && it->second.dependsOn(var2))
      {
         return true;
      }
   }

   //***************************************************************************
   // If we get here, the variables are not related through any factor, so
   // we return false.
   //***************************************************************************
   return false;

} // function areVarsConnected_m

/**
 * Counts the number of colour conflicts, given the current actions of the
 * MaxSumController.
 */
int noConflicts_m(const MaxSumController& controller)
{
   int totalConflicts = 0;
   //***************************************************************************
   // For each pair of actions (notice initialisation of 2nd iterator)
   //***************************************************************************
   for(MaxSumController::ConstActionIterator it1=controller.actionBegin();
         it1!=controller.actionEnd(); ++it1)
   {
      MaxSumController::ConstActionIterator it2=it1;
      ++it2;
      for(; it2!=controller.actionEnd(); ++it2)
      {
         //*********************************************************************
         // If the current pair of actions are related, and share the same
         // value, then increment the number of conflicts.
         //*********************************************************************
         if(areVarsConnected_m(controller,it1->first,it2->first) &&
               (it1->second==it2->second) )
         {
            ++totalConflicts;
         }

      } // inner for loop

   } // outer for loop

   return totalConflicts;

} // function noConflicts_m

/**
 * Tests a specified MaxSumController for consistency with a given factor
 * graph, and tries to run it.
 * @returns the number of failures
 */
int testMaxSum_m(MaxSumController& controller, const FactorMap_m& factors)
{
   int errorCount = 0;
   try
   {
      //************************************************************************
      // Remove any factors that are not in the current factor graph
      //************************************************************************
      for(MaxSumController::ConstFactorIterator it=controller.factorBegin();
            it!=controller.factorEnd(); ++it)
      {
         if(0==factors.count(it->first))
         {
            controller.removeFactor(it->first);
         }
      }

      //************************************************************************
      // Set the factors to their values as specified in the factor map
      //************************************************************************
      for(FactorMap_m::const_iterator it=factors.begin();
            it!=factors.end(); ++it)
      {
         controller.setFactor(it->first,it->second);
      }

      //************************************************************************
      // Ensure that the controller is now consistent with the factor map
      //************************************************************************
      int consistencyResult = isConsistent_m(controller,factors);
      if(0!=consistencyResult)
      {
         errorCount += consistencyResult;
         std::cout << "Controller is inconsistent with factor graph.\n";
         return errorCount;
      }

      //************************************************************************
      // Attempt to run the max-sum algorithm
      //************************************************************************
      std::cout << "Attempting to run max-sum algorithm...";
      std::clock_t runtime = std::clock();
      int iterationCount = controller.optimise();
      runtime = std::clock() - runtime;
      std::cout << "DONE.\n";

      double seconds = static_cast<double>(runtime) / CLOCKS_PER_SEC;
      int conflictCount = noConflicts_m(controller);

      //************************************************************************
      // Display run statistics: runtime, no. iterations  and no. conflicts.
      //************************************************************************
      std::cout << "RUNTIME=" << seconds;
      std::cout << " ITERATIONS=" << iterationCount;
      std::cout << " NUM_OF_CONFLICTS=" << conflictCount;
      std::cout << std::endl;
   }
   //***************************************************************************
   // Deal with any unexpected exceptions
   //***************************************************************************
   catch(std::exception e)
   {
      std::cout << "Caught unexpected exception: " << e.what();
      ++errorCount;
      return errorCount;
   }

   //***************************************************************************
   // Return the total number of encountered errors
   //***************************************************************************
   return errorCount;

} // function testMaxSum_m


/**
 * Main function tests a maxsum controller on several factor graphs.
 */
int main()
{
   std::cout << "Hello world!\n";

   FactorMap_m factors; // Reusable FactorMap for storing current factors
   int errorCount = 0; // counts the number of failures
   MaxSumController controller; // MaxSumController for testing
   std::clock_t runtime = std::clock();

   //***************************************************************************
   // Test behaviour of an uninitialised controller
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on empty graph                                  *\n";
   std::cout << "***********************************************************\n";
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Test on singleton graph, containing only one factor and variable
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on singleton graph                              *\n";
   std::cout << "***********************************************************\n";
   genTreeGraph_m(1,1,factors);
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Create a MaxSumController and test it on a line graph (i.e. tree graph
   // with branching factor of 1.
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on line graph                                   *\n";
   std::cout << "***********************************************************\n";
   genTreeGraph_m(10,1,factors);
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Test on ring with same number of factors
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on ring graph                                   *\n";
   std::cout << "***********************************************************\n";
   genRingGraph_m(10,factors);
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Test on tree with additional factors
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on tree graph                                   *\n";
   std::cout << "***********************************************************\n";
   genTreeGraph_m(4,2,factors);
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Test removing factors from factor graph in various ways
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing Factor removal                                  *\n";
   std::cout << "***********************************************************\n";
   errorCount += testRemoval_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Test on fully connected graph with last factors.
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on colourable fully connected graph             *\n";
   std::cout << "***********************************************************\n";
   genFullGraph_m(NO_COLOURS,factors);
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Test on fully connected graph with last factors.
   //***************************************************************************
   std::cout << "***********************************************************\n";
   std::cout << "* Testing on non-colourable fully connected graph         *\n";
   std::cout << "***********************************************************\n";
   genFullGraph_m(NO_COLOURS+2,factors);
   errorCount += testMaxSum_m(controller,factors);
   std::cout << std::endl;

   //***************************************************************************
   // Report the total runtime and number of failures.
   //***************************************************************************
   runtime = std::clock() - runtime;
   float seconds = static_cast<float>(runtime) / CLOCKS_PER_SEC;
   std::cout << "***********************************************************\n";
   std::cout << "* TOTAL RUNTIME: " << std::setw(6) << std::setfill('0');
   std::cout << std::setprecision(3) << std::fixed;
   std::cout << seconds <<  "   NUMBER OF ERRORS: ";
   std::cout << std::setw(4) << std::setfill(' ');
   std::cout << errorCount << "          *\n";
   std::cout << "***********************************************************\n";

   //***************************************************************************
   // Return success if all tests in this harness have passed.
   //***************************************************************************
   if(0==errorCount)
   {
      return EXIT_SUCCESS;
   }
   return EXIT_FAILURE;

} // function main




