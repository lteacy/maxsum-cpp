/**
 * @file MaxSumController.cpp
 * Implementation of MaxSumController class members.
 * @see MaxSumController.h
 * @author W. T. Luke Teacy - University of Southampton
 */

#include "MaxSumController.h"

using namespace maxsum;

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
         sumOfOthers -= *curInMsgs[it->first];
         maxMarginal(sumOfOthers,*(it->second));

         //*********************************************************************
         // If the max norm threshold has been passed, tell the current 
         // neighbour that they have mail.
         //*********************************************************************
         DiscreteFunction msgDiff(*(it->second));
         msgDiff -= *prevOutMsgs[it->first];
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
         *(it->second) = msgSum;
         *(it->second) -= *curInMsgs[it->first];

         //*********************************************************************
         // Normalise the current output message
         //*********************************************************************
         *(it->second) -= it->second->mean();

         //*********************************************************************
         // If the max norm threshold has been passed, tell the current 
         // neighbour that they have mail.
         //*********************************************************************
         DiscreteFunction msgDiff(*(it->second));
         msgDiff -= *prevOutMsgs[it->first];
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
   int iterationCount = 0;
   while(iterationCount<maxIterations_i)
   {
      //************************************************************************
      // Update the number of iterations that we've performed
      //************************************************************************
      ++iterationCount;

      //************************************************************************
      // Update the factor to variable messages
      //************************************************************************
      int numOfUpdates = updateFac2VarMsgs();

      //************************************************************************
      // Update the variable to factor messages
      //************************************************************************
      numOfUpdates += updateVar2FacMsgs();

      //************************************************************************
      // If there have been no message updates since the last iteration, then
      // we've converged, so we can stop.
      //************************************************************************
      if(0==numOfUpdates)
      {
         break;
      }

   } // for loop

   //***************************************************************************
   // Return the number of iterations performed.
   //***************************************************************************
   return iterationCount;

} // optimise function

