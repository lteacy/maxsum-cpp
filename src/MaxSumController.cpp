/**
 * @file MaxSumController.cpp
 * Implementation of MaxSumController class members.
 * @see MaxSumController.h
 * @author W. T. Luke Teacy - University of Southampton
 */

#include "MaxSumController.h"
#include <iostream>

using namespace maxsum;

/**
 * Utility function used to dump the current state of this controller
 * for debugging purposes.
 */
std::ostream& maxsum::operator<<(std::ostream& out,
                  const MaxSumController& controller)
{
   return out;

} // maxsum::operator<<

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
void MaxSumController::setFactor(FactorID id, const DiscreteFunction& factor)
{
   //***************************************************************************
   // Set the specified factor. (Note: oldValue is created automatically if
   // necessary).
   //***************************************************************************
   DiscreteFunction& oldValue = factors_i[id];

   //***************************************************************************
   // If this factor is currently related to any variables that it is no
   // longer related to, delete the appropriate edges.
   //***************************************************************************
   std::vector<VarID> toRemove(oldValue.noVars());
   std::set_difference(oldValue.varBegin(),oldValue.varEnd(),
         factor.varBegin(),factor.varEnd(),toRemove.begin());

   for(std::vector<VarID>::const_iterator it=toRemove.begin();
         it!=toRemove.end(); ++it)
   {
      //************************************************************************
      // Remove the redundant edges from the postoffice graphs
      //************************************************************************
      fac2varMsgs_i.removeEdge(id,*it);
      var2facMsgs_i.removeEdge(*it,id);

      //************************************************************************
      // If the variable is no longer related to any factors, then we remove
      // it from the action list.
      //************************************************************************
      if(!var2facMsgs_i.hasSender(*it))
      {
         actions_i.erase(*it);
      }

   } // for loop

   //***************************************************************************
   // For each variable in this factor's domain
   //***************************************************************************
   for(DiscreteFunction::VarIterator it=factor.varBegin();
         it!=factor.varEnd(); ++it)
   {
      //************************************************************************
      // Initialise input and output messages between the factor and
      // the current variable
      //************************************************************************
      DiscreteFunction msgTemplate(*it,0);
      fac2varMsgs_i.addEdge(id,*it,msgTemplate);
      var2facMsgs_i.addEdge(*it,id,msgTemplate);

      //************************************************************************
      // Touch the variable to ensure that it is in the action list.
      //************************************************************************
      ActionMap::iterator pos = actions_i.find(*it);
      if(actions_i.end()==pos)
      {
         actions_i[*it]=0;
      }

   } // for loop

   //***************************************************************************
   // Set the specified factor to its new value.
   //***************************************************************************
   factors_i[id] = factor;

   //***************************************************************************
   // Tell all factors and variables to recheck their mail.
   //***************************************************************************
   var2facMsgs_i.notifyAll();
   fac2varMsgs_i.notifyAll();

} // function setFactor

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
void MaxSumController::removeFactor(FactorID id)
{
   //***************************************************************************
   // If the specified factor is not in the factor graph, then we are done.
   //***************************************************************************
   FactorMap::iterator facPos = factors_i.find(id);
   if(factors_i.end()==facPos)
   {
      return;
   }
   DiscreteFunction& factor = facPos->second;

   //***************************************************************************
   // Otherwise, for each variable in this factor's domain
   //***************************************************************************
   for(DiscreteFunction::VarIterator it=factor.varBegin();
         it!=factor.varEnd(); ++it)
   {
      //************************************************************************
      // Delete the input and output messages.
      //************************************************************************
      fac2varMsgs_i.removeEdge(id,*it);
      var2facMsgs_i.removeEdge(*it,id);

      //************************************************************************
      // If the variable is no longer related to any factors, then we remove
      // it from the action list.
      //************************************************************************
      if(!var2facMsgs_i.hasSender(*it))
      {
         actions_i.erase(*it);
      }

   } // for loop

   //***************************************************************************
   // Finally, we delete the factor from the factors_i map
   //***************************************************************************
   factors_i.erase(facPos);

   //***************************************************************************
   // Tell all factors and variables to recheck their mail.
   //***************************************************************************
   var2facMsgs_i.notifyAll();
   fac2varMsgs_i.notifyAll();

} // function removeFactor

/**
 * Clear all factors and variables to form an empty factor graph.
 * @post the state of this maxsum::MaxSumController is reset to that
 * created by the default constructor, with no registered factors or
 * variables.
 */
 void MaxSumController::clear()
{
   //***************************************************************************
   // Clear all data structures.
   //***************************************************************************
   factors_i.clear();
   actions_i.clear();
   fac2varMsgs_i.clear();
   var2facMsgs_i.clear();

} // function clear

/**
 * Infer the factor graph from the given factor domains.
 * This function populates all member variables of MaxSumController by
 * examining the domain of each factor in factors_i
 */
void MaxSumController::inferGraph()
{
   //***************************************************************************
   // For each factor
   //***************************************************************************
   for(FactorMap::const_iterator fIt = factors_i.begin();
         fIt != factors_i.end(); ++fIt)
   {
      //************************************************************************
      // Get the function that defines this factor
      //************************************************************************
      const DiscreteFunction& curFunc = fIt->second;

      //************************************************************************
      // For each variable in the current factor's domain.
      //************************************************************************
      for(DiscreteFunction::VarIterator vIt = curFunc.varBegin();
            vIt != curFunc.varEnd(); ++vIt)
      {
         //*********************************************************************
         // Ensure that that variable's value is present in the actions list,
         // and initialised to zero.
         //*********************************************************************
         actions_i[*vIt] = 0;

         //*********************************************************************
         // Add an edge between the current factor and variable to the 
         // post office structures. This ensures that messages will flow
         // between connected factors and variables when the optimise()
         // function is next invoked.
         //*********************************************************************
         var2facMsgs_i.addEdge(*vIt,fIt->first);
         fac2varMsgs_i.addEdge(fIt->first,*vIt);

      } // inner for loop

   } // outer for loop

} // inferGraph

/**
 * Updates factor to variable messages.
 * This function only needs to update messages that have changed
 * significantly since the last iteration. A significant change
 * is one which exceeds the maxNormThreshold_i or results in a
 * change in variable assignment.
 * @returns the number of updated messages.
 */
int MaxSumController::updateFac2VarMsgs()
{
   //***************************************************************************
   // For each factor with a non-empty inbox
   //***************************************************************************
   while(var2facMsgs_i.newMail())
   {
      //************************************************************************
      // Get the next factor that has new mail.
      //************************************************************************
      FactorID fac = var2facMsgs_i.popNotice();

      //************************************************************************
      // Swap the old messages with the new ones, so that the new ones become
      // old, and the old ones become new. We can then overwrite the old ones.
      // Swapping like this prevents us having to create lots of temporary
      // objects, by reusing the old ones that we no longer need.
      //************************************************************************
      fac2varMsgs_i.swapOutBoxes(fac);

      //************************************************************************
      // Calculate the total sum of this factor and all its input messages
      //************************************************************************
      DiscreteFunction msgSum(factors_i[fac]);
      V2FPostOffice::InMsgMap curInMsgs = var2facMsgs_i.curInMsgs(fac);
      typedef V2FPostOffice::InMsgIt InMsgIt;
      for(InMsgIt it=curInMsgs.begin(); it!=curInMsgs.end(); ++it)
      {
         msgSum += *(it->second);
      }

      //************************************************************************
      // Update the output messages for each connected neighbour
      //************************************************************************
      F2VPostOffice::OutMsgMap curOutMsgs = fac2varMsgs_i.curOutMsgs(fac);
      F2VPostOffice::OutMsgMap prevOutMsgs = fac2varMsgs_i.prevOutMsgs(fac);
      typedef F2VPostOffice::OutMsgIt OutMsgIt;
      for(OutMsgIt it=curOutMsgs.begin(); it!=curOutMsgs.end(); ++it)
      {
         //*********************************************************************
         // Calculate the updated message for the current neighbour by
         // subtracting the neighbour's last message from the message sum, and
         // max marginalising.
         //*********************************************************************
         DiscreteFunction sumOfOthers(msgSum);
         DiscreteFunction& prevOutMsg = *prevOutMsgs[it->first];
         DiscreteFunction& curOutMsg = *(it->second);
         DiscreteFunction& curInMsg = *curInMsgs[it->first];
         sumOfOthers -= curInMsg;
         maxMarginal(sumOfOthers,curOutMsg);

         //*********************************************************************
         // If the max norm threshold has been passed, tell the current 
         // neighbour that they have mail.
         //*********************************************************************
         DiscreteFunction msgDiff(curOutMsg);
         msgDiff -= prevOutMsg;
         if(msgDiff.maxnorm() > maxNormThreshold_i)
         {
            fac2varMsgs_i.notify(it->first);
         }

      } // for loop

   } // while loop

   //***************************************************************************
   // Return the number of updated factor to variable messages
   //***************************************************************************
   return fac2varMsgs_i.noticeCount();

} // updateFac2VarMsgs

/**
 * Updates variable to factor messages.
 * This function only needs to update messages that have changed
 * significantly since the last iteration. A significant change
 * is one which exceeds the maxNormThreshold_i or results in a
 * change in variable assignment.
 * @post Each message is normalised so that the sum of its values is 0.
 * @returns the number of updated messages.
 */
int MaxSumController::updateVar2FacMsgs()
{
   //***************************************************************************
   // For each variable with a non-empty inbox
   //***************************************************************************
   while(fac2varMsgs_i.newMail())
   {
      //************************************************************************
      // Get the next variable that has new mail.
      //************************************************************************
      VarID var = fac2varMsgs_i.popNotice();

      //************************************************************************
      // Swap the old messages with the new ones, so that the new ones become
      // old, and the old ones become new. We can then overwrite the old ones.
      // Swapping like this prevents us having to create lots of temporary
      // objects, by reusing the old ones that we no longer need.
      //************************************************************************
      var2facMsgs_i.swapOutBoxes(var);

      //************************************************************************
      // Calculate the total sum of all input messages
      //************************************************************************
      DiscreteFunction msgSum(&var,(&var)+1,0);
      F2VPostOffice::InMsgMap curInMsgs = fac2varMsgs_i.curInMsgs(var);
      typedef F2VPostOffice::InMsgIt InMsgIt;
      for(InMsgIt it=curInMsgs.begin(); it!=curInMsgs.end(); ++it)
      {
         msgSum += *(it->second);
      }

      //************************************************************************
      // Update the output messages for each connected neighbour
      //************************************************************************
      V2FPostOffice::OutMsgMap curOutMsgs = var2facMsgs_i.curOutMsgs(var);
      V2FPostOffice::OutMsgMap prevOutMsgs = var2facMsgs_i.prevOutMsgs(var);
      typedef V2FPostOffice::OutMsgIt OutMsgIt;
      for(OutMsgIt it=curOutMsgs.begin(); it!=curOutMsgs.end(); ++it)
      {
         //*********************************************************************
         // Calculate the updated message for the current neighbour subtracting
         // the neighbour's last message from the message sum.
         //*********************************************************************
         DiscreteFunction& prevOutMsg = *prevOutMsgs[it->first];
         DiscreteFunction& curOutMsg = *(it->second);
         DiscreteFunction& curInMsg = *curInMsgs[it->first];
         curOutMsg = msgSum;
         curOutMsg -= curInMsg;

         //*********************************************************************
         // Normalise the current output message
         //*********************************************************************
         curOutMsg -= curOutMsg.mean();

         //*********************************************************************
         // If the max norm threshold has been passed, tell the current 
         // neighbour that they have mail.
         //*********************************************************************
         DiscreteFunction msgDiff(curOutMsg);
         msgDiff -= prevOutMsg;
         if(msgDiff.maxnorm() > maxNormThreshold_i)
         {
            var2facMsgs_i.notify(it->first);
         }

      } // for loop

      //************************************************************************
      // If the optimal value for this variable has changed, update its value,
      // and tell all neighbours to check their mail.
      //************************************************************************
      ValIndex& curAction = actions_i[var];
      ValIndex bestAction = msgSum.argmax();
      if(bestAction != curAction)
      {
         curAction = bestAction;
         var2facMsgs_i.notifyAll();
      }

   } // while loop

   //***************************************************************************
   // Return the number of updated variable to factor messages
   //***************************************************************************
   return var2facMsgs_i.noticeCount();

} // updateVar2FacMsgs

/**
 * Runs the max-sum algorithm to optimise the values for each variable.
 * @post MaxSumController::getValue will return the optimal value
 * for the the variable with unique identifier <code>id</code>.
 */
int MaxSumController::optimise()
{
   //***************************************************************************
   // While the algorithm has not converged, or the maximum number of
   // iterations has not been reached.
   //***************************************************************************
   std::cout << "OPTIMISING...\n";
   int iterationCount = 0;
   while(iterationCount<maxIterations_i)
   {
      //************************************************************************
      // Update the number of iterations that we've performed
      //************************************************************************
      ++iterationCount;
      std::cout << "ITERATION: " << iterationCount << std::endl;

      //************************************************************************
      // Update the factor to variable messages
      //************************************************************************
      std::cout << "UPDATING FAC2VAR MESSAGES...\n";
      int numOfUpdates = updateFac2VarMsgs();
      std::cout << *this << std::endl;

      //************************************************************************
      // Update the variable to factor messages
      //************************************************************************
      std::cout << "UPDATING VAR2FAC MESSAGES...\n";
      numOfUpdates += updateVar2FacMsgs();
      std::cout << *this << std::endl;

      //************************************************************************
      // If there have been no message updates since the last iteration, then
      // we've converged, so we can stop.
      //************************************************************************
      if(0==numOfUpdates)
      {
         break;
      }

   } // while loop
   std::cout << "DONE\n\n";

   //***************************************************************************
   // Return the number of iterations performed.
   //***************************************************************************
   return iterationCount;

} // optimise function

