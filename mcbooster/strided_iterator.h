/*
 * strided_iterator.h
 *
 * original code: https://github.com/thrust/thrust/blob/master/examples/strided_range.cu
 *
 * Created on : Feb 25, 2016
 * Author: Antonio Augusto Alves Junior
 * SPDX-License-Identifier: BSD-3-Clause
 *      
 */


#ifndef STRIDED_ITERATOR_H_
#define STRIDED_ITERATOR_H_

#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/functional.h>

namespace mcbooster {
/** \class strided_range
 * Strided range iterator original code: https://github.com/thrust/thrust/blob/master/examples/strided_range.cu
 */
template<typename Iterator>
class strided_range {
  public:
    typedef typename thrust::iterator_difference<Iterator>::type difference_type;

    struct stride_functor : public thrust::unary_function<difference_type, difference_type> {
        difference_type stride;

        stride_functor(difference_type stride)
            : stride(stride) {}

        __host__ __device__ difference_type operator()(const difference_type &i) const { return stride * i; }
    };

    typedef typename thrust::counting_iterator<difference_type> CountingIterator;
    typedef typename thrust::transform_iterator<stride_functor, CountingIterator> TransformIterator;
    typedef typename thrust::permutation_iterator<Iterator, TransformIterator> PermutationIterator;

    /// type of the strided_range iterator
    typedef PermutationIterator iterator;

    /// construct strided_range for the range [first,last)
    strided_range(Iterator first, Iterator last, difference_type stride)
        : first(first)
        , last(last)
        , stride(stride) {}

    iterator begin() const {
        return PermutationIterator(first, TransformIterator(CountingIterator(0), stride_functor(stride)));
    }

    iterator end() const { return begin() + ((last - first) + (stride - 1)) / stride; }

  protected:
    Iterator first;
    Iterator last;
    difference_type stride;
};
}

#endif /* STRIDED_ITERATOR_H_ */
