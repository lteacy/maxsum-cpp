/**
 * @file StaticVector.h
 * An alternative to std::vector that uses a fixed size block rather to
 * avoid allocation on the heap.
 * @author Luke Teacy
 */
#ifndef MAX_SUM_STATIC_VECTOR_H
#define MAX_SUM_STATIC_VECTOR_H

#include <cassert>
#include <algorithm>

/**
 * Namespace for all public types and functions defined by the Max-Sum library.
 */
namespace maxsum
{
   /**
    * An alternative to std::vector that uses a fixed size block rather to
    * avoid allocation on the heap. Optimised for small variable size arrays
    * that don't get bigger than some small value.
    * @author Luke Teacy
    */
   template<class T, int maxSize=10> class StaticVector
   {
   public:

      /**
       * Size type for this vector.
       */
      typedef size_t size_type;

      /**
       * Read only iterator type.
       */
      typedef const T* const_iterator;

      /**
       * Output interator type.
       */
      typedef T* iterator;

      /**
       * Type of value stored in this vector.
       */
      typedef T value_type;

      /**
       * Reference to value type.
       */
      typedef T& reference;

      /**
       * Const reference to value type.
       */
      typedef const T& const_reference;

   private:

      /**
       * Fixed size array in which to hold data.
       */
      T vData_i[maxSize];

      /**
       * Pointer to the end of the actual data.
       */
      T* pEnd_i;

   public:

      /**
       * Default constructor creates an empty list.
       */
      StaticVector() : pEnd_i(vData_i) {}

      /**
       * Fill constructor.
       * Fills this vector with n copies of a given value.
       */
      StaticVector(int n, const T& val=T())
         : pEnd_i(vData_i+n)
      {
         assert(maxSize>n);
         std::fill_n(vData_i,n,val);
      }

      /**
       * Fill constructor.
       * Fills this vector with a given range.
       */
      template<class It> StaticVector(It first, It last)
         : pEnd_i(vData_i)
      {
         while(first!=last)
         {
            *pEnd_i = *first;
            ++pEnd_i;
            ++first;
            assert(pEnd_i<=(vData_i+maxSize));
         }

      } // fill constructor

      /**
       * Copy constructor.
       */
      StaticVector(const StaticVector& rhs)
         : pEnd_i(vData_i)
      {
         for(const_iterator it=rhs.vData_i; it!=rhs.pEnd_i; ++it)
         {
            *pEnd_i = *it;
            ++pEnd_i;
            assert(pEnd_i<=(vData_i+maxSize));
         }
      }

      /**
       * Copy assignment.
       */
      StaticVector& operator=(const StaticVector& rhs)
      {
         pEnd_i = vData_i;
         for(const_iterator it=rhs.vData_i; it!=rhs.pEnd_i; ++it)
         {
            *pEnd_i = *it;
            ++pEnd_i;
            assert(pEnd_i<=(vData_i+maxSize));
         }
         return *this;
      }

      /**
       * Writable iterator to the start of this vector.
       */
      iterator begin()
      {
         return vData_i;
      }

      /**
       * Writable iterator to the end of this vector.
       */
      iterator end()
      {
         return pEnd_i;
      }

      /**
       * Read only iterator to the start of this vector.
       */
      const_iterator begin() const
      {
         return vData_i;
      }

      /**
       * Read only iterator to the end of this vector.
       */
      const_iterator end() const
      {
         return pEnd_i;
      }

      /**
       * Returns a read only reference to the nth element.
       */
      const_reference operator[](size_type n) const
      {
         assert(n<maxSize);
         return vData_i[n];
      }

      /**
       * Returns a writable reference to the nth element.
       */
      reference operator[](size_type n)
      {
         assert(n<maxSize);
         return vData_i[n];
      }

      /**
       * Returns the size of this vector.
       */
      size_type size() const
      {
         assert(0<=pEnd_i-vData_i);
         return pEnd_i-vData_i;
      }

      /**
       * Returns the maximum size of this container.
       */
      size_type max_size() const
      {
         return maxSize;
      }

      /**
       * True iff this vector is empty.
       */
      bool empty() const
      {
         return pEnd_i==vData_i;
      }

      /**
       * Inserts a new element into this vector.
       */
      iterator insert(iterator position, const T& value)
      {
         return insert(position,&value,(&value)+1);
      }

      /**
       * Inserts a list of elements into this vector.
       */
      template<class It> iterator insert(iterator pPos, It first, It last)
      {
         //*********************************************************************
         // Store the start position so that we can return it later
         // (required for standard library compatibility).
         //*********************************************************************
         iterator pFirst = pPos;

         //*********************************************************************
         // Sanity change position iterator is in correct range
         //*********************************************************************
         assert(pPos >= vData_i);
         assert(pPos <= pEnd_i);
         assert(pPos < vData_i+maxSize);

         //*********************************************************************
         // Store any elements that need to be moved in a buffer.
         //*********************************************************************
         value_type vBuffer[maxSize];
         iterator pBufferEnd = std::copy(pPos,pEnd_i,vBuffer);
         assert(pBufferEnd < vBuffer+maxSize);
         assert((pBufferEnd-vBuffer)==(pEnd_i-pPos));

         //*********************************************************************
         // Now try to copy the new elements into place
         //*********************************************************************
         for(It cur=first; cur!=last; ++cur)
         {
            *pPos = *cur;
            ++pPos;
            assert(pPos < vData_i+maxSize);
         }

         //*********************************************************************
         // Now copy our old elements back into place
         //*********************************************************************
         for(const_iterator it=vBuffer; it!=pBufferEnd; ++it)
         {
            *pPos = *it;
            ++pPos;
            assert(pPos < vData_i+maxSize);
         }

         //*********************************************************************
         // The pPos iterator should now point to the new end of the vector.
         //*********************************************************************
         pEnd_i=pPos;

         //*********************************************************************
         // Return an iterator to the first inserted element
         //*********************************************************************
         return pFirst;

      } // insert

      /**
       * Resizes this vector.
       * If the new size is smaller, then the last elements are dropped of the
       * list. If the new size is larger, then the new elements are initalised
       * using the input value.
       */
      void resize(size_type n, const value_type& val=value_type())
      {
         int diff = n-size();
         iterator oldEnd = pEnd_i;
         pEnd_i += diff;
         assert(pEnd_i<=(vData_i+maxSize));

         if(0<diff)
         {
            std::fill_n(oldEnd,diff,val);
         }
      }

      /**
       * Required for standard library compatibility.
       * No operation, since memory is statically allocated.
       */
      void reserve(size_type n) 
      {
         assert(maxSize>=n);
      }

      /**
       * Required for standard library compatibility.
       * No operation, since memory is statically allocated.
       */
      void shrink_to_fit() { }

      /**
       * Returns the current capacity of this vector.
       * In this implementation, that value is always fixed to max_size.
       */
      size_type capacity() const
      {
         return maxSize;
      }

      /**
       * Sets the size of this vector to zero.
       */
      void clear()
      {
         pEnd_i=vData_i;
      }

      /**
       * Swap to vectors.
       * This is the only operation I would expect to be slower than
       * std::vector, since it actually involves copying. But for small
       * vectors, maybe this is still quicker than the overall cost of dynamic
       * memory allocation?
       */
      void swap(StaticVector& rhs)
      {
         StaticVector myClone(*this);
         *this = rhs;
         rhs = myClone;
      }

   }; // class StaticVector

} // namespace maxsum

#endif // MAX_SUM_STATIC_VECTOR_H
