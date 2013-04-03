/**
 * @file MaxSumController.h
 * Defines the MaxSumController class, which implements the max-sum algorithm.
 * @author W. T. Luke Teacy - University of Southampton
 */
#ifndef MAXSUM_MAXSUMCONTROLLER_H
#define MAXSUM_MAXSUMCONTROLLER_H

#include <iostream>
#include <map>
#include <stack>
#include "common.h"
#include "DiscreteFunction.h"
#include "PostOffice.h"

/**
 * Namespace for all public types and functions defined by the Max-Sum library.
 */
namespace maxsum
{
   /**
    * This class maintains a factor graph and implements the max-sum algorithm.
    * More specifically, this class implements the max-sum algorithm using the
    * factor graph regime, in which messages are passed along the edges of a 
    * factor graph consisting of factor nodes and variable (action) nodes.
    * Once the algorithm has converged, actions selected by maximising over
    * the sum of messages sent to each variable (action) node.
    */
   class MaxSumController
   {
   private:

      /**
       * Utility function used to dump the current state of this controller
       * for debugging purposes.
       */
      friend std::ostream& operator<<(std::ostream& out,
            MaxSumController& controller);

      /**
       * Type of container used to map factor's to their defining functions.
       */
      typedef std::map<FactorID,DiscreteFunction> FactorMap;

      /**
       * Map storing the functions associated with each factor under the
       * control of this object.
       */
      FactorMap factors_i;

      /**
       * Map storing the total value for each factor (the factor + the sum of
       * all its input messages. This is used if we need to calculate
       * additional things, such as the second best joint action for each
       * factor (for VPI).
       */
      FactorMap factorTotalValue_i;

      /**
       * Type of container used to map (action) variables to their currently
       * assigned values.
       */
      typedef std::map<VarID,ValIndex> ValueMap;

      /**
       * Map storing the current values of all variables in the union of
       * the function domains mapped by factors_i.
       */
      ValueMap values_i;

      /**
       * PostOffice for factor to variable messages.
       */
      util::F2VPostOffice fac2varMsgs_i;

      /**
       * PostOffice for variable to factor messages.
       */
      util::V2FPostOffice var2facMsgs_i;

      /**
       * The maximum number of max-sum iterations allowed before the
       * ::optimise() function is forced to return.
       */
      int maxIterations_i;

      /**
       * Maximum difference between message values at which we consider the
       * algorithm to have converged.
       */
      ValType maxNormThreshold_i;

      /**
       * Updates factor to variable messages.
       * This function only needs to update messages that have changed
       * significantly since the last iteration. A significant change
       * is one which exceeds the maxNormThreshold_i or results in a
       * change in variable assignment.
       * @returns the number of updated messages.
       */
      int updateFac2VarMsgs();

      /**
       * Updates variable to factor messages.
       * This function only needs to update messages that have changed
       * significantly since the last iteration. A significant change
       * is one which exceeds the maxNormThreshold_i or results in a
       * change in variable assignment.
       * @returns the number of updated messages.
       */
      int updateVar2FacMsgs();

      /**
       * Infer the factor graph from the given factor domains.
       */
      void inferGraph();

   public:

      /**
       * Read only iterator for all variable value assigments.
       */
      typedef ValueMap::const_iterator ConstValueIterator;

      /**
       * Read only iterator for all factors.
       */
      typedef FactorMap::const_iterator ConstFactorIterator;
      
      /**
       * Default maximum number of iterations for max-sum algorithm.
       */
      static const int DEFAULT_MAX_ITERATIONS=100;

      /**
       * Default maximum maxnorm allowed between the old and new values
       * of a message, before it is assumed to have converged.
       */
      static const ValType DEFAULT_MAXNORM_THRESHOLD;

      /**
       * Construct a new maxsum::MaxSumController.
       * @param[in] maxIterations The maximum number of iterations for the
       * max-sum algorithm.
       * @param[in] maxnorm The maximum maxnorm allowed between the old and new
       * values of a message, before it is assumed to have converged.
       */
      MaxSumController
      (
       int maxIterations=DEFAULT_MAX_ITERATIONS,
       ValType maxnorm=DEFAULT_MAXNORM_THRESHOLD
      )
      : maxIterations_i(maxIterations),
        maxNormThreshold_i(maxnorm) {}

      /**
       * Returns the total value for a specified factor.
       * The total value is the factor plus the sum of all its received messages.
       * @pre the optimise method must be called first to properly initialise
       * this value.
       * @returns the total value for this factor.
       */
      const DiscreteFunction& getTotalValue(FactorID fac) const
      {
         FactorMap::const_iterator pos = factorTotalValue_i.find(fac);
         if(factors_i.end()==pos)
         {
            throw new NoSuchElementException("MaxSumController::getFactor()",
                  "No such factor in factor graph.");
         }
         return pos->second;
      }

      /**
       * Accessor method for factor function.
       * @param[in] id the unique identifier of the desired factor.
       * @param[in] factor the function representing this factor.
       * @returns a reference to the function associated with the factor with
       * unique identifier <code>id</code>.
       * @post A copy of <code>factor</code> is stored internally by
       * this maxsum::MaxSumController and used to form part of a factor graph.
       * @post Any previous value of the specified factor is overwritten.
       */
      void setFactor(FactorID id, const DiscreteFunction& factor);

      /**
       * Removes the specified factor from this controller's factor graph.
       * In addition, any variables that were previously only connected to this
       * factor, will also be removed from the factor graph. Notice that this
       * does not require the remaining nodes to form a connected graph, but
       * it does mean that variables or factors connect exist in isolation.
       * For example, although every variable and factor must have at least one
       * edge, two variable-factor graph pairs may exist without any edges
       * between the pairs.
       * @param[in] id the id of the factor to remove from the graph.
       * @post If the specified factor was previously in the factor graph,
       * it will be removed by this function.
       * @post Any variables that were previously only connected to this
       * factor, will also be removed from the factor graph.
       */
      void removeFactor(FactorID id);

      /**
       * Clear all factors and variables to form an empty factor graph.
       * @post the state of this maxsum::MaxSumController is reset to that
       * created by the default constructor, with no registered factors or
       * variables.
       */
      void clear();

      /**
       * Returns true if and only if the specified factor is managed by this
       * maxsum::MaxSumController.
       * @returns true if and only if the specified factor is managed by this
       * maxsum::MaxSumController.
       */
      bool hasFactor(FactorID id) const
      {
         return 0!=factors_i.count(id);;
      }

      /**
       * Returns true if and only if the specified variable, <code>var</code>
       * and factor, <code>fac</code> are
       * known to this maxsum::MaxSumController, and <code>var</code> is in the
       * domain of <code>fac</code>.
       * @returns true if and only if the specified variable, <code>var</code>
       * and factor, <code>fac</code> are
       * known to this maxsum::MaxSumController, and <code>var</code> is in the
       * domain of <code>fac</code>.
       */
      bool hasEdge(FactorID id, VarID var) const
      {
         return var2facMsgs_i.hasEdge(var,id);
      }

      /**
       * Returns the number of factors know to this factor graph.
       */
      int noFactors() const
      {
         return factors_i.size();
      }
      
      /**
       * Returns the number of variables known to this factor graph.
       */
      int noVars() const
      {
         return var2facMsgs_i.numOfSenders();;
      }

      /**
       * Returns the number of edges in the factor graph.
       */
      int noEdges() const
      {
         return var2facMsgs_i.numOfEdges();;
      }

      /**
       * Returns a read only iterator to the beginning of the 
       * variable value map.
       */
      ConstValueIterator valBegin() const
      {
         return values_i.begin();
      }

      /**
       * Returns a read only iterator to the end of the variable value map.
       */
      ConstValueIterator valEnd() const
      {
         return values_i.end();
      }

      /**
       * Returns a read only iterator to the beginning of the factor map.
       */
      ConstFactorIterator factorBegin() const
      {
         return factors_i.begin();
      }

      /**
       * Returns a read only iterator to the end of the factor map.
       */
      ConstFactorIterator factorEnd() const
      {
         return factors_i.end();
      }

      /**
       * Accessor method for factor function.
       * @param[in] id the unique identifier of the desired factor.
       * @returns a reference to the function associated with the factor with
       * unique identifier <code>id</code>.
       * @throws maxsum::NoSuchElementException if the specified factor is not
       * known to this maxsum::MaxSumController.
       */
      const DiscreteFunction& getFactor(FactorID id) const
      {
         FactorMap::const_iterator pos = factors_i.find(id);
         if(factors_i.end()==pos)
         {
            throw new NoSuchElementException("MaxSumController::getFactor()",
                  "No such factor in factor graph.");
         }
         return pos->second;
      }

      /**
       * Accessor method for (writable) reference to factor function.
       * This version returns a writable reference to a function to allow
       * efficient updates to the function values. Note this is an UNSAFE
       * procedure, because the MaxSumController will not be notified if
       * any change is made to function's domain through this handle, which
       * in turn will change the factor graph. 
       * 
       * In future, we may modify this function to unsure that no unexpected
       * domain changes are made, but for now, this allows us to bypass all
       * the redundant copying and checking that is done by
       * MaxSumController::setFactor. If you need a safe procedure use that
       * function instead.
       *
       * Also, if the factor is modified in anyway through the returned
       * reference, the MaxSumController must be notified by calling
       * MaxSumController::notifyFactorChange.
       *
       * @param[in] id the unique identifier of the desired factor.
       * @returns a reference to the function associated with the factor with
       * unique identifier <code>id</code>.
       * @throws maxsum::NoSuchElementException if the specified factor is not
       * known to this maxsum::MaxSumController.
       * @pre a factor with this id must already be in the factor graph.
       * @post the caller must ensure that the domain of the returned function
       * is not changed, and so promise only to modify its values. Any change
       * in value must be flagged to the MaxSumController by calling 
       * MaxSumController::notifyFactorChange.
       */
      DiscreteFunction& getUnSafeWritableFactorHandle(FactorID id) 
      {
         FactorMap::iterator pos = factors_i.find(id);
         if(factors_i.end()==pos)
         {
            throw new NoSuchElementException("MaxSumController::getFactor()",
                  "No such factor in factor graph.");
         }
         return pos->second;
      }

      /**
       * Function used to notify this MaxSumController of any changes made
       * to a factor without its knowledge.
       * This function should be called after any change to a factor
       * made using MaxSumController::getUnSafeWritableFactorHandle
       * @param id the id of the changed factor
       */
      void notifyFactor(FactorID id)
      {
         var2facMsgs_i.notify(id);
      }

      /**
       * Returns true if and only if the specified variable is in the domain
       * of at least one of the factors managed by this maxsum::MaxSumController.
       * @returns true if and only if the specified variable is in the domain
       * of at least one of the factors managed by this maxsum::MaxSumController.
       */
      bool hasValue(VarID id) const
      {
         return 0!=values_i.count(id);
      }

      /**
       * Returns the current value assigned to the specified variable.
       * This value is only optimised if MaxSumController::optimise has been
       * previously called, and no factor has been modified since it was last
       * called.
       * @param[in] id the unique identifier of the desired variable.
       * @returns the current value assigned to the specified variable.
       * @throws maxsum::NoSuchElementException if the specified value is not
       * in the domain of any of the factors managed by this
       * maxsum::MaxSumController.
       */
      ValType getValue(VarID id) const
      {
         ValueMap::const_iterator pos = values_i.find(id);
         if(values_i.end()==pos)
         {
            throw new NoSuchElementException("MaxSumController::getValue()",
                  "No such variable in factor graph.");
         }
         return pos->second;
      }

      /**
       * Runs the max-sum algorithm to optimise the values for each variable.
       * @post ::getValue(VarID id) will return the optimal value for the
       * the variable with unique identifier <code>id</code>.
       * @returns the number of max-sum iterations performed.
       */
      int optimise();

   }; // MaxSumController class

   /**
    * Default maximum maxnorm allowed between the old and new values
    * of a message, before it is assumed to have converged.
    */
   const ValType MaxSumController::DEFAULT_MAXNORM_THRESHOLD=0.0000001;

   /**
    * Utility function used to dump the current state of this controller
    * for debugging purposes.
    */
   std::ostream& operator<<(std::ostream& out,
         MaxSumController& controller);

} // maxsum


#endif // MAXSUM_MAXSUMCONTROLLER_H
