/**
 * @file PostOffice.h
 * This file defines the utility maxsum::PostOffice class.
 * @todo Move PostOffice to util namespace as well - we don't want to be
 * making guarrantees about its interface (or indeed any interface?) after
 * release.
 */
#ifndef MAXSUM_UTIL_POSTOFFICE_H
#define MAXSUM_UTIL_POSTOFFICE_H

#include <map>
#include <set>
#include <queue>
#include "DiscreteFunction.h"
#include "util_containers.h"

namespace maxsum
{
namespace util
{
   /**
    * Class used to store and manage messages sent between factor graph nodes.
    * @tparam Sender Type used to uniquely identify message senders, e.g.
    * maxsum::FactorID or maxsum::VarID
    * @tparam Receiver Type used to uniquely identify message receivers, e.g.
    * maxsum::FactorID or maxsum::VarID
    * @tparam Message Class used to represent messages which, by default, are
    * maxsum::DiscreteFunction objects.
    */
   template<class Sender, class Receiver, class Message=DiscreteFunction>
   class PostOffice
   {
   public:

      /**
       * Map of Receivers to Messages for a specific Sender.
       */
      typedef util::RefMap<Receiver,Message*> OutMsgMap;

      /**
       * Iterator type for PostOffice::OutMsgMap.
       */
      typedef typename OutMsgMap::const_iterator OutMsgIt;

      /**
       * Map of Senders to Messages for a specific Receivers.
       */
      typedef util::RefMap<Sender,Message*> InMsgMap;

      /**
       * Iterator type for PostOffice::InMsgMap.
       */
      typedef typename InMsgMap::const_iterator InMsgIt;

   private:

      /**
       * Map of Receivers to Messages for a specific Sender.
       */
      typedef std::map<Receiver,Message*> PrivOutMsgMap;

      /**
       * Iterator type for PostOffice::OutMsgMap.
       */
      typedef typename PrivOutMsgMap::iterator PrivOutMsgIt;

      /**
       * Map of Senders to Messages for a specific Receivers.
       */
      typedef std::map<Sender,Message*> PrivInMsgMap;

      /**
       * Iterator type for PostOffice::InMsgMap.
       */
      typedef typename PrivInMsgMap::iterator PrivInMsgIt;


      /**
       * Convenience typedef for outboxes_i map type.
       */
      typedef std::map<Sender,PrivOutMsgMap> OutboxMap;

      /**
       * Convenience typedef for inboxes_i map type.
       */
      typedef std::map<Receiver,PrivInMsgMap> InboxMap;

   public:

      /**
       * Type of iterator returned by PostOffice::senderBegin() and
       * PostOffice::senderEnd().
       * This type of iterator allows read-only access to the set of Senders
       * registered with this maxsum::PostOffice.
       * @see PostOffice::senderBegin
       * @see PostOffice::senderEnd
       */
      typedef typename util::KeySet<OutboxMap>::const_iterator SenderIt;

      /**
       * Type of iterator returned by PostOffice::receiverBegin and
       * PostOffice::receiverEnd.
       * This type of iterator allows read-only access to the set of Receivers
       * registered with this maxsum::PostOffice.
       * @see PostOffice::receiverBegin
       * @see PostOffice::receiverEnd
       */
      typedef typename util::KeySet<InboxMap>::const_iterator ReceiverIt;

   private:

      /**
       * Current outboxes indexed by sender
       */
      OutboxMap curOutboxes_i;

      /**
       * Previous outboxes indexed by sender.
       */
      OutboxMap prevOutboxes_i;

      /**
       * Current inboxes indexed by receiver.
       */
      InboxMap curInboxes_i;

      /**
       * Previous inboxes indexed by receiver.
       */
      InboxMap prevInboxes_i;

      /**
       * KeySet provided interface to the set of senders.
       */
      util::KeySet<OutboxMap> senders_i;

      /**
       * KeySet provided interface to the set of senders.
       */
      util::KeySet<InboxMap> receivers_i; 

      /**
       * Queue of receivers who currently have new mail.
       */
      std::queue<Receiver> notices_i;
      
      /**
       * Utility function used for deep copy construction and assignment.
       * Makes sure that all pointers point internally and that nothing is
       * shared. This function should only be called for copy functions, and
       * for no other reason.
       */
      void deepCopyMembers()
      {
         //*********************************************************************
         // Deep copy current messages and update both inbox and outbox
         // references.
         //*********************************************************************
         for(typename OutboxMap::iterator boxIt=curOutboxes_i.begin();
             boxIt!=curOutboxes_i.end(); ++boxIt)
         {
            Sender s = boxIt->first;
            
            for(PrivOutMsgIt msgIt=boxIt->second.begin();
                msgIt!=boxIt->second.end(); ++msgIt)
            {
               //***************************************************************
               // Copy outbox message
               //***************************************************************
               Receiver r = msgIt->first;
               Message* pMsgCopy = new Message(*(msgIt->second));
               msgIt->second = pMsgCopy;
               
               //***************************************************************
               // Update corresponding inbox message to point to the same thing
               //***************************************************************
               curInboxes_i[r][s] = pMsgCopy;
               
            }
            
         } // outer for loop
         
         //*********************************************************************
         // Deep copy previous messages and update both inbox and outbox
         // references
         //*********************************************************************
         for(typename OutboxMap::iterator boxIt=prevOutboxes_i.begin();
             boxIt!=prevOutboxes_i.end(); ++boxIt)
         {
            Sender s = boxIt->first;
            
            for(PrivOutMsgIt msgIt=boxIt->second.begin();
                msgIt!=boxIt->second.end(); ++msgIt)
            {
               //***************************************************************
               // Copy outbox message
               //***************************************************************
               Receiver r = msgIt->first;
               Message* pMsgCopy = new Message(*(msgIt->second));
               msgIt->second = pMsgCopy;
               
               //***************************************************************
               // Update corresponding inbox message to point to the same thing
               //***************************************************************
               prevInboxes_i[r][s] = pMsgCopy;
            }
            
         } // outer for loop
         
         //*********************************************************************
         // Make sure key sets point to our own updated maps
         //*********************************************************************
         senders_i.setMap(&curOutboxes_i);
         receivers_i.setMap(&curInboxes_i);
         
      } // function deepCopyMembers()

   public:

      /**
       * Constructs an empty PostOffice with no edges, senders or receivers.
       */
      PostOffice()
         : curOutboxes_i(), prevOutboxes_i(),
           curInboxes_i(), prevInboxes_i(),
           senders_i(&curOutboxes_i), receivers_i(&curInboxes_i),
           notices_i()
      {}
      
      /**
       * Deep copy constructor
       */
      PostOffice(const PostOffice& rhs)
      :  curOutboxes_i(rhs.curOutboxes_i),
         prevOutboxes_i(rhs.prevOutboxes_i),
         curInboxes_i(rhs.curInboxes_i),
         prevInboxes_i(rhs.prevInboxes_i),
         senders_i(&curOutboxes_i),
         receivers_i(&curInboxes_i),
         notices_i(rhs.notices_i)
      {
         deepCopyMembers();
      }
      
      /**
       * Deep copy assignment.
       */
      PostOffice& operator=(const PostOffice& rhs)
      {
         curOutboxes_i = rhs.curOutboxes_i;
         prevOutboxes_i = rhs.prevOutboxes_i;
         curInboxes_i = rhs.curInboxes_i;
         prevInboxes_i = rhs.prevInboxes_i;
         senders_i.setMap(&curOutboxes_i);
         receivers_i.setMap(&curInboxes_i);
         notices_i = rhs.notices_i;
         deepCopyMembers();
         return *this;
      }

      /**
       * Removes all messages and edges from this postoffice.
       */
      void clear()
      {
         curOutboxes_i.clear();
         prevOutboxes_i.clear();
         curInboxes_i.clear();
         prevInboxes_i.clear();
         while(!notices_i.empty())
         {
            notices_i.pop();
         }
      }

      /**
       * Returns the current set of output messages for a given sender.
       * @throws maxsum::UnknownAddressException if sender is not registered.
       */
      OutMsgMap curOutMsgs(Sender s)
      {
         typename OutboxMap::iterator pos = curOutboxes_i.find(s);
         if(curOutboxes_i.end()==pos)
         {
            throw UnknownAddressException("curOutMsgs","Unknown sender.");
         }
         return OutMsgMap(pos->second);
      }

      /**
       * Returns the previous set of output messages for a given sender.
       * @throws maxsum::UnknownAddressException if sender is not registered.
       */
      OutMsgMap prevOutMsgs(Sender s)
      {
         typename OutboxMap::iterator pos = prevOutboxes_i.find(s);
         if(prevOutboxes_i.end()==pos)
         {
            throw UnknownAddressException("prevOutMsgs","Unknown sender.");
         }
         return OutMsgMap(pos->second);
      }

      /**
       * Returns the current set of input messages for a given receiver.
       * @throws maxsum::UnknownAddressException if receiver is not registered.
       */
      InMsgMap curInMsgs(Receiver r)
      {
         typename InboxMap::iterator pos = curInboxes_i.find(r);
         if(curInboxes_i.end()==pos)
         {
            throw UnknownAddressException("curInMsgs","Unknown receiver.");
         }
         return InMsgMap(pos->second);
      }

      /**
       * Returns the previous set of input messages for a given receiver.
       * @throws maxsum::UnknownAddressException if receiver is not registered.
       */
      InMsgMap prevInMsgs(Receiver r)
      {
         typename InboxMap::iterator pos = prevInboxes_i.find(r);
         if(prevInboxes_i.end()==pos)
         {
            throw UnknownAddressException("prevInMsgs","Unknown receiver.");
         }
         return InMsgMap(pos->second);
      }

      /**
       * Returns true if any receivers have new mail.
       */
      bool newMail() const { return !notices_i.empty(); }

      /**
       * Notifies a receiver that they have new mail.
       * @param[in] r the receiver to notify
       */
      void notify(Receiver r) { notices_i.push(r); }

      /**
       * Notifies all receivers that they have new mail.
       */
      void notifyAll()
      {
         while(!notices_i.empty())
         {
            notices_i.pop();
         }

         typedef typename util::KeySet<InboxMap>::const_iterator Iterator;
         for(Iterator it=receivers_i.begin(); it!=receivers_i.end(); ++it)
         {
            notify(*it);
         }
      }

      /**
       * Returns the number of receivers who are currently notified to check
       * their mail.
       */
      int noticeCount() const
      {
         return notices_i.size();
      }

      /**
       * Returns the identity of the next receiver who has new mail, and
       * removes their identity from the notification list.
       * @pre PostOffice::newmail returns true
       * @pre PostOffice::noticeCount returns integer greater than 0.
       * @post the size of the notification list is reduced by 1.
       * @throws maxsum::EmptyNoticeException if PostOffice::newMail returns
       * false;
       */
      Receiver popNotice()
      {
         if(notices_i.empty())
         {
            throw EmptyNoticeException("popNotice()",
                  "Tried to pop from empty notice queue.");
         }
         Receiver result = notices_i.front();
         notices_i.pop();
         return result;
      }

      /**
       * Swaps the current messages from a specified sender with their previous
       * ones. This allows the previous messages to be overwritten with new
       * messages, without having to free of allocate temporary objects.
       */
      void swapOutBoxes(Sender s)
      {
         //*********************************************************************
         // If this sender is not registered, then there is nothing to do
         //*********************************************************************
         typename OutboxMap::iterator curOutPos = curOutboxes_i.find(s);
         if(curOutboxes_i.end()==curOutPos)
         {
            return;
         }

         //*********************************************************************
         // Otherwise swap the list of outboxes for this sender in one go.
         //*********************************************************************
         prevOutboxes_i[s].swap(curOutPos->second);

         //*********************************************************************
         // Inbox pointers need the be swapped individually
         //*********************************************************************
         for(typename InboxMap::iterator curIt=curInboxes_i.begin();
               curIt!=curInboxes_i.end(); ++curIt)
         {
            //******************************************************************
            // If the current receiver is not related to this sender, continue
            // to the next receiver.
            //******************************************************************
            PrivInMsgIt curMsgPos = curIt->second.find(s);
            if(curIt->second.end()==curMsgPos)
            {
               continue;
            }

            //******************************************************************
            // Otherwise swap the current and previous in message pointers.
            //******************************************************************
            Message*& prevPtr = prevInboxes_i[curIt->first][s];
            Message*&  curPtr = curMsgPos->second;
            Message*   tmpPtr = prevPtr;

            prevPtr = curPtr;
             curPtr = tmpPtr;

         } // for loop

      } // swapOutBoxes

      /**
       * Adds an edge between a specified sender and receiver.
       * @param[in] s the sender's id
       * @param[in] r the receiver's id
       * @post Messages can now be sent from <code>s</code> to <code>s</code>
       * via this maxsum::PostOffice.
       */
      void addEdge(Sender s, Receiver r)
      {
         addEdge(s,r,Message());
      }

      /**
       * Adds an edge between a specified sender and receiver.
       * @param[in] s the sender's id
       * @param[in] r the receiver's id
       * @param[in] msgVal initial value for newly constructed messages.
       * @post Messages can now be sent from <code>s</code> to <code>s</code>
       * via this maxsum::PostOffice.
       */
      void addEdge(Sender s, Receiver r, const Message& msgVal)
      {
         //*********************************************************************
         // If we have a message already, then we're done.
         //*********************************************************************
         PrivOutMsgMap& curOutMsgs = curOutboxes_i[s];
         Message*& pCurOutMsg = curOutMsgs[r];
         if(0!=pCurOutMsg)
         {
            return;
         }

         //*********************************************************************
         // Retrieve all other message pointers for this sender-receiver pair.
         //*********************************************************************
         PrivInMsgMap& curInMsgs = curInboxes_i[r];
         PrivInMsgMap& prevInMsgs = prevInboxes_i[r];
         PrivOutMsgMap& prevOutMsgs = prevOutboxes_i[s];

         Message*& pCurInMsg = curInMsgs[s];
         Message*& pPrevInMsg = prevInMsgs[s];
         Message*& pPrevOutMsg = prevOutMsgs[r];

         //*********************************************************************
         // Make sure that all message pointers for this sender receiver
         // pair are consistent.
         //*********************************************************************
         assert(pCurInMsg==pCurOutMsg);
         assert(pPrevInMsg==pPrevOutMsg);
         assert( (0==pCurInMsg) == (pCurInMsg==pPrevInMsg) );
         assert( (0==pCurOutMsg) == (pCurOutMsg==pPrevOutMsg) );

         //*********************************************************************
         // Otherwise create new current and previous messages and stick
         // then in the relevent lists.
         //*********************************************************************
         pCurOutMsg = new Message(msgVal);
         pPrevOutMsg = new Message(msgVal);
         pCurInMsg = pCurOutMsg;
         pPrevInMsg = pPrevOutMsg;

      } // addEdge

      /**
       * Removes an edge between a specified sender and receiver.
       * @param[in] s the sender's id
       * @param[in] r the receiver's id
       * @post Messages will no longer be sent from <code>s</code> to
       * <code>s</code> via this maxsum::PostOffice.
       */
      void removeEdge(Sender s, Receiver r)
      {
         //*********************************************************************
         // If the sender edge does not exist, then this edge cannot exist,
         // therefore, we're done.
         //*********************************************************************
         typename OutboxMap::iterator curOutboxPos  = curOutboxes_i.find(s);
         if(curOutboxes_i.end()==curOutboxPos)
         {
            return;
         }

         //*********************************************************************
         // If this sender exits, but is not connected to specified receiver
         // then we are also done.
         //*********************************************************************
         PrivOutMsgMap& curOutMsgs = curOutboxPos->second;
         PrivOutMsgIt curOutMsgPos = curOutMsgs.find(r);
         if(curOutMsgs.end()==curOutMsgPos)
         {
            return;
         }

         //*********************************************************************
         // Free allocated memory for the current and previous messages.
         // Only need to do this for outboxes, because inboxes point to the
         // same messages.
         //*********************************************************************
         PrivOutMsgMap& prevOutMsgs = prevOutboxes_i[s];
         Message*& pCurOutMsg = curOutMsgPos->second;
         PrivOutMsgIt prevOutMsgPos = prevOutMsgs.find(r);
         assert(prevOutMsgs.end()!=prevOutMsgPos); // should never happen.
         Message*& pPrevOutMsg = prevOutMsgPos->second;
         delete pCurOutMsg;
         delete pPrevOutMsg;
         pCurOutMsg=0;
         pPrevOutMsg=0;

         //*********************************************************************
         // Remove all relevant map enteries.
         // For efficiency, we use the outbox iterators because we already
         // have them.
         //*********************************************************************
         curOutMsgs.erase(curOutMsgPos);
         prevOutMsgs.erase(prevOutMsgPos);
         curInboxes_i[r].erase(s);
         prevInboxes_i[r].erase(s);

         //*********************************************************************
         // If the sender no longer has any edges, delete it.
         //*********************************************************************
         if(curOutboxes_i[s].empty())
         {
            curOutboxes_i.erase(s);
         }

         //*********************************************************************
         // Likewise, if the receiver no longer has any edges, delete it too.
         //*********************************************************************
         if(curInboxes_i[r].empty())
         {
            curInboxes_i.erase(r);
         }

      } // removeEdge

      /**
       * Returns true if a specified sender is registered with this postoffice.
       */
      bool hasSender(Sender id) const
      {
         return senders_i.contains(id);
      }

      /**
       * Returns true if a specified receiver is registered with this
       * postoffice.
       */
      bool hasReceiver(Receiver id) const
      {
         return receivers_i.contains(id);
      }

      /**
       * Returns true if a specified sender and receiver are connected.
       * @param[in] s the sender's id
       * @param[in] r the receiver's id
       * @returns true if <code>s</code> can send mail to <code>r</code> via
       * this maxsum::PostOffice.
       */
      bool hasEdge(Sender s, Receiver r) const
      {
         //*********************************************************************
         // If the sender edge does not exist, then this edge cannot exist,
         // therefore, we're done.
         //*********************************************************************
         typename OutboxMap::const_iterator curOutboxPos =curOutboxes_i.find(s);
         if(curOutboxes_i.end()==curOutboxPos)
         {
            return false;
         }

         //*********************************************************************
         // If this sender exits, but is not connected to specified receiver
         // then we are also done.
         //*********************************************************************
         const PrivOutMsgMap& curOutMsgs = curOutboxPos->second;
         typename PrivOutMsgMap::const_iterator curOutMsgPos
            = curOutMsgs.find(r);

         if(curOutMsgs.end()==curOutMsgPos)
         {
            return false;
         }

         //*********************************************************************
         // If we get this far, the edge must exist, so we return true.
         //*********************************************************************
         return true;

      } // hasEdge

      /**
       * Returns the number of edges (routes between sender's and receivers)
       * registered in this PostOffice.
       */
      int numOfEdges() const
      {
         //*********************************************************************
         // Count the number of edges for each sender
         //*********************************************************************
         int edgeCount = 0;
         for(typename OutboxMap::const_iterator it=curOutboxes_i.begin();
               it!=curOutboxes_i.end(); ++it)
         {
            edgeCount += it->second.size();
         }
         return edgeCount;

      } // numOfEdges

      /**
       * Returns the number of Sender's registered with this PostOffice.
       */
      int numOfSenders() const { return senders_i.size(); }

      /**
       * Returns the number of Receiver's registered with this PostOffice.
       */
      int numOfReceivers() const { return receivers_i.size(); }

      /**
       * Returns iterator to the beginning of the Sender set.
       * Together with PostOffice::senderEnd this provides access to the set of
       * all senders.
       * Example usage:
       * <pre>
       * PostOffice<VarID,FactorID> post(...);
       * ...
       * typedef PostOffice<VarID,FactorID>::SenderIt Iterator;
       * for(Iterator it=post.senderBegin(); it!=post.senderEnd(); ++it)
       * {
       *    std::cout << "Sender " << *it << " is registered.\n";
       * }
       * </pre>
       * @see PostOffice::senderEnd
       */
      SenderIt senderBegin() const
      {
         return senders_i.begin();
      }

      /**
       * Returns iterator to the beginning of the Sender set.
       * Together with PostOffice::senderBegin this provides access to the set
       * of all senders.
       * Example usage:
       * <pre>
       * PostOffice<VarID,FactorID> post(...);
       * ...
       * typedef PostOffice<VarID,FactorID>::SenderIt Iterator;
       * for(Iterator it=post.senderBegin(); it!=post.senderEnd(); ++it)
       * {
       *    std::cout << "Sender " << *it << " is registered.\n";
       * }
       * </pre>
       * @see PostOffice::senderBegin
       */
      SenderIt senderEnd() const
      {
         return senders_i.end();
      }

      /**
       * Returns iterator to the beginning of the Receiver set.
       * Together with PostOffice::receiverEnd this provides access to the set
       * of all receivers.
       * Example usage:
       * <pre>
       * PostOffice<VarID,FactorID> post(...);
       * ...
       * typedef PostOffice<VarID,FactorID>::ReceiverIt Iterator;
       * for(Iterator it=post.receiverBegin(); it!=post.receiverEnd(); ++it)
       * {
       *    std::cout << "Receiver " << *it << " is registered.\n";
       * }
       * </pre>
       * @see PostOffice::receiverEnd
       */
      ReceiverIt receiverBegin()
      {
         return receivers_i.begin();
      }

      /**
       * Returns iterator to the beginning of the Receiver set.
       * Together with PostOffice::receiverBegin this provides access to the set
       * of all receivers.
       * Example usage:
       * <pre>
       * PostOffice<VarID,FactorID> post(...);
       * ...
       * typedef PostOffice<VarID,FactorID>::ReceiverIt Iterator;
       * for(Iterator it=post.receiverBegin(); it!=post.receiverEnd(); ++it)
       * {
       *    std::cout << "Receiver " << *it << " is registered.\n";
       * }
       * </pre>
       * @see PostOffice::receiverBegin
       */
      ReceiverIt receiverEnd()
      {
         return receivers_i.end();
      }

      /**
       * Destructor frees all resources used by this PostOffice.
       * @post Any external pointers to messages managed by this post office
       * will be invalidated.
       */
      virtual ~PostOffice()
      {
         //********************************************************************
         // Free all current messages by iterating through the outboxes.
         // We do not need to free inboxes, because these point to the same
         // messages, which should only be freed once.
         //********************************************************************
         for(typename OutboxMap::iterator boxIt=curOutboxes_i.begin();
               boxIt!=curOutboxes_i.end(); ++boxIt)
         {
            for(PrivOutMsgIt msgIt=boxIt->second.begin();
                  msgIt!=boxIt->second.end(); ++msgIt)
            {
               delete msgIt->second;
            }

         } // outer for loop

         //********************************************************************
         // Free all previous messages by iterating through the outboxes.
         // We do not need to free inboxes, because these point to the same
         // messages, which should only be freed once.
         //********************************************************************
         for(typename OutboxMap::iterator boxIt=prevOutboxes_i.begin();
               boxIt!=prevOutboxes_i.end(); ++boxIt)
         {
            for(PrivOutMsgIt msgIt=boxIt->second.begin();
                  msgIt!=boxIt->second.end(); ++msgIt)
            {
               delete msgIt->second;
            }

         } // outer for loop

      } // destructor

   }; // class PostOffice

   /**
    * Convenience typedef for Variable to Factor PostOffices.
    */
   typedef PostOffice<VarID,FactorID> V2FPostOffice;

   /**
    * Convenience typedef for Factor to Variable PostOffices.
    */
   typedef PostOffice<FactorID,VarID> F2VPostOffice;

   /**
    * Convenience typedef for Factor to Factor PostOffices.
    */
   typedef PostOffice<FactorID,FactorID> F2FPostOffice;

} // namespace util
} // namespace maxsum



#endif // MAXSUM_UTIL_POSTOFFICE_H

