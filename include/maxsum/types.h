/**
 * @file types.h
 * Common types used Max-Sum library.
 * These are C compatible.
 * @author Luke Teacy
 */
#ifndef MAX_SUM_TYPES_H
#define MAX_SUM_TYPES_H

#ifdef __cplusplus /* Only use namespace for C++ code */
/**
 * Namespace for all public types and functions defined by the Max-Sum library.
 */
namespace maxsum
{
#endif

   /**
    * Type of values stored by maxsum::DiscreteFunction objects.
    * This is, this type is used to represent the codomain of
    * mathematical functions represented by maxsum::DiscreteFunction objects.
    * @see maxsum::DiscreteFunction
    */
   typedef double ValType;

   /**
    * Type used for uniquely identifying variables.
    * This is purposely an integer type, because we want this to be efficient
    * for storing and passing between functions. Note: bitwise operations can
    * be used in some addressing schemes, e.g. something like IP addresses.
    * @see maxsum::FactorID
    */
   typedef unsigned int VarID;

   /**
    * Type used for uniquely identifying factors in a factor graph.
    * This is purposely an integer type, because we want this to be efficient
    * for storing and passing between functions. Note: bitwise operations can
    * be used in some addressing schemes, e.g. something like IP addresses.
    * @see maxsum::VarID
    */
   typedef unsigned int FactorID;

   /**
    * Integer type used for indexing coefficient values. This is the value
    * type for all variables that are referenced an identified using
    * maxsum::VarID. In particular, if a value of type maxsum::ValIndex
    * is specified for each variable in a maxsum::DiscreteFunction object's
    * domain, then exactly one value of type maxsum::ValType will be returned
    * by element accessor functions, such as maxsum::DiscreteFunction::at
    */
   typedef int ValIndex;

#ifdef __cplusplus /* Only use namespace for C++ code */
} // namespace maxsum
#endif

#endif // MAX_SUM_TYPES_H
