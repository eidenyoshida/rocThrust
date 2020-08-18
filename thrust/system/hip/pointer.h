/******************************************************************************
 * Copyright (c) 2008-2018, NVIDIA CORPORATION.  All rights meserved.
 * Modifications Copyright (c) 2019-2020, Advanced Micro Devices, Inc.  All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in ccudaliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

#pragma once

#include <thrust/detail/config.h>
#include <thrust/system/hip/detail/execution_policy.h>
#include <thrust/detail/type_traits.h>
#include <thrust/detail/pointer.h>

namespace thrust
{
namespace hip_rocprim
{
    template <typename>
    class pointer;

} // end hip_rocprim
} // end thrust

template <typename Element, typename Tag, typename Reference, typename Derived>
struct std::iterator_traits<thrust::pointer<Element, Tag, Reference, Derived>>
{
private:
    typedef thrust::pointer<Element, Tag, Reference, Derived> ptr;

public:
    typedef typename ptr::iterator_category iterator_category;
    typedef typename ptr::value_type        value_type;
    typedef typename ptr::difference_type   difference_type;
    typedef Element*                        pointer;
    typedef typename ptr::reference         reference;
};

namespace thrust
{

// specialize thrust::iterator_traits to avoid problems with the name of
// pointer's constructor shadowing its nested pointer type
// do this before pointer is defined so the specialization is correctly
// used inside the definition

template <typename Element>
struct iterator_traits<thrust::hip_rocprim::pointer<Element> >
{
private:
  typedef thrust::hip_rocprim::pointer<Element> ptr;

public:
  typedef typename ptr::iterator_category iterator_category;
  typedef typename ptr::value_type        value_type;
  typedef typename ptr::difference_type   difference_type;
  typedef ptr                             pointer;
  typedef typename ptr::reference         reference;
};    // end iterator_traits

namespace hip_rocprim {

/*! \addtogroup system_backends Systems
 *  \ingroup system
 *  \{
 */

/*! \namespace thrust::system::hip
 *  \brief \p thrust::system::hip is the namespace containing functionality for allocating, manipulating,
 *         and deallocating memory available to Thrust's hip backend system.
 *         The identifiers are provided in a separate namespace underneath <tt>thrust::system</tt>
 *         for import convenience but are also aliased in the top-level <tt>thrust::hip</tt>
 *         namespace for easy access.
 *
 */


// forward declaration of reference for pointer
template <typename Element>
class reference;

/*! \p pointer stores a pointer to an object allocated in memory available to the hip system.
 *  This type provides type safety when dispatching standard algorithms on ranges resident
 *  in hip memory.
 *
 *  \p pointer has pointer semantics: it may be dereferenced and manipulated with pointer arithmetic.
 *
 *  \p pointer can be created with the function \p hip::malloc, or by explicitly calling its constructor
 *  with a raw pointer.
 *
 *  The raw pointer encapsulated by a \p pointer may be obtained by eiter its <tt>get</tt> member function
 *  or the \p raw_pointer_cast function.
 *
 *  \note \p pointer is not a "smart" pointer; it is the programmer's responsibility to deallocate memory
 *  pointed to by \p pointer.
 *
 *  \tparam T specifies the type of the pointee.
 *
 *  \see hip::malloc
 *  \see hip::free
 *  \see raw_pointer_cast
 */

template <typename T>
class pointer : public thrust::pointer<T,
                                        thrust::hip_rocprim::tag,
                                        thrust::hip_rocprim::reference<T>,
                                        thrust::hip_rocprim::pointer<T>>
{

private:
    typedef thrust::pointer<T,
                            thrust::hip_rocprim::tag,
                            thrust::hip_rocprim::reference<T>,
                            thrust::hip_rocprim::pointer<T>>
        super_t;

public:
    /*! \p pointer's no-argument constructor initializes its encapsulated pointer to \c 0.
     */
    __host__ __device__ pointer()
        : super_t()
    {
    }

    #if THRUST_CPP_DIALECT >= 2011
    // NOTE: This is needed so that Thrust smart pointers can be used in
    // `std::unique_ptr`.
    __host__ __device__
    pointer(decltype(nullptr)) : super_t(nullptr) {}
    #endif


    template <typename OtherT>
    __host__ __device__ explicit pointer(OtherT* ptr)
        : super_t(ptr)
    {
    }


    /*! This constructor allows construction from another pointer-like object with related type.
     *
     *  \param other The \p OtherPointer to copy.
     *  \tparam OtherPointer The system tag associated with \p OtherPointer shall be convertible
     *          to \p thrust::system::hip::tag and its element type shall be convertible to \p T.
     */
    template <typename OtherPointer>
    __host__ __device__
                pointer(const OtherPointer& other,
                        typename thrust::detail::enable_if_pointer_is_convertible<OtherPointer,
                                                                            pointer>::type* = 0)
        : super_t(other)
    {
    }

    /*! This constructor allows construction of a <tt>pointer<const T></tt> from a <tt>T*</tt>.
     *
     *  \param ptr A raw pointer to copy from, presumed to point to a location in memory
     *         accessible by the \p hip system.
     *  \tparam OtherT \p OtherT shall be convertible to \p T.
     */

    template <typename OtherPointer>
    __host__ __device__
    explicit
    pointer(const OtherPointer &other,
            typename thrust::detail::enable_if_void_pointer_is_system_convertible<
                OtherPointer,
                pointer>::type * = 0) : super_t(other)
    {
    }

    /*! Assignment operator allows assigning from another pointer-like object with related type.
     *
     *  \param other The other pointer-like object to assign from.
     *  \tparam OtherPointer The system tag associated with \p OtherPointer shall be convertible
     *          to \p thrust::system::hip::tag and its element type shall be convertible to \p T.
     */
    template <typename OtherPointer>
    __host__ __device__ typename thrust::detail::
        enable_if_pointer_is_convertible<OtherPointer, pointer, pointer&>::type
        operator=(const OtherPointer& other)
    {
        return super_t::operator=(other);
    }
}; // struct pointer

#if THRUST_CPP_DIALECT >= 2011
template <typename T>
__host__ __device__
bool operator!=(decltype(nullptr), pointer<T>);

template <typename T>
__host__ __device__
bool operator!=(pointer<T>, decltype(nullptr));

template <typename T>
__host__ __device__
bool operator==(decltype(nullptr), pointer<T>);

template <typename T>
__host__ __device__
bool operator==(pointer<T>, decltype(nullptr));
#endif


/*! \p reference is a wrapped reference to an object stored in memory available to the \p hip system.
 *  \p reference is the type of the result of dereferencing a \p hip::pointer.
 *
 *  \tparam T Specifies the type of the referenced object.
 */
template <typename T>
class reference
    : public thrust::reference<
          T,
          thrust::hip_rocprim::pointer<T>,
          thrust::hip_rocprim::reference<T> >
{

private:
  typedef thrust::reference<
      T,
      thrust::hip_rocprim::pointer<T>,
      thrust::hip_rocprim::reference<T> >
      super_t;

public:
  typedef typename super_t::value_type value_type;
  typedef typename super_t::pointer    pointer;

    /*! This constructor initializes this \p reference to refer to an object
     *  pointed to by the given \p pointer. After this \p reference is constructed,
     *  it shall refer to the object pointed to by \p ptr.
     *
     *  \param ptr A \p pointer to copy from.
     */

  __host__ __device__ explicit reference(const pointer &ptr)
      : super_t(ptr)
  {
  }

    /*! This constructor accepts a const reference to another \p reference of related type.
     *  After this \p reference is constructed, it shall refer to the same object as \p other.
     *
     *  \param other A \p reference to copy from.
     *  \tparam OtherT The element type of the other \p reference.
     *
     *  \note This constructor is templated primarily to allow initialization of <tt>reference<const T></tt>
     *        from <tt>reference<T></tt>.
     */

  template <typename OtherT>
  __host__ __device__
  reference(const reference<OtherT> &other,
            typename thrust::detail::enable_if_convertible<
                typename reference<OtherT>::pointer,
                pointer>::type * = 0)
      : super_t(other)
  {
  }

    /*! Copy assignment operator copy assigns from another \p reference of related type.
     *
     *  \param other The other \p reference to assign from.
     *  \return <tt>*this</tt>
     *  \tparam OtherT The element type of the other \p reference.
     */
  template <typename OtherT>
  __host__ __device__
      reference &
      operator=(const reference<OtherT> &other);

    /*! Assignment operator assigns from a \p value_type.
     *
     *  \param x The \p value_type to assign from.
     *  \return <tt>*this</tt>
     */
  __host__ __device__
      reference &
      operator=(const value_type &x);
};    // struct reference


/*! Exchanges the values of two objects referred to by \p reference.
 *  \p x The first \p reference of interest.
 *  \p y The second \p reference ot interest.
 */
template <typename T>
__host__ __device__ void swap(reference<T> x, reference<T> y);

} // end namespace hip_rocprim

namespace system {

/*! \namespace thrust::hip
 *  \brief \p thrust::hip is a top-level alias for thrust::system::hip.
 */
namespace hip
{
using thrust::hip_rocprim::pointer;
using thrust::hip_rocprim::reference;
} // end hip
} // end system

namespace hip {
using thrust::hip_rocprim::pointer;
using thrust::hip_rocprim::reference;
} // end hip

} // end thrust

#include <thrust/system/hip/detail/pointer.inl>
