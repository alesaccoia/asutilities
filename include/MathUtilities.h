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

static inline uint32_t next_pow_2 (uint32_t num) {
  uint32_t n = num > 0 ? num - 1 : 0;

  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;

  return n;
}

static inline uint32_t nearest_pow_2 (uint32_t num) {
  uint32_t n = next_pow_2(num);
  float n_1 = pow(2,log2(n) - 1);
  if ((n - num) < (num - n_1)) {
    return n;
  } else {
    return n_1;
  }
}

}

#endif /* MathUtilities_h */
