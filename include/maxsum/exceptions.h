/**
 * @file exceptions.h
 * Defines exception classes for all error conditions that may occur in the
 * maxsum library.
 * @author Luke Teacy
 */
#ifndef MAXSUM_EXCEPTIONS_H
#define MAXSUM_EXCEPTIONS_H

#include <string>
#include <exception>

/**
 * Namespace for all public types and functions defined by the Max-Sum library.
 */
namespace maxsum
{
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
    * Exception thrown by maxsum::util::PostOffice::popNotice when there are no
    * active notices.
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
    * Exception thrown when a maxsum::util::PostOffice does not recognise the
    * ID of a Sender or Receiver.
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
    * Exception thrown when subindices are incorrectly specified for a 
    * maxsum::DiscreteFunction.
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
    * Exception thrown when conflicting domains are specified for a
    * variable.
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
    * Exception thrown when a variable is referred to, but has not yet been
    * registered using either maxsum::registerVariable
    * or maxsum::registerVariables.
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
    * Exception thrown when variable domains are somehow registered
    * as inconsistent.
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

} // namespace maxsum

#endif // MAXSUM_EXCEPTIONS_H
