
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include "maxsum/register.h"
#include "maxsum/common.h"
#include "maxsum/DiscreteFunction.h"

namespace
{

   /**
    * Structure used to hold test data for ind2sub and sub2ind functions.
    */
   struct IndexTestCase_m
   {

      int ind;
      std::vector<int> siz;
      std::vector<int> sub;

      /**
       * Constructor allocates space according to number of dimensions.
       */
      IndexTestCase_m(int dims=0) : ind(0), siz(dims), sub(dims) {}

      /**
       * Copy constructor
       */
      IndexTestCase_m(const IndexTestCase_m& k)
         : ind(k.ind),
           siz(k.siz.begin(),k.siz.end()),
           sub(k.sub.begin(),k.sub.end()) {}

      /**
       * Assignment Operator performs deep copy.
       */
      IndexTestCase_m& operator=(const IndexTestCase_m& rhs);

      /**
       * Comparison Operator
       */
      bool operator==(const IndexTestCase_m& k) const;

   }; // struct IndexTestCase_m

   /**
    * Pretty print IndexTestCases for diagnostics.
    */
   std::ostream& operator<<(std::ostream& out, const IndexTestCase_m& rhs)
   {
      out << "{dims=" << rhs.siz.size();
      out << " siz=<";

      for(int i=0; i<rhs.siz.size(); i++)
      {
         if(0!=i) out << ',';
         out << rhs.siz[i];
      }

      out << "> sub=<";

      for(int i=0; i<rhs.sub.size(); i++)
      {
         if(0!=i) out << ',';
         out << rhs.sub[i];
      }
      
      out <<">}";

      return out;

   } // << operator for IndexTestCase_m

   /**
    * Assignment Operator performs deep copy.
    */
   IndexTestCase_m& IndexTestCase_m::operator=(const IndexTestCase_m& rhs)
   {
      ind = rhs.ind;
      siz = rhs.siz;
      sub = rhs.sub;
      return *this;
   }

   /**
    * Comparison Operator
    */
   bool IndexTestCase_m::operator==(const IndexTestCase_m& k) const
   {
      //*********************************************************************
      // Check that the linear indices match
      //*********************************************************************
      if(k.ind != ind)
      {
         return false;
      }

      //*********************************************************************
      // Check that the number of dimensions match
      //*********************************************************************
      if( (k.siz.size() != siz.size()) || (k.sub.size() != sub.size()) )
      {
         return false;
      }

      //*********************************************************************
      // Check subindices and array size match
      //*********************************************************************
      for(int i=0; i<sub.size(); i++)
      {
         if(k.sub[i]!=sub[i])
         {
            return false;
         }
      }

      for(int i=0; i<siz.size(); i++)
      {
         if(k.siz[i]!=siz[i])
         {
            return false;
         }
      }

      //*********************************************************************
      // If we get to here, everything is the same, so we return true
      //*********************************************************************
      return true;
         
   } // == operator

   /**
    * Function used to read test data from ind2sub and sub2ind functions.
    * Input is read from a CSV file, in which each line represents a
    * different test case.
    * Each line as the format:
    * ndims,ind,siz[0],...,siz[ndims-1],sub[0],...,sub[ndims-1]
    */
   int readIndexTestData
   (
    const char* const psFilename,
    std::list<IndexTestCase_m>& testCases
   )
   {
      //*********************************************************************
      // Open CSV File for reading
      //*********************************************************************
      std::fstream file(psFilename, std::ios::in);
      if(!file.is_open())
      {
         std::cerr << "Could not open file: " << psFilename << std::endl;
         return 1;
      }

      //*********************************************************************
      // Construct a test case for each line of the input
      //*********************************************************************
      std::string csvLine;
      while(std::getline(file,csvLine))
      {
         //******************************************************************
         // Parse the current line into a list of integers
         //******************************************************************
         std::istringstream csvStream(csvLine);
         std::queue<int> lineData;
         while(csvStream.good())
         {
            int curInt = -1;
            csvStream >> curInt;
            csvStream.ignore(1,',');
            lineData.push(curInt);
         }

         //******************************************************************
         //    Ignore empty lines
         //******************************************************************
         if(lineData.empty())
         {
            std::cerr << "Warning: Encountered Empty line." << std::endl;
            continue;
         }

         //******************************************************************
         // Validate the current line by checking the number of integers
         // against the number of dimensions.
         //******************************************************************
         int ndims = lineData.front();
         lineData.pop();

         int expectedSize = 2*ndims+1;
         if(lineData.size()!=expectedSize)
         {
            std::cerr << "Invalid test case: ndims=" << ndims << ", len="
               << lineData.size() << ", expected=" << expectedSize
               << std::endl;
            return 2;
         }

         //******************************************************************
         // Create a new Testcase to hold the data
         //******************************************************************
         IndexTestCase_m curCase(ndims);

         //******************************************************************
         // Read the linear index
         //******************************************************************
         curCase.ind = lineData.front();
         lineData.pop();

         //******************************************************************
         // Read the size array
         //******************************************************************
         for(int i=0; i<ndims; i++)
         {
            curCase.siz[i] = lineData.front();
            lineData.pop();
         }

         //******************************************************************
         // Read the sub index array
         //******************************************************************
         for(int i=0; i<ndims; i++)
         {
            curCase.sub[i] = lineData.front();
            lineData.pop();
         }

         //******************************************************************
         // Sanity check to make sure we've processed all the numbers on
         // this line.
         //******************************************************************
         if(!lineData.empty())
         {
            std::cerr << "Failed to process all data on current CVS line."
                      << std::endl;
            return 3;
         }

         //******************************************************************
         // Stick the current case into the output list
         //******************************************************************
         testCases.push_back(curCase);

      } // loop over CVS file

      //*********************************************************************
      // Return successfully
      //*********************************************************************
      file.close();
      return 0;

   } // function readIndexTestData

} // module namespace

int testIndices()
{
   //***************************************************************************
   // Load Test Data for ind2sub and sub2ind functions
   //***************************************************************************
   std::list<IndexTestCase_m> testCases;
   if(0!=readIndexTestData("testdata/ind2sub_testdata.csv",testCases))
   {
      std::cout << "Failed to read ind2sub test data." << std::endl;
      return 1;
   }

   //***************************************************************************
   // For each test case, ensure result is reproduced exactly.
   //***************************************************************************
   typedef std::list<IndexTestCase_m>::const_iterator CaseIterator;
   int count = 0;
   for(CaseIterator it = testCases.begin(); it != testCases.end(); it++)
   {
      std::cout << "Test " << ++count << ": " << (*it) << " : ";
      //************************************************************************
      // Validate output from sub2ind
      //************************************************************************
      const int outInd = maxsum::sub2ind(it->siz,it->sub);
      if(outInd != it->ind)
      {
         std::cout << "Incorrect result from sub2ind: " << outInd << " !="
                   << it->ind << std::endl;
         return 2;
      }

      //************************************************************************
      // Validate output from ind2sub
      //************************************************************************
      int ndims = it->siz.size();
      std::vector<int> outSub(ndims);
      maxsum::ind2sub(it->siz,it->ind,outSub);

      if(outSub.size() != ndims)
      {
         std::cout << "Incorrect number of dimensions from ind2sub."
                   << std::endl;
         return 3;
      }

      for(int i=0; i<ndims; i++)
      {
         if(outSub[i] != it->sub[i])
         {
            std::cout << "Incorrect result from ind2sub" << std::endl;
            IndexTestCase_m result(ndims);
            result.ind = outInd;
            result.siz = it->siz;
            result.sub = outSub;
            std::cout << "Last Result: " << result << " ind: " << outInd
               << "==" << it->ind << std::endl;
            return 4;
         }
      }

      std::cout << "Passed." << std::endl;

   } // for loop 

   //***************************************************************************
   // Read test data from exception cases - these should all fail
   //***************************************************************************
   std::list<IndexTestCase_m> badCases;
   if(0!=readIndexTestData("testdata/ind2sub_baddata.csv",badCases))
   {
      std::cout << "Failed to read ind2sub test data." << std::endl;
      return 1;
   }

   //***************************************************************************
   // Test Bad sub2ind cases - these should all throw exceptions
   //***************************************************************************
   count = 0;
   for(CaseIterator it = badCases.begin(); it != badCases.end(); it++)
   {
      std::cout << "Exception Test " << ++count << ": " << (*it) << " : ";

      //************************************************************************
      // Validate exception from sub2ind
      //************************************************************************
      try
      {
         maxsum::sub2ind(it->siz,it->sub);
         std::cout << "sub2ind Failed to throw exception." << std::endl;
         return 5;
      }
      catch(maxsum::OutOfRangeException e)
      {
         // This is good
      }

      //************************************************************************
      // Validate exception from ind2sub
      //************************************************************************
      try
      {
         std::vector<int> badSub;
         maxsum::ind2sub(it->siz,it->ind,badSub);
         std::cout << "ind2sub Failed to throw exception." << std::endl;
         return 6;
      }
      catch(maxsum::OutOfRangeException e)
      {
         // This is good
      }

      std::cout << "Passed." << std::endl;
 
   } // for loop

   //***************************************************************************
   // Return success
   //***************************************************************************
   return 0;

} // function testIndices

/**
 * Function for testing variable registration functions.
 */
int testRegister()
{
   using namespace maxsum;

   //***************************************************************************
   // Try to create a function before registering its variables
   //***************************************************************************
   std::cout << "Trying to create function with unregistered variables.\n";
   VarID aDomain[] = {1,2,3};
   ValIndex aSiz[] = {10, 20, 5};

   try
   {
      DiscreteFunction a(aDomain,aDomain+2);
      std::cout << "Missing UnknownVariableException" << std::endl;
      return 1;
   }
   catch(UnknownVariableException e) { /* this should happen */ }

   //***************************************************************************
   // Check that nothing has been registered yet
   //***************************************************************************
   std::cout << "Checking that nothing is registered yet.\n";
   if( isRegistered(14) ) 
   {
      std::cout << "Registration of non-existant variable" << std::endl;
      return 2;
   }

   if(getNumOfRegisteredVariables()!=0)
   {
      std::cout << "Incorrect number of registered variables." << std::endl;
      std::cout << "Count should be 0, but is actually " << 
         getNumOfRegisteredVariables() << std::endl;
      return 2;
   }

   //***************************************************************************
   // Try to register a single variable
   //***************************************************************************
   std::cout << "Trying to register single variable.\n";
   registerVariable(14,2);
   if(!isRegistered(14))
   {
      std::cout << "Variable not registered" << std::endl;
      return 3;
   }

   if(2!=getDomainSize(14))
   {
      std::cout << "Variable has wrong domain size" << std::endl;
      return 4;
   }

   //***************************************************************************
   // Try to register variables with empty or singleton domains
   //***************************************************************************
   std::cout << "Trying to register invalid domains.\n";
   try
   {
      registerVariable(23,0);
      std::cout << "Empty domains should not be registered" << std::endl;
      return 5;
   }
   catch(OutOfRangeException e) { /* this should happen */ }

   try
   {
      registerVariable(27,1);
      std::cout << "Singleton domains should not be registered" << std::endl;
      return 6;
   }
   catch(OutOfRangeException e) { /* this should happen */ }

   //***************************************************************************
   // Try to register a list of variables
   //***************************************************************************
   std::cout << "Trying to register a list of variables.\n";
   registerVariables(aDomain,aDomain+3,aSiz,aSiz+3);

   for(int k=0; k<3; k++)
   {
      if(!isRegistered(aDomain[k]))
      {
         std::cout << "List registration failed." << std::endl;
         return 7;
      }

      if(aSiz[k]!=getDomainSize(aDomain[k]))
      {
         std::cout << "Wrong size for list variables." << std::endl;
         return 8;
      }
   }

   //***************************************************************************
   // Try to register one of the variables again
   //***************************************************************************
   std::cout << "Trying to register a variable again.\n";
   registerVariable(14,2);
   if(!isRegistered(14))
   {
      std::cout << "Variable not registered" << std::endl;
      return 9;
   }

   if(2!=getDomainSize(14))
   {
      std::cout << "Variable has wrong domain size" << std::endl;
      return 10;
   }

   //***************************************************************************
   // Try to register some more variables again
   //***************************************************************************
   std::cout << "Trying to register a variable list again.\n";
   registerVariables(aDomain+1,aDomain+3,aSiz+1,aSiz+3);

   for(int k=0; k<3; k++)
   {
      if(!isRegistered(aDomain[k]))
      {
         std::cout << "List registration failed." << std::endl;
         return 11;
      }

      if(aSiz[k]!=getDomainSize(aDomain[k]))
      {
         std::cout << "Wrong size for list variables." << std::endl;
         return 12;
      }
   }

   //***************************************************************************
   // Try to register a variable again with a different domain
   //***************************************************************************
   std::cout << "Trying to register inconsistent domains.\n";
   try
   {
      registerVariable(3,100);
      std::cout << "Domain size cannot be changed" << std::endl;
      return 12;
   }
   catch(InconsistentDomainException e) { /* this should happen */ }

   //***************************************************************************
   // Try to register some more variables with different domains
   //***************************************************************************
   try
   {
      ValIndex badSizes[] = {23,94};
      registerVariables(aDomain+1,aDomain+3,badSizes,badSizes+2);
      std::cout << "Domain size of list variables cannot change" << std::endl;
      return 13;
   }
   catch(InconsistentDomainException e) { /* this should happen */ }

   //***************************************************************************
   // Check that the number of registered variables is correct
   //***************************************************************************
   if(getNumOfRegisteredVariables()!=4)
   {
      std::cout << "Incorrect number of registered variables." << std::endl;
      std::cout << "Count should be 4, but is actually " << 
         getNumOfRegisteredVariables() << std::endl;
      return 14;
   }

   //***************************************************************************
   // Try to create a function that depends on registered variables
   //***************************************************************************
   std::cout << "Trying to create function with registered variables.\n";
   try
   {
      DiscreteFunction b(aDomain,aDomain+2);
   }
   catch(UnknownVariableException e)
   {
      std::cout << "Unknown variable exception thrown for registered "
                   "variables." << std::endl;
      return 15;
   }

   //***************************************************************************
   // Return success
   //***************************************************************************
   std::cout << "Registration tests all passed.\n";
   return 0;

} // function testRegister

int main()
{
   //***************************************************************************
   // Test linear and sub index functions
   //***************************************************************************
   std::cout << "***************************************" << std::endl;
   std::cout << "Test indices" << std::endl;
   std::cout << "***************************************" << std::endl;
   int exitValue = testIndices();
   if(0!=exitValue)
   {
      return exitValue;
   }

   //***************************************************************************
   // Test variable register
   //***************************************************************************
   std::cout << "***************************************" << std::endl;
   std::cout << "Test Variable Register" << std::endl;
   std::cout << "***************************************" << std::endl;
   exitValue = testRegister();
   if(0!=exitValue)
   {
      return exitValue;
   }

} // function main

