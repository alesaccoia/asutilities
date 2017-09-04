//
//  UtilityClasses.h
//  tetra
//
//  Created by Alessandro Saccoia on 4/14/17.
//  Copyright © 2017 alsc. All rights reserved.
//

#ifndef UtilityClasses_h
#define UtilityClasses_h

namespace asu {
namespace utilities {

struct noncopyable  
{  
  noncopyable() =default;  
  noncopyable(const noncopyable&) =delete;  
  noncopyable& operator=(const noncopyable&) =delete;  
};

}}


#endif /* UtilityClasses_h */
