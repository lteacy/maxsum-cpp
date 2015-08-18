/**
 * @file DiscreteFunction.cpp
 * Implementation of member functions of class maxsum::DiscreteFunction
 */
#include <cmath>
#include <vector>
#include <cstdarg>
#include <algorithm>
#include <iostream>
#include <maxsum/DiscreteFunction.h>
#include <maxsum/DomainIterator.h>
#include <maxsum/common.h>

using namespace maxsum;

// module namespace for private module code.
namespace
{
   inline ValType add_m(ValType x, ValType y)
   {
      return x+y;
   }

} // module namespace


/**
 * Access coefficient by subindices specified by the current indices
 * specified by a maxsum::DomainIterator. Note: this works by accessing
 * maxsum::DomainIterator::getSubInd() array. If the domain of the
 * maxsum::DomainIterator exactly matches the domain of this function,
 * then it is more efficient to index using the corresponding linear
 * index instead. That is
 * <p>
 * <code>
 * maxsum::DiscreteFunction f(...); // some function
 * maxsum::DomainIterator it(f); // iterator over domain of f
 * maxsum::ValType x = f(it); // this works
 * maxsum::ValType x = f(it.getInd()); // more efficient in this case.
 * </code>
 * </p>
 * @param[in] it Iterator used to index this function.
 * @see maxsum::DomainIterator::getSubInd()
 * @see maxsum::DomainIterator::getInd()
 */
ValType& DiscreteFunction::operator()(const DomainIterator& it)
{
   return (*this)(it.getVars(),it.getSubInd());
}

/**
 * Make the domain of this function include the domain of another.
 * If necessary, the domain of this function is expanded to include the
 * domain of the parameter fun.
 * @param[in] fun function whose domain we want to expand to.
 * @todo Make this more efficient by using fun.size_i cache.
 * @post domain of this is union of its previous domain, with that of fun.
 */
void DiscreteFunction::expand(const DiscreteFunction& fun)
{
   this->expand(fun.varBegin(),fun.varEnd());
}

/**
 * Expand the domain of this function to include a named variable.
 * @param[in] var the id of the variable to add to this function's domain.
 * @post The domain of this function is the union of its previous domain,
 * and the additional specified variable.
 */
void DiscreteFunction::expand(const VarID var)
{
   VarID vars[] = {var};
   expand(vars,vars+1);
}

/**
 * Returns true if this function depends on the specified variable.
 * @param[in] var The id of the variable to search for in this function's
 * domain.
 * @returns true if <code>var</code> is in this function's domain.
 */
bool DiscreteFunction::dependsOn(VarID var) const
{
   return varEnd()!=find(varBegin(),varEnd(),var);
}

/**
 * Access coefficient by subindices specified by the current indices
 * specified by a maxsum::DomainIterator. Note: this works by accessing
 * maxsum::DomainIterator::getSubInd() array. If the domain of the
 * maxsum::DomainIterator exactly matches the domain of this function,
 * then it is more efficient to index using the corresponding linear
 * index instead. That is
 * <p>
 * <code>
 * maxsum::DiscreteFunction f(...); // some function
 * maxsum::DomainIterator it(f); // iterator over domain of f
 * maxsum::ValType x = f(it); // this works
 * maxsum::ValType x = f(it.getInd()); // more efficient in this case.
 * </code>
 * </p>
 * @param[in] it Iterator used to index this function.
 * @see maxsum::DomainIterator::getSubInd()
 * @see maxsum::DomainIterator::getInd()
 */
const ValType& DiscreteFunction::operator()(const DomainIterator& it) const
{
   return (*this)(it.getVars(),it.getSubInd());
}

/**
 * Access coefficient using subindices specified in argument list.
 * Number of arguments needs to match the number of variables in this
 * function's domain, otherwise bad things will happen.
 */
ValType& DiscreteFunction::operator()(ValIndex ind1, ValIndex ind2, ...)
{
   //***************************************************************************
   // Create a vector in which to hold all the indices
   //***************************************************************************
   std::vector<ValIndex> indices;
   indices.reserve(vars_i.size());

   //***************************************************************************
   // Stick the named indices in the list
   //***************************************************************************
   indices.push_back(ind1);
   indices.push_back(ind2);

   //***************************************************************************
   // Stick any other indices passed as arguments in the list
   //***************************************************************************
   va_list args;
   va_start ( args, ind2 );
   for(int k=0; k<vars_i.size()-2; ++k)
   {
      indices.push_back(va_arg(args,ValIndex));
   }
   va_end(args);

   //***************************************************************************
   // If all is well return the specified value
   //***************************************************************************
   return (*this)(indices.begin(),indices.end());

} // operator()(ValIndex ind1, ValIndex ind2, ...)

/**
 * Access coefficient using subindices specified in argument list.
 */
const ValType& DiscreteFunction::operator()
   (ValIndex ind1, ValIndex ind2, ...) const
{
   //***************************************************************************
   // Create a vector in which to hold all the indices
   //***************************************************************************
   std::vector<ValIndex> indices;
   indices.reserve(vars_i.size());

   //***************************************************************************
   // Stick the named indices in the list
   //***************************************************************************
   indices.push_back(ind1);
   indices.push_back(ind2);

   //***************************************************************************
   // Stick any other indices passed as arguments in the list
   //***************************************************************************
   va_list args;
   va_start ( args, ind2 );
   for(int k=0; k<vars_i.size()-2; ++k)
   {
      indices.push_back(va_arg(args,ValIndex));
   }
   va_end(args);

   //***************************************************************************
   // If all is well return the specified value
   //***************************************************************************
   return (*this)(indices.begin(),indices.end());

} // operator()(ValIndex ind1, ValIndex ind2, ...) const

/**
 * Access coefficient using subindices specified as argumments
 * This is equivalent to DiscreteFunction::operator()
 * but is more convenient when for use with pointers e.g.
 * <p>
 * <code>
 * x->(k); // illegal
 * <br/>
 * x->at(k); // ok
 * </code>
 * </p>
 * @see DiscreteFunction::operator()
 */
ValType& DiscreteFunction::at(ValIndex ind1, ValIndex ind2, ...)
{
   //***************************************************************************
   // Create a vector in which to hold all the indices
   //***************************************************************************
   std::vector<ValIndex> indices;
   indices.reserve(vars_i.size());

   //***************************************************************************
   // Stick the named indices in the list
   //***************************************************************************
   indices.push_back(ind1);
   indices.push_back(ind2);

   //***************************************************************************
   // Stick any other indices passed as arguments in the list
   //***************************************************************************
   va_list args;
   va_start ( args, ind2 );
   for(int k=0; k<vars_i.size()-2; ++k)
   {
      indices.push_back(va_arg(args,ValIndex));
   }
   va_end(args);

   //***************************************************************************
   // If all is well return the specified value
   //***************************************************************************
   return (*this)(indices.begin(),indices.end());

} // function at()

/**
 * Access coefficient using subindices specified as argumments
 * This is equivalent to DiscreteFunction::operator()
 * but is more convenient when for use with pointers e.g.
 * <p>
 * <code>
 * x->(k); // illegal
 * <br/>
 * x->at(k); // ok
 * </code>
 * </p>
 * @see DiscreteFunction::operator()
 */
const ValType& DiscreteFunction::at(ValIndex ind1, ValIndex ind2, ...) const
{
   //***************************************************************************
   // Create a vector in which to hold all the indices
   //***************************************************************************
   std::vector<ValIndex> indices;
   indices.reserve(vars_i.size());

   //***************************************************************************
   // Stick the named indices in the list
   //***************************************************************************
   indices.push_back(ind1);
   indices.push_back(ind2);

   //***************************************************************************
   // Stick any other indices passed as arguments in the list
   //***************************************************************************
   va_list args;
   va_start ( args, ind2 );
   for(int k=0; k<vars_i.size()-2; ++k)
   {
      indices.push_back(va_arg(args,ValIndex));
   }
   va_end(args);

   //***************************************************************************
   // If all is well return the specified value
   //***************************************************************************
   return (*this)(indices.begin(),indices.end());

} // function at() const

/**
 * Swaps the value and domain of this function with another.
 * After the call to this member function, the elements in this function
 * are those which were in fun before the call, and the elements of fun
 * are those which were in this. 
 * @param fun Another DiscreteFunction whose value and domain is swapped
 * with that of this one.
*/
void DiscreteFunction::swap(DiscreteFunction& fun)
{
   vars_i.swap(fun.vars_i);
   size_i.swap(fun.size_i);
   values_i.swap(fun.values_i);
}

/**
 * Sets this function to a constant scalar value.
 * @param[in] val the value to assign to this function.
 */
DiscreteFunction& DiscreteFunction::operator=(ValType val)
{
   vars_i.clear();
   size_i.clear();
   values_i.resize(1);
   values_i(0) = val; 
   return *this;
}

/**
 * Sets this function to be equal to another.
 * @param[in] val the value to assign to this function.
 */
DiscreteFunction& DiscreteFunction::operator=(const DiscreteFunction& val)
{
   vars_i = val.vars_i;
   size_i = val.size_i;
   values_i = val.values_i;
   return *this;
}

/**
 * Adds a function to this one, expanding the domain if necessary.
 */
DiscreteFunction& DiscreteFunction::operator+=(const DiscreteFunction& rhs)
{
   //***************************************************************************
   // Vectorise if operands have the same domain
   //***************************************************************************
   if(sameDomain(*this,rhs))
   {
      values_i += rhs.values_i;
      return *this;
   }

   //***************************************************************************
   // If necessary, expand the domain of this function to include the domain
   // of the input function
   //***************************************************************************
   expand(rhs); 

   //***************************************************************************
   // Add corresponding elements of input function to this one.
   //***************************************************************************
   for(DomainIterator it(*this); it.hasNext(); ++it)
   {
      this->at(it.getInd()) += rhs(it);
   }
   
   return *this;
}

/**
 * Subtracts a function from this one, expanding domain if necessary.
 */
DiscreteFunction& DiscreteFunction::operator-=(const DiscreteFunction& rhs)
{
   //***************************************************************************
   // Vectorise if operands have the same domain
   //***************************************************************************
   if(sameDomain(*this,rhs))
   {
      values_i -= rhs.values_i;
      return *this;
   }

   //***************************************************************************
   // If necessary, expand the domain of this function to include the domain
   // of the input function
   //***************************************************************************
   expand(rhs); 

   //***************************************************************************
   // Subtract corresponding elements of input function to this one.
   //***************************************************************************
   for(DomainIterator it(*this); it.hasNext(); ++it)
   {
      this->at(it.getInd()) -= rhs(it);
   }
   
   return *this;
}

/**
 * Multiplies this function by another, expanding domain if necessary.
 */
DiscreteFunction& DiscreteFunction::operator*=(const DiscreteFunction& rhs)
{
   //***************************************************************************
   // Vectorise if operands have the same domain
   //***************************************************************************
   if(sameDomain(*this,rhs))
   {
      values_i *= rhs.values_i;
      return *this;
   }

   //***************************************************************************
   // If necessary, expand the domain of this function to include the domain
   // of the input function
   //***************************************************************************
   expand(rhs); 

   //***************************************************************************
   // Multiply corresponding elements of input function to this one.
   //***************************************************************************
   for(DomainIterator it(*this); it.hasNext(); ++it)
   {
      this->at(it.getInd()) *= rhs(it);
   }
   
   return *this;
}

/**
 * Divides this function by another, expanding domain if necessary.
 */
DiscreteFunction& DiscreteFunction::operator/=(const DiscreteFunction& rhs)
{
   //***************************************************************************
   // Vectorise if operands have the same domain
   //***************************************************************************
   if(sameDomain(*this,rhs))
   {
      values_i /= rhs.values_i;
      return *this;
   }

   //***************************************************************************
   // If necessary, expand the domain of this function to include the domain
   // of the input function
   //***************************************************************************
   expand(rhs); 

   //***************************************************************************
   // Multiply corresponding elements of input function to this one.
   //***************************************************************************
   for(DomainIterator it(*this); it.hasNext(); ++it)
   {
      this->at(it.getInd()) /= rhs(it);
   }
   
   return *this;
}

/**
 * Returns the element-wise maximum of this function and a specified
 * scalar. That is, if M = N.max(s) then M(k)=max(N(k),s).
 * @param[in] s the scalar value to compare
 * @param[out] result the result of the operation.
 */
void DiscreteFunction::max(const ValType s, DiscreteFunction& result)
{

// If possible use eigen array op
#if ((EIGEN_WORLD_VERSION == 3) && (EIGEN_MAJOR_VERSION >= 1)) || (EIGEN_WORLD_VERSION > 3)
   result.values_i = this->values_i.max(s);

// Otherwise use basic implementation
#else
    result = *this;
    for(int k=0; k<result.domainSize(); k++)
    {
        result(k) = (s > result(k)) ? s : result(k);
    }

#endif

} // max


/**
 * Returns the maximum scalar value for function across entire domain.
 */
ValType DiscreteFunction::max() const
{
   return values_i.maxCoeff();
}

/**
 * Returns the maximum scalar value for function across entire domain.
 */
ValType DiscreteFunction::min() const
{
   return values_i.minCoeff();
}

/**
 * Returns the linear index of the maximum value accross entire domain.
 */
ValIndex DiscreteFunction::argmax() const
{
   ValIndex row;
   values_i.maxCoeff(&row);
   return row;
}

/**
 * Returns the linear index of the 2nd largest value.
 * Example usage:
 * <p>
 * <code>
 * ValIndex mx1 = fun.argmax();
 * ValIndex mx2 = fun.argmax2(mx1);
 * </code>
 * </p>
 * @param mxInd the value returned by DiscreteFunction::argmax()
 * This is the maximum of the set of values, excluding the largest one,
 * returned by DiscreteFunction::argmax
 */
ValIndex DiscreteFunction::argmax2(const ValIndex mxInd) const
{
   ValType mx2Val = -std::numeric_limits<ValType>::max();
   ValIndex mx2Ind = 0;

   for(ValIndex it=0; it<domainSize(); ++it)
   {
      if(mxInd==it) // skip max to find largest of rest.
      {
         continue;
      }

      ValType curVal = at(it);
      if(curVal>mx2Val)
      {
         mx2Val = curVal;
         mx2Ind = it;
      }
      
   } // for loop

   return mx2Ind;

} // argmax2

/**
* Returns the maxnorm for this function.
* The maxnorm is defined as the maximum absolute value of the function:
* \f[
* \|f(k)\|_{\infty} = \max_k |f(k)|
* \f]
*/
ValType DiscreteFunction::maxnorm() const
{
   ValType result = 0;
   for(int k=0; k<domainSize(); ++k)
   {
      ValType absVal = std::fabs(values_i[k]);
      if(result<absVal)
      {
         result = absVal;
      }
   }
   return result;
}

/**
* Returns the mean scalar value for function across entire domain.
*/
ValType DiscreteFunction::mean() const
{
   ValType N = domainSize();
   ValType result = 0;
   for(int k=0; k<domainSize(); ++k)
   {
      result += values_i[k] / N;
   }
   return result;
}

/**
 * Pretty prints a maxsum::DiscreteFunction
 */
std::ostream& maxsum::operator<<(std::ostream& out, const DiscreteFunction& fun)
{
   //***************************************************************************
   // For now just do something simple: iterate over domain and print values
   //***************************************************************************
   for(DomainIterator it(fun); it.hasNext(); ++it)
   {
      DomainIterator::VarList vars = it.getVars();
      DomainIterator::IndList inds = it.getSubInd();
      out << '(';
      for(int k=0; k<fun.noVars(); ++k)
      {
         if(0!=k) out << ", ";
         out << vars[k] << '=' << inds[k];
      }
      out << ") : " << fun(it.getInd()) << std::endl;
   }

   return out;

} // operator<<

/**
 * Check that two maxsum::DiscreteFunction objects have the same domain.
 * Two functions have the same domain, if they depend on the same set of
 * variables.
 * @param[in] f1 First function to compare
 * @param[in] f2 Second functions to compare
 * @returns true if both function have the same domain.
 */
bool maxsum::sameDomain(const DiscreteFunction& f1, const DiscreteFunction& f2)
{
   if(f1.noVars()!=f2.noVars())
   {
      return false;
   }

   return std::equal(f1.varBegin(),f1.varEnd(),f2.varBegin());

} // function sameDomain

/**
 * Check that two maxsum::DiscreteFunction objects are equal within a
 * specified tolerance. This function returns true if and only if, for all
 * <code>k</code>:
 * <p>
 * <code>
 * -tol < 1-f1(k)/f2(k) < tol
 * </code>
 * </p>
 * @param[in] f1 First function to compare
 * @param[in] f2 Second function to compare
 * @param[in] tol tolerance used for comparing values
 * @see maxsum::DEFAULT_VALUE_TOLERANCE
 */
bool maxsum::equalWithinTolerance
(
 const DiscreteFunction& f1,
 const DiscreteFunction& f2,
 ValType tol
)
{
   //***************************************************************************
   // Iterator over the combined domain of f1 and f2
   //***************************************************************************
   DomainIterator it(f1);
   it.addVars(f2.varBegin(),f2.varEnd());
   while(it.hasNext())
   {
      //************************************************************************
      // Check that the difference in value for the current positions are
      // equal within tolerance.
      //************************************************************************
      ValType diff = 1-f1(it)/f2(it);
      if( (diff > tol) || (diff < -tol) )
      {
         return false;
      }

      ++it;

   } // for loop

   //***************************************************************************
   // If we get to here, everything is equal within tolerance
   //***************************************************************************
   return true;

} // function equalWithinTolerance

/**
 * Check that two maxsum::DiscreteFunction objects are equal with a
 * specified tolerance, and have exactly the same domain.
 * This function returns true if and only if:
 * <p>
 * <code>
 * true == sameDomain(f1,f2) && equalWithinTolerance(f1,f2,tol)
 * </code>
 * </p>
 * @param[in] f1 First function to compare
 * @param[in] f2 Second function to compare
 * @param[in] tol tolerance used for comparing values
 * @see maxsum::DEFAULT_VALUE_TOLERANCE
 */
bool maxsum::strictlyEqualWithinTolerance
(
 const DiscreteFunction& f1,
 const DiscreteFunction& f2,
 ValType tol
)
{
   //***************************************************************************
   // Check domains are equal
   //***************************************************************************
   if(!sameDomain(f1,f2))
   {
      return false;
   }

   //***************************************************************************
   // Check that values are equal
   //***************************************************************************
   for(DomainIterator it(f1); it.hasNext(); ++it)
   {
      //************************************************************************
      // Check that the difference in value for the current positions are
      // equal within tolerance.
      //************************************************************************
      ValType diff = 1-f1(it.getInd())/f2(it.getInd());
      if( (diff > tol) || (diff < -tol) )
      {
         return false;
      }
   }

   //***************************************************************************
   // If we get to here everything is equal, so we return true
   //***************************************************************************
   return true;

} // strictlyEqualWithinTolerance

/**
 * Marginalise a maxsum::DiscreteFunction by maximisation.
 * This function reduces the domain of inFun to that of outFun by
 * maximisation, and stores the result in outFun. This behaviour is
 * equivalent to maxsum::marginal(inFun,std::max<ValType>,outFun).
 * @pre variables in domain of outFun are a subset of variables in inFun.
 * @post previous content of outFun is overwritten.
 * @post The domains of outFun and inFun remain unchanged.
 * @param[in] inFun function to marginalise
 * @param[out] outFun maxsum::DiscreteFunction in which to store result.
 * @throws maxsum::BadDomainException is the domain of outFun is not a
 * subset of inFun.
 * @see maxsum::marginal()
 * @see maxsum::minMarginal()
 * @see maxsum::meanMarginal()
 */
void maxsum::maxMarginal
(
 const DiscreteFunction& inFun,
 DiscreteFunction& outFun
)
{
   const ValType& (*pMax)(const ValType&, const ValType&) = std::max<ValType>;
   marginal(inFun,pMax,outFun);
}

/**
 * Marginal a maxsum::DiscreteFunction by minimisation.
 * This function reduces the domain of inFun to that of outFun by
 * minimisation, and stores the result in outFun. This behaviour is
 * equivalent to maxsum::marginal(inFun,std::min<ValType>,outFun).
 * @pre variables in domain of outFun are a subset of variables in inFun.
 * @post previous content of outFun is overwritten.
 * @post The domains of outFun and inFun remain unchanged.
 * @param[in] inFun function to marginalise
 * @throws maxsum::BadDomainException is the domain of outFun is not a
 * subset of inFun.
 * @param[out] outFun maxsum::DiscreteFunction in which to store result.
 * @see maxsum::marginal()
 * @see maxsum::maxMarginal()
 * @see maxsum::meanMarginal()
 */
void maxsum::minMarginal
(
 const DiscreteFunction& inFun,
 DiscreteFunction& outFun
)
{
   const ValType& (*pMin)(const ValType&, const ValType&) = std::min<ValType>;
   marginal(inFun,pMin,outFun);
}

/**
 * Marginalise a maxsum::DiscreteFunction by averaging.
 * This function reduces the domain of inFun to that of outFun by
 * averaging, and stores the result in outFun.
 * @pre variables in domain of outFun are a subset of variables in inFun.
 * @post previous content of outFun is overwritten.
 * @post The domains of outFun and inFun remain unchanged.
 * @param[in] inFun function to marginalise
 * @param[out] outFun maxsum::DiscreteFunction in which to store result.
 * @throws maxsum::BadDomainException is the domain of outFun is not a
 * subset of inFun.
 * @see maxsum::marginal()
 * @see maxsum::minMarginal()
 * @see maxsum::maxMarginal()
 */
void maxsum::meanMarginal
(
 const DiscreteFunction& inFun,
 DiscreteFunction& outFun
)
{
   //**************************************************************************
   // Marginalise over free domain by summation
   //**************************************************************************
   marginal(inFun,add_m,outFun);

   //**************************************************************************
   // Now each element of the output is the sum over the relevant values
   // in the input function, so we need to normalise to get the average.
   //
   // Note: another way to do this would be to marginalise using a functor
   // class Mean, which is constructed using the size of the free domain, so
   // that it knows the number of values to average over. This might be
   // more numerically stable, and also only requires one pass rather than 
   // two. However, its not clear that the extra complexity would be worth it.
   //**************************************************************************
   ValType w = outFun.domainSize();
   w = w / inFun.domainSize();
   for(int k=0; k<outFun.domainSize(); ++k)
   {
      outFun(k) *= w;
   }

} // meanMarginal

