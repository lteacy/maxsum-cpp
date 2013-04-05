/**
 * @file MatrixBaseAddons.h
 * Extends eigen3 base class to include supporting functionality for the
 * max-sum library. The methods defined here will be available in all eigen
 * types, including the maxsum::DiscreteFunction, which is derived from
 * these base types.
 */

/**
 * Adds a list of Functions to this one, expanding the domain if
 * necessary.
 * The result is \f$this + \sum_k funcs[k]\f$
 * @todo Make this more efficient by expanding domain in advance.
 * @returns reference to this function.
 */
template<class VecIt> Derived& add(VecIt begin, VecIt end)
{
    for(VecIt it=begin; it!=end; ++it)
    {
        (*this) += *it;
    }
    return *this;
}

/**
 * Access coefficient using scalar index.
 * This is equivalent to DiscreteFunction::operator()(maxsum::ValIndex ind)
 * but is more convenient when for use with pointers e.g.
 * <pre>
 * x->(k); // illegal
 * x->at(k); // ok
 * </pre>
 * @see DiscreteFunction::operator()(maxsum::ValIndex ind)
 */
Scalar& at(Index ind)
{
    return (*this)(ind);
}

/**
 * Access coefficient using scalar index.
 * This is equivalent to DiscreteFunction::operator()(maxsum::ValIndex ind) const
 * but is more convenient when for use with pointers e.g.
 * <pre>
 * x->(k); // illegal
 * x->at(k); // ok
 * </pre>
 * @see DiscreteFunction::operator()(maxsum::ValIndex ind) const
 */
Scalar at(Index ind) const
{
    return (*this)(ind);
}

/**
 * Returns the minimum scalar value of the function across entire domain.
 */
Scalar min() const { return array().minCoeff(); }

/**
 * Returns the maximum scalar value for function across entire domain.
 */
Scalar max() const { return array().maxCoeff(); }

/**
 * Returns the linear index of the maximum value across entire domain.
 */
Index argmax() const
{
    Index result;
    array().maxCoeff(&result);
    return result;
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
Index argmax2(Index mxInd) const
{
   //**************************************************************************
   // Setup arrays to store the max and argmax for the head and tail of this
   // array (excluding the maximum at mxInd).
   //**************************************************************************
   Array<Scalar,2,1> maxVals;
   maxVals << at(0), at(0);

   Array<Index,2,1> maxInds;
   maxInds << 0, 0;

   //**************************************************************************
   // Find the argmax for head
   //**************************************************************************
   if(0<mxInd)
   {
      maxVals(0) = head(mxInd).array().maxCoeff(&maxInds(0));
   }

   //**************************************************************************
   // Find the argmax for the tail
   //**************************************************************************
   if( (mxInd+1) < size() )
   {
      maxVals(1) = tail(size()-mxInd).array().maxCoeff(&maxInds(1));
   }

   //**************************************************************************
   // Return which ever is the maximum
   //**************************************************************************
   if(maxVals(0)>maxVals(1))
   {
      return maxInds(0);
   }
   else
   {
      return mxInd+1+maxInds(1);
   }
   
} // argmx2

/**
 * Returns the maxnorm for this function.
 * The maxnorm is defined as the maximum absolute value of the function:
 * \f[
 * \|f(k)\|_{\infty} = \max_k |f(k)|
 * \f]
 */
Scalar maxnorm() const
{
    return array().abs().maxCoeff();
}

