/**
 * @file util_containers.h
 * This file defines some utility container types required for the
 * implementation of the maxsum library.
 * @attention The types defined in this header are not intended to form part
 * of the public interface to the maxsum library. Knowledge of these types is
 * therefore not required for interfacing with the library.
 */
#ifndef MAXSUM_UTIL_CONTAINERS_H
#define MAXSUM_UTIL_CONTAINERS_H

#include <cassert>
#include <map>

namespace maxsum
{

/**
 * Utility namespace for types used for maxsum library implementation.
 * The contents of this namespace are not intended to form part of the external
 * interface to the maxsum library, and can be safely ignored by third party
 * developers.
 */
namespace util
{
   /**
    * Utility class for presenting the keys of a map in a read-only container.
    * Provides methods for iterating over this set that adhere to standard
    * library concepts.
    * @attention This class is used as part of the implementation of the
    * maxsum::PostOffice class only, and so does not need to be referenced
    * directly by calling libraries. We therefore only implement the subset
    * of the standard container iterface that we actually need for our own
    * purposes.
    * @tparam Map the class of underlying map referenced by this object.
    */
   template<class Map> class KeySet
   {
   public:

      /**
       * The type of value contained in this set, which is always equal to the
       * key type of the underlying map.
       */
      typedef typename Map::key_type value_type;

      /**
       * Iterator type that allows read-only access to the keys of the
       * underlying map. This is achieved by overriding the map iterator's
       * own dereferencing operator, so that only a constant reference to the
       * key is returned.
       */
      class const_iterator : public Map::const_iterator
      {
      public:
         
         /**
          * Constructs a new KeySet iterator from a map iterator.
          */
         const_iterator(typename Map::const_iterator it)
            : Map::const_iterator(it) {}

         /**
          * Overrides dereference operator so that a direct reference to the 
          * Map key is returned.
          */
         const KeySet::value_type& operator*() const
         {
            return (Map::const_iterator::operator*()).first;
         }

      }; // KeySet::const_iterator

   private:

      /**
       * Reference to the map underlying this maxsum::KeySet.
       */
      const Map* pMap_i;

   public:

      /**
       * Construct a new maxsum::KeySet backed by a specified map.
       * @param[in] keyMap the backend map whoses key set we wish to present
       * through this class. Changes to keyMap are tracked by this object,
       * but no modificiations can be made to keyMap through this object.
       * @attention If pKeyMap is null, then it must be set to point to a valid
       * object using KeySet::setMap before any other member function is called.
       * @post *pKeyMap must not be destroyed before this maxsum::KeySet.
       */
      KeySet(const Map* const pKeyMap=0) : pMap_i(pKeyMap) {}

      /**
       * Sets the backend map referenced by this KeySet.
       */
      void setMap(const Map* const pKeyMap)
      {
         pMap_i = pKeyMap;
      }

      /**
       * Returns an iterator to the beginning of the key set.
       * @returns an iterator to the beginning of the key set.
       */
      const_iterator begin() const
      {
         assert(0!=pMap_i);
         return const_iterator(pMap_i->begin());
      }

      /**
       * Returns an iterator to the end of the key set.
       * As in the standard library, the end iterator points one space beyond
       * the last element in the container.
       */
      const_iterator end() const
      {
         assert(0!=pMap_i);
         return const_iterator(pMap_i->end());
      }

      /**
       * Returns the size of the underlying map.
       * @returns the size of the underlying map.
       */
      int size() const
      {
         assert(0!=pMap_i);
         return pMap_i->size();
      }

      /**
       * Returns an iterator to the specified key, if it is in the map, or
       * KeySet::const_iterator::end if it is not.
       * @returns an iterator to the specified key, if it is in the map, or
       * KeySet::const_iterator::end if it is not.
       */
      const_iterator find(const value_type& k) const
      {
         assert(0!=pMap_i);
         return const_iterator(pMap_i->find(k));
      }

      /**
       * Returns true if the specified key is in this set.
       */
      bool contains(const value_type& k) const
      {
         return 0!=pMap_i->count(k);
      }

   }; // class KeySet

   /**
    * This class provides a read only wrapper around an existing map, such
    * as that provided by std::map.
    * In particular, this class provides a const implementation of the
    * [] operator, which throws an exception if the requested key is not
    * already in the underlying map, rather than trying to insert it.
    * This functionality is not provided by the standard library.
    * <p>Note: Currently, we only implement that subset of the map iterface
    * that we actually require for our purposes.</p>
    * @attention This class is used as part of the implementation of the
    * maxsum::PostOffice class only, and so does not need to be referenced
    * directly by calling libraries. We therefore only implement the subset
    * of the standard container iterface that we actually need for our own
    * purposes.
    * @tparam Key Type of key used by this map.
    * @tparam Val Type of value referenced by this map. Internally, the map
    * stores pointers to Val objects, rather than the Val objects themselves,
    * but presents the same interface as a std::map that does store them.
    */
   template<class Key, class Val> class RefMap
   {
   public:

      /**
       * Type of map used to store value pointers.
       */
      typedef std::map<Key,Val> PtrMap;

      typedef typename PtrMap::const_iterator const_iterator;

      typedef typename PtrMap::value_type value_type;

      typedef typename PtrMap::value_type key_type;

      typedef typename PtrMap::mapped_type mapped_type;

   private:

      /**
       * std::map used to store value pointers.
       * Essentially, RefMap is just a wrapper around this object.
       */
      const PtrMap* pMap_i;

   public:

      /**
       * Constructs an empty map.
       */
      RefMap(const PtrMap& map) : pMap_i(&map) {}

      /**
       * Returns an iterator the first element in this map.
       * This iterator provides readonly access to the elements of this map.
       */
      const_iterator begin() const
      {
         return pMap_i->begin();
      }

      /**
       * Returns an iterator to the next position after the last element of
       * this map. See C++ standard library documentation for rationale.
       */
      const_iterator end() const
      {
         return pMap_i->end();
      }

      /**
       * Provides a constant reference to the value pointed to by key, or
       * throws an exception is the key is not currently mapped to any value.
       * @attention This is intentionally different from the behaviour of 
       * std::map::operator[], because we only want to provide read-access
       * through this operator.
       * @param[in] key the key whoses value we want to return.
       * @throws NoSuchElementException if key is not currently in this
       * map.
       * @returns a constant reference to the mapped value.
       */
      const Val& operator[](Key key) const
      {
         typename PtrMap::const_iterator it = pMap_i->find(key);
         if(pMap_i->end()==it)
         {
            throw NoSuchElementException("sum::util::ReMap::operator[]",
                  "No such key in this RefMap");
         }

         return it->second;

      } // operator[]

      /**
       * Tries to find the value for a given key in this map.
       */
      const_iterator find(Key key) const
      {
         return pMap_i->find(key);
      }

      /**
       * Returns the number of elements in this map.
       */
      int size() const
      {
         return pMap_i->size();
      }

   }; // class RefMap

} // namespace util
} // namespace maxsum



#endif // MAXSUM_UTIL_CONTAINERS_H

