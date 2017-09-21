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

}

#endif /* MathUtilities_h */
