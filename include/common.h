/**
 * @file common.h
 * Common types and functions used by Max-Sum library.
 * In particular, this file defines Exception classes used by the library, and
 * functions for calculating subindices and linear indices for referencing
 * the contents of N-D arrays.
 * @author Luke Teacy
 */
#ifndef MAX_SUM_COMMON_H
#define MAX_SUM_COMMON_H

#include <cfloat>
#include <string>
#include <vector>
#include <exception>

/**
 * Namespace for all public types and functions defined by the Max-Sum library.
 */
namespace maxsum
{
   /**
    * Type of values stored by maxsum::DiscreteFunction objects.
    */
   typedef double ValType;

   /**
    * Default tolerance used for comparing values of type maxsum::ValType.
    * This is the default value used by the maxsum::equalWithinTolerance()
    * function, when comparing to maxsum::DiscreteFunction objects for equality.
    * Note, if ValType is ever redefined, then this value should be changed
    * appropriately also.
    */
   const ValType DEFAULT_VALUE_TOLERANCE = DBL_EPSILON * 1000.0;

   /**
    * Type used for uniquely identifying variables.
    * This is purposely an integer type - want this to be efficient for
    * storing and passing between functions. Note, bitwise operations can
    * be used in some addressing schemes, e.g. something like IP addresses.
    */
   typedef unsigned int VarID;

   /**
    * Type used for uniquely identifying factors in a factor graph.
    * This is purposely an integer type - want this to be efficient for
    * storing and passing between functions. Note, bitwise operations can
    * be used in some addressing schemes, e.g. something like IP addresses.
    */
   typedef unsigned int FactorID;

   /**
    * Integer type used for indexing coefficient values.
    */
   typedef int ValIndex;

   /**
    * Exception thrown when there has been an attempt to access an element of
    * a container that does not exist, and cannot be created on demand.
    */
   class NoSuchElementException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Constructs a new exception with specified location and message.
       * @param[in] where_ the source code location where this exception was
       * generated.
       * @param[in] mesg_ Message describing the reason for this exception.
       */
      NoSuchElementException 
         (const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("NoSuchElementException: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~NoSuchElementException() throw() {}

   }; // NoSuchElementException

   /**
    * Exception thrown by PostOffice::popNotice when there are no active
    * notices.
    * @see PostOffice::popNotice
    */
   class EmptyNoticeException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Constructs a new exception with specified location and message.
       * @param[in] where_ the source code location where this exception was
       * generated.
       * @param[in] mesg_ Message describing the reason for this exception.
       */
      EmptyNoticeException 
         (const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("EmptyNoticeException: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~EmptyNoticeException() throw() {}

   }; // EmptyNoticeException

   /**
    * Exception thrown when a maxsum::PostOffice does not recognise the ID of
    * a Sender or Receiver.
    */
   class UnknownAddressException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Constructs a new exception with specified location and message.
       * @param[in] where_ the source code location where this exception was
       * generated.
       * @param[in] mesg_ Message describing the reason for this exception.
       */
      UnknownAddressException
         (const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("UnknownAddressException: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~UnknownAddressException() throw() {}

   }; // UnknownAddressException

   /**
    * Exception thrown when subindices are incorrectly specified for a function.
    */
   class BadDomainException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Constructs a new exception with specified location and message.
       * @param[in] where_ the source code location where this exception was
       * generated.
       * @param[in] mesg_ Message describing the reason for this exception.
       */
      BadDomainException
         (const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("BadDomainException: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~BadDomainException() throw() {}

   }; // BadDomainException

   /**
    * Exception thrown when indices are out of range.
    */
   class OutOfRangeException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Constructs a new exception with specified location and message.
       * @param[in] where_ the source code location where this exception was
       * generated.
       * @param[in] mesg_ Message describing the reason for this exception.
       */
      OutOfRangeException(const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("Out of range exception: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~OutOfRangeException() throw() {}

   }; // out of range exception

   /**
    * Exception thrown conflicting domains are specified for a variable.
    */
   class DomainConflictException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Creates a new expection of this type.
       * @param[in] where_ the position in the source code where this was
       * generated.
       * @param[in] mesg_ message describing the cause of this exception.
       */
      DomainConflictException(const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("Domain conflict exception: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~DomainConflictException() throw() {}

   }; // Domain conflict exception

   /**
    * Exception thrown when a variable is not registered.
    */
   class UnknownVariableException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Creates a new expection of this type.
       * @param[in] where_ the position in the source code where this was
       * generated.
       * @param[in] mesg_ message describing the cause of this exception.
       */
      UnknownVariableException(const std::string where_, const std::string mesg_)
         throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("Unknown variable exception: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~UnknownVariableException() throw() {}

   }; // UnknownVariableException

   /**
    * Exception thrown when variable domains are inconsistent.
    */
   class InconsistentDomainException : public std::exception
   {
   protected:

      /**
       * String identifying the source code locatioin where this exceptioin
       * was generated.
       */
      const std::string where;

      /**
       * Message describing the cause of this exception.
       */
      const std::string mesg;

   public:

      /**
       * Creates a new expection of this type.
       * @param[in] where_ the position in the source code where this was
       * generated.
       * @param[in] mesg_ message describing the cause of this exception.
       */
      InconsistentDomainException
      (
       const std::string where_,
       const std::string mesg_
      )
      throw(): where(where_), mesg(mesg_) {}

      /**
       * Returns a message describing the cause of this exception, and
       * the location it was thrown.
       * @returns a message describing the cause of this exception, and
       * the location it was thrown.
       */
      const char* what() const throw()
      {
         return std::string("Domain conflict exception: " + mesg +
               "\t[ in "+where +" ]").c_str();
      }

      /**
       * Destroys this exception and free's its allocated resources.
       */
      virtual ~InconsistentDomainException() throw() {}

   }; // InconsistentDomainException

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
