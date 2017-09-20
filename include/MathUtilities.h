//
//  MathUtilities.h
//  asutilities
//
//  Created by Alessandro Saccoia on 9/15/17.
//
//

#ifndef MathUtilities_h
#define MathUtilities_h

namespace asu {

template <class DataT>
static inline size_t max_index( DataT *const input, const size_t size, float* value ) {
  size_t max = 0;
  for ( size_t i = 1; i < size; ++i ) {
    if ( input[i] > input[max] ) {
      *value = input[i];
      max = i;
    }
  }
  return max;
}

/**
 * Computes the mean
 */
template<class FTYPE>
inline typename FTYPE::value_type mean1D(const FTYPE& v) {
  typename FTYPE::value_type sum = std::accumulate(v.begin(), v.end(), 0.0);
  return sum / v.size();
}

/**
 * Computes the variance
 */
template<class FTYPE>
inline typename FTYPE::value_type variance1D(const FTYPE& v) {
  auto mean = mean1D(v);
  typename FTYPE::value_type sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
  return (sq_sum / v.size() - mean * mean);
}


}

#endif /* MathUtilities_h */
