from ctypes import *
from ctypes.util import *

# Try to find and load maxsum-cpp library
maxsum_libfile = find_library("MaxSum")
lib_maxsum = CDLL(maxsum_libfile)

# Specify argument types where required
lib_maxsum.newMaxSumController_ms.argtypes = [c_int,c_double]
lib_maxsum.newMaxSumController_ms.restype = c_void_p

lib_maxsum.deleteMaxSumController_ms.argtypes = [c_void_p]
lib_maxsum.deleteMaxSumController_ms.restype = None

lib_maxsum.setFactor_ms.argtypes = [c_void_p, c_uint, c_int, POINTER(c_uint), POINTER(c_double)]
lib_maxsum.setFactor_ms.restype = c_int 

lib_maxsum.removeFactor_ms.argtypes = [c_void_p, c_uint]
lib_maxsum.removeFactor_ms.restype = None

lib_maxsum.removeFactor_ms.argtypes = [c_void_p, c_uint]
lib_maxsum.removeFactor_ms.restype = None

lib_maxsum.clearAll_ms.argtypes = [c_void_p]
lib_maxsum.clearAll_ms.restype = None

lib_maxsum.noFactors_ms.argtypes = [c_void_p]
lib_maxsum.noFactors_ms.restype = c_uint

lib_maxsum.noVars_ms.argtypes = [c_void_p]
lib_maxsum.noVars_ms.restype = c_uint

lib_maxsum.optimise_ms.argtypes = [c_void_p]
lib_maxsum.optimise_ms.restype = c_int

lib_maxsum.getValues_ms.argtypes = [c_void_p,POINTER(c_uint),POINTER(c_int)]
lib_maxsum.getValues_ms.restype = c_int

class MaxSumException(Exception):
   """Exception raised for errors caused by maxsum library.

   Attributes:
      expr -- input expression in which the error occurred
      msg  -- explanation of the error
   """

   def __init__(self, msg):
      self.msg = msg

   def what(self):
      return self.msg

class MaxSumController:
   """Python Wrapper for MaxSumController in maxsum-cpp library."""

   def __init__(self,maxIterations=100,maxnorm=0.0000001):
      self.obj = lib_maxsum.newMaxSumController_ms(maxIterations,maxnorm)

   def __del__(self):
      lib_maxsum.deleteMaxSumController_ms(self.obj)

   def setFactor(self,factor_id,domain,data):
      DomainType = c_uint * len(domain)
      DataType = c_double * len(data)

      nDims = len(domain)
      cDomain = DomainType(*domain)
      cData = DataType(*data)

      err = lib_maxsum.setFactor_ms(self.obj,factor_id,nDims,cDomain,cData)

      if 0>err:
         raise MaxSumException("Error while setting factor. Domain may contain unregistered variables?")

   def removeFactor(self,factor_id):
      lib_maxsum.removeFactor_ms(self.obj,factor_id)

   def clearAll(self):
      lib_maxsum.clearAll_ms(self.obj)

   def noFactors(self):
      return lib_maxsum.noFactors_ms(self.obj)

   def noVars(self):
      return lib_maxsum.noVars_ms(self.obj)

   def optimise(self):
      return lib_maxsum.optimise_ms(self.obj)

   def getValues(self):
      VariableType = c_uint * self.noVars()
      ValueType = c_int * self.noVars()
      var_ids = VariableType()
      values = ValueType()
      varCount = lib_maxsum.getValues_ms(self.obj,var_ids,values)

      # sanity check we got the correct number of variables
      if varCount != self.noVars():
         raise MaxSumException("Incorrect number of variables returned by getValues")

      # write result into a dictionary and return
      valueMap = dict()
      for k in range(self.noVars()):
         valueMap[var_ids[k]] = values[k]

      return valueMap

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




