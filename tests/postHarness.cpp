/**
 * @file postHarness.cpp
 * Test harness for the maxsum::PostOffice class.
 */
#include "PostOffice.h"
#include <string>
#include <vector>
#include <iostream>
#include <set>

using namespace maxsum;
using namespace maxsum::util;

/**
 * Represents an edge between sender and receiver.
 */
struct Edge_m
{
   std::string sender;
   long receiver;
   Edge_m(std::string s, long r) : sender(s), receiver(r) {}
};

/**
 * Type of postoffice used for test purposes.
 * Here we purposely use key and value types that cannot be
 * implicitly converted.
 */
typedef PostOffice<std::string,long> PostOffice_m;

/**
 * Fill map with random messages.
 */
template <class Map> bool fillMap(const Map& funcMap)
{
   //***************************************************************************
   // Fill the map with random messages
   //***************************************************************************
   const ValIndex DOMAIN_SIZE = 3;
   VarID var = 0;
   typedef typename Map::const_iterator Iterator;
   for(Iterator it=funcMap.begin(); it!=funcMap.end(); ++it)
   {
      ++var;
      if(!isRegistered(var))
      {
         registerVariable(var,DOMAIN_SIZE);
      }

      if(0==it->second)
      {
         std::cout << "\nfillMap detected null pointer in message map.\n";
         return false;
      }
      
      try
      {
         *(it->second) = DiscreteFunction(var,std::rand()%1000);
      }
      catch(std::exception e)
      {
         std::cout << "\nfillMap encountered an exception.\n";
         throw e;
      }

   } // for loop

   return true;

} // function fillMap

/**
 * Checks that the key set of inMap is equal to keySet.
 */
template<class Map, class K> bool validKeys
(
 const Map& inMap,
 const std::set<K>& keySet
)
{
   if(inMap.size() != keySet.size())
   {
      return false;
   }

   for(typename Map::const_iterator it=inMap.begin();
         it!=inMap.end(); ++it)
   {
      if(keySet.end()==keySet.find(it->first))
      {
         return false;
      }
   }
   return true;

} // function validKeys

/**
 * Utility function for checking whether two maps contain exactly the
 * same values.
 */
template<class M1, class M2> bool equalMaps(const M1& map1, const M2& map2)
{
   //***************************************************************************
   // Check that both maps have the same size
   //***************************************************************************
   if(map1.size() != map2.size())
   {
      return false;
   }

   //***************************************************************************
   // Check that all pairs are exactly the same.
   //***************************************************************************
   for(typename M1::const_iterator it1=map1.begin(); it1!=map1.end(); ++it1)
   {
      typename M2::const_iterator it2=map2.find(it1->first);
      if(map2.end()==it2)
      {
         return false;
      }
      
      if(it2->second != it1->second)
      {
         return false;
      }
   }

   //***************************************************************************
   // If we get here, everything is equal, so we return true.
   //***************************************************************************
   return true;

} // function equalMaps

/**
 * Utility function for checking whether two pointer maps contain exactly the
 * same values.
 */
template<class M1, class M2> bool equalPtrMaps(const M1& map1, const M2& map2)
{
   //***************************************************************************
   // Check that both maps have the same size
   //***************************************************************************
   if(map1.size() != map2.size())
   {
      return false;
   }

   //***************************************************************************
   // Check that all pairs are exactly the same.
   //***************************************************************************
   for(typename M1::const_iterator it1=map1.begin(); it1!=map1.end(); ++it1)
   {
      typename M2::const_iterator it2=map2.find(it1->first);
      if(map2.end()==it2)
      {
         return false;
      }
      
      if(*(it2->second) != *(it1->second))
      {
         return false;
      }
   }

   //***************************************************************************
   // If we get here, everything is equal, so we return true.
   //***************************************************************************
   return true;

} // equalPtrMaps

/**
 * Utility function for checking whether two pointer maps contain exactly the
 * same values.
 */
template<class Map, class PtrMap> bool
   mapEqualsPtrMap(const Map map1, const PtrMap map2)
{
   //***************************************************************************
   // Check that both maps have the same size
   //***************************************************************************
   if(map1.size() != map2.size())
   {
      return false;
   }

   //***************************************************************************
   // Check that all pairs are exactly the same.
   //***************************************************************************
   for(typename Map::const_iterator it1=map1.begin(); it1!=map1.end(); ++it1)
   {
      typename PtrMap::const_iterator it2=map2.find(it1->first);
      if(map2.end()==it2)
      {
         return false;
      }
      
      if(*(it2->second) != it1->second)
      {
         return false;
      }
   }

   //***************************************************************************
   // If we get here, everything is equal, so we return true.
   //***************************************************************************
   return true;

} // function mapEqualsPtrMap

/**
 * Fills a map from a map of pointers to the same type.
 */
template<class Map, class PtrMap>
   void fillFromPtrMap(Map& map, const PtrMap& ptrMap)
{
   map.clear();
   for(typename PtrMap::const_iterator k=ptrMap.begin(); k!=ptrMap.end(); ++k)
   {
      map[k->first] = *(k->second);
   }

} // fillFromPtrMap

/**
 * Check that outboxes can be reproduced from inboxes.
 */
bool checkInEqualsOut(PostOffice_m& office)
{
   //***************************************************************************
   // Try to reproduce the outbox for each sender using inbox values.
   //***************************************************************************
   for(PostOffice_m::SenderIt sIt=office.senderBegin(); sIt!=office.senderEnd();
         ++sIt)
   {
      //************************************************************************
      // Try to reproduce the current sender's outbox
      //************************************************************************
      std::map<long,DiscreteFunction*> curCopy, prevCopy;
      for(PostOffice_m::ReceiverIt rIt=office.receiverBegin();
            rIt!=office.receiverEnd(); ++rIt)
      {
         //*********************************************************************
         // Ignore receivers not related to this sender
         //*********************************************************************
         if(!office.hasEdge(*sIt,*rIt))
         {
            continue;
         }

         //*********************************************************************
         // Try to find this sender in the current receiver's inbox
         //*********************************************************************
         PostOffice_m::InMsgMap  curIn = office.curInMsgs(*rIt);
         PostOffice_m::InMsgMap prevIn = office.prevInMsgs(*rIt);
         PostOffice_m::InMsgIt curInPos = curIn.find(*sIt);
         PostOffice_m::InMsgIt prevInPos = prevIn.find(*sIt);

         if(curIn.end()==curInPos)
         {
            std::cout << "\nfill failed to find current in message\n";
            return false;
         }

         if(prevIn.end()==prevInPos)
         {
            std::cout << "\nfill failed to find previous in message\n";
            return false;
         }

         //*********************************************************************
         // Copy the current and previous messages into our corresponding maps
         //*********************************************************************
         curCopy[*rIt] = curInPos->second;
         prevCopy[*rIt] = prevInPos->second;

      } // for loop

      //************************************************************************
      // Get the stored outboxes for the current sender
      //************************************************************************
      PostOffice_m::OutMsgMap  cur = office.curOutMsgs(*sIt);
      PostOffice_m::OutMsgMap prev = office.prevOutMsgs(*sIt);

      //************************************************************************
      // Check that our maps copied from the inboxes match the actual outboxes
      //************************************************************************
      if(!equalMaps(cur,curCopy))
      {
         std::cout << "\nFailed to reproduce current outbox from inboxes\n";
         return false;
      }

      if(!equalMaps(prev,prevCopy))
      {
         std::cout << "\nFailed to reproduce previous outbox from inboxes\n";
         return false;
      }

   } // for loop

   //***************************************************************************
   // If we get to here, reproduction was successful
   //***************************************************************************
   return true;

} // function checkInEqualsOut

/**
 * Fill post office with random messages.
 * Here, we fill all messages maps with random messages. However, even if we
 * put different random messages in inboxes and outboxes, both should be backed
 * by the same value, so only the last message written should be kepted.
 */
bool fillOffice(PostOffice_m& office)
{
   //***************************************************************************
   // Fill post office outboxes with random messages
   //***************************************************************************
   for(PostOffice_m::SenderIt it=office.senderBegin(); it!=office.senderEnd();
         ++it)
   {
      //************************************************************************
      // Copy original message map values
      //************************************************************************
      PostOffice_m::OutMsgMap  cur = office.curOutMsgs(*it);
      PostOffice_m::OutMsgMap prev = office.curOutMsgs(*it);
      std::map<long,DiscreteFunction> oldCur, oldPrev;
      fillFromPtrMap(oldCur,cur);
      fillFromPtrMap(oldPrev,prev);

      //************************************************************************
      // Check that they match
      //************************************************************************
      if( !mapEqualsPtrMap(oldCur,cur) || !mapEqualsPtrMap(oldPrev,prev) )
      {
         std::cout << "\nfill failed to copy maps\n";
         return false;
      }

      //************************************************************************
      // fill the map with random values
      //************************************************************************
      if(!fillMap(office.curOutMsgs(*it)))
      {
         std::cout << "\nFailed to fill current outbox messages due to "
            "previous error.\n";
         return false;
      }

      if(!fillMap(office.prevOutMsgs(*it)))
      {
         std::cout << "\nFailed to fill previous outbox messages due to "
            "previous error.\n";
         return false;
      }

      //************************************************************************
      // Check that they no longer match
      //************************************************************************
      if( mapEqualsPtrMap(oldCur,cur) || mapEqualsPtrMap(oldPrev,prev) )
      {
         std::cout << "\nfill failed to modify maps\n";
         return false;
      }

   } // for loop

   //***************************************************************************
   // Try to reproduce the outbox for each sender using inbox values.
   //***************************************************************************
   if(!checkInEqualsOut(office))
   {
      std::cout << "\nfill produced inconsistent in and out boxes.\n";
      return false;
   }

   //***************************************************************************
   // If we get here, the fill was successful, so we return true
   //***************************************************************************
   return true;

} // function fillOffice

/**
 * Tests outbox swapping functionality.
 */
bool testSwap(PostOffice_m& office)
{
   //***************************************************************************
   // Try to fill office with random messages
   //***************************************************************************
   if(!fillOffice(office))
   {
      std::cout << "\nSwap test failed due to fill failure\n";
      return false;
   }

   //***************************************************************************
   // For each Sender
   //***************************************************************************
   for(PostOffice_m::SenderIt it=office.senderBegin(); it!=office.senderEnd();
         ++it)
   {
      //************************************************************************
      // Copy original message map values
      //************************************************************************
      PostOffice_m::OutMsgMap  cur = office.curOutMsgs(*it);
      PostOffice_m::OutMsgMap prev = office.prevOutMsgs(*it);
      std::map<long,DiscreteFunction*> oldCur(cur.begin(),cur.end());
      std::map<long,DiscreteFunction*> oldPrev(prev.begin(),prev.end());

      //************************************************************************
      // Make sure previous and current messages don't match
      //************************************************************************
      if( equalMaps(oldPrev,cur) || equalMaps(prev,oldCur) )
      {
         std::cout << "\nswap test for sender " << *it
            << " failed due to prior pointer equality.\n";
         return false;
      }

      //************************************************************************
      // Attempt and validate swap
      //************************************************************************
      office.swapOutBoxes(*it);
      if( !equalMaps(oldPrev,cur) || !equalMaps(prev,oldCur) )
      {
         std::cout << "\nSwap failed for Sender " << *it << std::endl;
         return false;
      }

      //************************************************************************
      // Check that inboxes are consistent after swap
      //************************************************************************
      if(!checkInEqualsOut(office))
      {
         std::cout << "\nswap produced inconsistent in and out boxes.\n";
         return false;
      }

   } // for loop

   //***************************************************************************
   // If we get to here, swap test was successful
   //***************************************************************************
   return true;

} // function testSwap

/**
 * Tests Notification functionality.
 */
bool testNotification(PostOffice_m& office)
{
   //***************************************************************************
   // Retrieve the list of all receivers, and make sure there are no duplicates
   //***************************************************************************
   std::vector<long> receivers(office.receiverBegin(),office.receiverEnd());

   std::sort(receivers.begin(),receivers.end()); // required by std::unique()
   if(receivers.end()!=std::unique(receivers.begin(),receivers.end()))
   {
      std::cout << "\nDuplicated detected in receivers list.\n";
      return false;
   }

   //***************************************************************************
   // Start by notifying all receivers
   //***************************************************************************
   office.notifyAll();

   //***************************************************************************
   // If there are no registered receivers, then we should never have mail
   //***************************************************************************
   if(receivers.empty())
   {
      //************************************************************************
      // Notice count should be zero when there are no receivers
      //************************************************************************
      if(0!=office.noticeCount())
      {
         std::cout << "\nNon-zero notice count when no receivers registered.\n";
         return false;
      }

      //************************************************************************
      // There should be no mail when there are no receivers
      //************************************************************************
      if(office.newMail())
      {
         std::cout << "\nNew mail when no receivers registered.\n";
         return false;
      }

      //************************************************************************
      // Make sure that popNotice() raises an exception.
      //************************************************************************
      try
      {
         office.popNotice();
         std::cout << "\n#0 Missing exception after popNotice()\n";
         return false;
      }
      catch(EmptyNoticeException e) { /* this should happen */ }

      //************************************************************************
      // If the above tests check out, then given that we have no receivers,
      // there is nothing left for us to check.
      //************************************************************************
      return true;

   } // if statement

   //***************************************************************************
   // If we get here, then we have at least one registered receiver, which
   // should by now be notified. Thus, newMail() should now return true.
   //***************************************************************************
   if(!office.newMail())
   {
      std::cout << "\nnewMail()==false after notifyAll()\n";
      return false;
   }

   //***************************************************************************
   // Get the identities of all notified receivers
   //***************************************************************************
   std::vector<long> notices;
   notices.reserve(receivers.size());
   while(office.newMail())
   {
      notices.push_back(office.popNotice());
   }

   //***************************************************************************
   // Make sure that all receivers have been notified
   //***************************************************************************
   if( notices.size()!=receivers.size() )
   {
      std::cout << "\nWrong number of notices returned: " << notices.size()
         << " should be " << receivers.size();
      return false;
   }

   std::sort(notices.begin(),notices.end()); // required by std::equal()
   if(!std::equal(notices.begin(),notices.end(),receivers.begin()))
   {
      std::cout << "\nIncorrect list of all notifications.\n";
      return false;
   }

   //***************************************************************************
   // Ensure that there are now no notices
   //***************************************************************************
   if(0!=office.noticeCount())
   {
      std::cout << "\nNon-zero notice count after newMail()==false\n";
      return false;
   }

   try
   {
      office.popNotice();
      std::cout << "\n#1 Missing exception after popNotice()\n";
      return false;
   }
   catch(EmptyNoticeException e) { /* this should happen */ }

   //***************************************************************************
   // Notify one receiver and validate notices
   //***************************************************************************
   office.notify(receivers[0]);

   if(!office.newMail())
   {
      std::cout << "\nNo new mail after notification.\n";
      return false;
   }

   if(1!=office.noticeCount())
   {
      std::cout << "\nWrong notice count after notification: "
         << office.noticeCount() << " should be 1.\n";
      return false;
   }

   long receiverId = office.popNotice();
   if(receivers[0]!=receiverId)
   {
      std::cout << "\nWrong notice returned: " << receiverId << " should be "
         << receivers[0];
      return false;
   }

   if(office.newMail())
   {
      std::cout << "\nNew mail after popNotice()\n";
      return false;
   }

   if(0!=office.noticeCount())
   {
      std::cout << "\nWrong count after popNotice()\n";
      return false;
   }

   try
   {
      office.popNotice();
      std::cout << "\n#2 Missing exception after popNotice()\n";
      return false;
   }
   catch(EmptyNoticeException e) { /* this should happen */ }

   //***************************************************************************
   // Notify another few receivers.
   //***************************************************************************
   int correctCount = receivers.size()/2;
   for(int k=0; k<correctCount; ++k)
   {
      office.notify(receivers[k]);
   }

   //***************************************************************************
   // Check notice count is correct
   //***************************************************************************
   if(office.noticeCount()!=correctCount)
   {
      std::cout << "\nIncorrect notice count " << office.noticeCount()
         << " after sending " << correctCount << " notices.\n";
      return false;
   }

   //***************************************************************************
   // Check new mail flag after sending notices
   //***************************************************************************
   if(office.newMail() == (0==correctCount))
   {
      std::cout << "\nIncorrect newMail flag after sending " << correctCount
         << " notices.\n";
      return false;
   }
   
   //***************************************************************************
   // Get the last set of notices.
   //***************************************************************************
   std::vector<long> lastNotices;
   lastNotices.reserve(office.noticeCount());
   while(office.newMail())
   {
      lastNotices.push_back(office.popNotice());
   }
   std::sort(lastNotices.begin(),lastNotices.end());

   //***************************************************************************
   // Check for duplicates
   //***************************************************************************
   if(lastNotices.end()!=std::unique(lastNotices.begin(),lastNotices.end()))
   {
      std::cout << "\nLast set of notices contains duplicates.\n";
      return false;
   }

   //***************************************************************************
   // Check that the correct set of receiver ids have been notified.
   //***************************************************************************
   if(lastNotices.size() != correctCount)
   {
      std::cout << "\nWrong number of notices popped after last notice: "
         << lastNotices.size() << " should be " << correctCount << '\n';
      return false;
   }
   if(!std::equal(lastNotices.begin(),lastNotices.end(),receivers.begin()))
   {
      std::cout << "\nWrong notices returned after last notice.\n";
      return false;
   }

   //***************************************************************************
   // If we've got this far - we've done well!
   //***************************************************************************
   return true;

} // function testNotification

/**
 * Returns true if PostOffice state is consistent with edge list.
 */
bool isConsistent(const std::vector<Edge_m>& edges, PostOffice_m& office)
{
   //***************************************************************************
   // Ensure the number of edges is correct
   //***************************************************************************
   if(edges.size() != office.numOfEdges())
   {
      std::cout << "\nIncorrect number of edges\n";
      return false;
   }

   //***************************************************************************
   // For each valid edge
   //***************************************************************************
   typedef std::map<std::string,std::set<long> > SenderMap;
   typedef std::map<long,std::set<std::string> > ReceiverMap;
   SenderMap senders;
   ReceiverMap receivers;
   for(std::vector<Edge_m>::const_iterator it=edges.begin(); it!=edges.end();
         ++it)
   {
      //************************************************************************
      // Ensure the edge is recorded in the post office
      //************************************************************************
      if(!office.hasEdge(it->sender,it->receiver))
      {
         std::cout << "\nMissing edges in post office\n";
         return false;
      }

      //************************************************************************
      // Index the edges by sender and receiver
      //************************************************************************
      senders[it->sender].insert(it->receiver);
      receivers[it->receiver].insert(it->sender);

   } // for loop

   //***************************************************************************
   // Ensure that edges don't exist unless we have specified them.
   //***************************************************************************
   if(office.hasEdge("NoSuch",194832))
   {
      std::cout << "\nNon-existant edge between non-existant parties\n";
      return false;
   }

   for(ReceiverMap::iterator rIt=receivers.begin(); rIt!=receivers.end(); ++rIt)
   {
      long curReceiver = rIt->first;
      for(SenderMap::iterator sIt=senders.begin(); sIt!=senders.end(); ++sIt)
      {
         std::string curSender = sIt->first;
         if(senders[curSender].end()==senders[curSender].find(curReceiver))
         {
            if(office.hasEdge(curSender,curReceiver))
            {
               std::cout << "\nNon-existant edge between existant parties\n";
               return false;
            }
         }

      } // inner for loop
   } // outer for loop

   //***************************************************************************
   // Ensure that non-existant edges throw exceptions
   //***************************************************************************
   try
   {
      office.curOutMsgs("NoSuch");
      std::cout << "\nMissing exception from curOutMsgs()\n";
      return false;
   }
   catch(maxsum::UnknownAddressException e) { /* OK */ }

   try
   {
      office.prevOutMsgs("NoSuch");
      std::cout << "\nMissing exception from prevOutMsgs()\n";
      return false;
   }
   catch(maxsum::UnknownAddressException e) { /* OK */ }

   try
   {
      office.curInMsgs(12439);
      std::cout << "\nMissing exception from curInMsgs()\n";
      return false;
   }
   catch(maxsum::UnknownAddressException e) { /* OK */ }

   try
   {
      office.prevInMsgs(123403);
      std::cout << "\nMissing exception from prevInMsgs()\n";
      return false;
   }
   catch(maxsum::UnknownAddressException e) { /* OK */ }

   //***************************************************************************
   // Ensure that the number of senders and receivers is correct
   //***************************************************************************
   if(senders.size() != office.numOfSenders())
   {
      std::cout << "\nIncorrect number of senders: " << office.numOfSenders()
         << "should be " << senders.size() << std::endl;
      return false;
   }

   if(receivers.size() != office.numOfReceivers())
   {
      std::cout << "\nIncorrect number of receivers: "
         << office.numOfReceivers() << " should be " << receivers.size()
         << std::endl;
      return false;
   }

   //***************************************************************************
   // Check that the domain is correct for all input message maps
   //***************************************************************************
   for(ReceiverMap::iterator rIt=receivers.begin(); rIt!=receivers.end(); ++rIt)
   {
      PostOffice_m::InMsgMap  curInMsgs = office.curInMsgs(rIt->first);
      PostOffice_m::InMsgMap prevInMsgs = office.prevInMsgs(rIt->first);
      
      if(!validKeys(curInMsgs,rIt->second))
      {
         std::cout << "\nWrong keyset for curInMsgs\n";
         return false;
      }

      if(!validKeys(prevInMsgs,rIt->second))
      {
         std::cout << "\nWrong keyset for curInMsgs\n";
         return false;
      }

   } // for loop

   //***************************************************************************
   // Check that the domain is correct for all output message maps
   //***************************************************************************
   for(SenderMap::iterator sIt=senders.begin(); sIt!=senders.end(); ++sIt)
   {
      PostOffice_m::OutMsgMap  curOutMsgs = office.curOutMsgs(sIt->first);
      PostOffice_m::OutMsgMap prevOutMsgs = office.prevOutMsgs(sIt->first);

      if(!validKeys(curOutMsgs,sIt->second))
      {
         std::cout << "\nWrong keyset for curOutMsgs\n";
         return false;
      }

      if(!validKeys(prevOutMsgs,sIt->second))
      {
         std::cout << "\nWrong keyset for prevOutMsgs\n";
         return false;
      }

   } // for loop

   return true;

} // function isConsistent

int main()
{
   int errorCount = 0;
   try
   {
      //************************************************************************
      // Construct a list of edges between some senders and receivers
      //************************************************************************
      std::cout << "Constructing senders and receivers...";
      std::vector<Edge_m> edges;
      edges.push_back(Edge_m("a",1));
      edges.push_back(Edge_m("a",2));
      edges.push_back(Edge_m("a",3));
      edges.push_back(Edge_m("b",3));
      edges.push_back(Edge_m("b",4));
      edges.push_back(Edge_m("c",4));
      edges.push_back(Edge_m("d",5));
      edges.push_back(Edge_m("d",1));
      edges.push_back(Edge_m("e",6));
      std::cout << "OK\n";

      //************************************************************************
      // Construct a PostOffice for test purposes
      //************************************************************************
      std::cout << "Constructing PostOffice...";
      PostOffice_m postOffice;
      std::cout << "OK\n";

      //************************************************************************
      // Check that the PostOffice initially has no edges, receivers, or senders
      //************************************************************************
      std::cout << "Validating new PostOffice...";
      if(isConsistent(std::vector<Edge_m>(),postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }

      //************************************************************************
      // Register our senders and receivers and check consistency
      //************************************************************************
      std::cout << "Registering edges...";
      for(std::vector<Edge_m>::iterator it=edges.begin(); it!=edges.end(); ++it)
      {
         postOffice.addEdge(it->sender,it->receiver);
      }

      if(isConsistent(edges,postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }

      //************************************************************************
      // Test Notification functionality
      //************************************************************************
      std::cout << "Trying to notify receivers...";
      if(testNotification(postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }

      //************************************************************************
      // Ensure that duplicates change nothing
      //************************************************************************
      std::cout << "Registering duplicate edges...";
      for(std::vector<Edge_m>::iterator it=edges.begin();
            it!=edges.end(); ++it)
      {
         postOffice.addEdge(it->sender,it->receiver);
      }

      if(isConsistent(edges,postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }

      //************************************************************************
      // Check deletion works
      //************************************************************************
      std::random_shuffle(edges.begin(),edges.end());
      std::vector<Edge_m>::iterator middle = edges.begin()+edges.size()/2;
      std::vector<Edge_m> deletedEdges(edges.begin(),middle);
      std::cout << "Deleting " << deletedEdges.size() << " edges...";
      std::vector<Edge_m> remainingEdges(middle,edges.end());
      for(std::vector<Edge_m>::iterator it=deletedEdges.begin();
            it!=deletedEdges.end(); ++it)
      {
         postOffice.removeEdge(it->sender,it->receiver);
      }

      if(isConsistent(remainingEdges,postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }

      //************************************************************************
      // Try to fill and validate inboxes and outboxes
      //************************************************************************
      std::cout << "Trying to fill out and in boxes...";
      if(fillOffice(postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }

      //************************************************************************
      // Test Swapping functionality
      //************************************************************************
      std::cout << "Trying to swap current and previous messages...";
      if(testSwap(postOffice))
      {
         std::cout << "OK\n";
      }
      else
      {
         std::cout << "FAILED\n";
         ++errorCount;
      }
   }
   //***************************************************************************
   // Catch any unexpected exceptions.
   //***************************************************************************
   catch(std::exception& e)
   {
      std::cout << "\nCaught unexpected exception: " << e.what() << std::endl;
      ++errorCount;
   }

   //***************************************************************************
   // Print out the total number of errors and return failure if this is 
   // non-zero.
   //***************************************************************************
   std::cout << "Total errors: " << errorCount << std::endl;
   if(0!=errorCount)
   {
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;

} // main function
