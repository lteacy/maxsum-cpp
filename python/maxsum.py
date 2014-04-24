from ctypes import *
from ctypes.util import *

# Try to find and load maxsum-cpp library
maxsum_libfile = find_library("MaxSum")
print maxsum_libfile
lib_maxsum = CDLL(maxsum_libfile)

class MaxSumException(Exception):
   """Exception raised for errors caused by maxsum library.

   Attributes:
      expr -- input expression in which the error occurred
      msg  -- explanation of the error
   """

   def __init__(self, msg):
      self.msg = msg

# Returns true if the specified variable is registered.
# @param var id of the variable to search for.
# @returns true if the specified variable is registered.
def isRegistered(var):
   if 0<lib_maxsum.isRegistered_ms(var):
      return True
   else:
      return False

# Returns the registered domain size for a specified variable.
# @param var id of the variable to search for.
# @returns domain size of var, or -1 if variable is unknown.
def getDomainSize(var):
   result = lib_maxsum.getDomainSize_ms(var)
   if 0>result:
      raise MaxSumException("Domain size requested for unknown variable.")
   else:
      return result
   
# Returns the number of currently registered variables.
# @returns the number of currently registered variables.
def getNumOfRegisteredVariables_ms():
   return lib_maxsum.getNumOfRegisteredVariables_ms()

# Registers a variable with a specified domain size.
# Puts the specified variable in a global register, and stores its domain
# size. Variables can be registered multiple times, but their domain size
# must never change.
# @returns -1 if this variable is already registered, but with a different
# domain size.
# @param var the unique id of this variable
# @param siz the domain size of this variable
# @see maxsum::registerVariables
def registerVariable(var, siz):
   result = lib_maxsum.registerVariable_ms(var,siz)
   if 0>result:
      raise MaxSumException("Domain size requested for unknown variable.")
   else:
      return result

# Python version of matlab sub2ind function
# @param siz vector of dimension sizes
# @param sub vector of subindices
# @return corresponding scalar index
def sub2ind(siz,sub):
   if len(siz) != len(sub):
      raise MaxSumException("siz and subindex array must have same length.")
   nDims = len(siz)
   arrayType = c_int * nDims
   sizArray = arrayType()
   subArray = arrayType()
   for k in range(len(siz)):
      sizArray[k] = siz[k]
      subArray[k] = sub[k]

   idx = lib_maxsum.sub2ind_ms(nDims,byref(sizArray),byref(subArray))
   if 0>idx:
      raise MaxSumException("Exception thrown by maxsum::sub2ind.")
   else:
      return idx

# Python version of matlab ind2sub function
# @param siz vector of dimension sizes
# @param scalar index
# @return corresponding list of subindices
def ind2sub(siz,idx):
   nDims = len(siz)
   arrayType = c_int * nDims
   sizArray = arrayType()
   subArray = arrayType()
   for k in range(len(siz)):
      sizArray[k] = siz[k]

   status = lib_maxsum.ind2sub_ms(idx, nDims, byref(sizArray), byref(subArray))

   if 0>status:
      raise MaxSumException("Exception thrown by maxsum::ind2sub.")
   else:
      sub = []
      for k in subArray:
         sub.append(k)
      return sub




