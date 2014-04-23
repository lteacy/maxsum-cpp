/** 
 * @file maxsum_c.h
 * Provides C compatible interface to maxsum C++ library
 * 
 */
#ifndef MAX_SUM_C_H
#define MAX_SUM_C_H

#include "maxsum/types.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
using namespace maxsum;
#endif

/**
 * Returns true if the specified variable is registered.
 * @param var id of the variable to search for.
 * @returns true if the specified variable is registered.
 */ 
int isRegistered_ms(VarID var);

/**
 * Returns the registered domain size for a specified variable.
 * @param var id of the variable to search for.
 * @returns domain size of var
 * @throws UnknownVariableException if the variable is not registered.
 */
ValIndex getDomainSize_ms(VarID var);

/**
 * Returns the number of currently registered variables.
 * @returns the number of currently registered variables.
 */
int getNumOfRegisteredVariables_ms();

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
void registerVariable_ms(VarID var, ValIndex siz);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif // MAX_SUM_C_H
