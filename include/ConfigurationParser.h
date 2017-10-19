//
//  ConfigurationParser.h
//  Mogees-lib2-tests
//
//  Created by Alessandro Saccoia on 5/25/17.
//  Copyright © 2017 mogees. All rights reserved.
//

#ifndef ConfigurationParser_h
#define ConfigurationParser_h

#include "yaml-cpp/yaml.h"
#include <fstream>


using namespace std;

typedef void (*updateStateCb)(void* state, std::string nodeName, const YAML::Node& scalarNode);
typedef void (*emitConfigurationCb)(void* state);

/**
 *  Parses a YAML configuration file, calling "emit" for each of the possible 
 *  permutations of parameters that are specified with the sequence syntax (i.e. [1,2,3]).
 *  The configurations are emitted in a depth-first way.
 *  Given a configuration like
 *  a: "foo"
 *  b: [1,2];
 *  c: [5.0,10.0];
 *  the 4 possible permutations will be emitted.
 *  @param update Called whenever a scalar, non sequence value is found, allows the user to
 *                update a state accordingly
 *  @param emit   Called when a permutation of the configuration has been parsed
 *
 */
inline void ParseConfiguration(const YAML::Node& rootNode, void* state, updateStateCb update, emitConfigurationCb emit) {
  std::list<std::pair<const YAML::Node, unsigned int>> stack;
  do {
    bool emitIteration = true;
    for (YAML::const_iterator it = rootNode.begin(); it != rootNode.end(); ++it){
      if (it->second.IsScalar()) {
        // we have a simple node that is not a Sequence
        update(state, it->first.as<std::string>(), it->second);
      } else {
        if (stack.empty()) {
          update(state, it->first.as<std::string>(), it->second[0]);
          stack.push_back(std::make_pair((it->second), 0));
        } else if (stack.back().first == (it->second)) {
          // we are passing on the topmost node whose values are being iterated
          // increase the current index
          ++stack.back().second;
          if (stack.back().second == it->second.size()) {
            // if we are past the size of the node, this configuration shouldn't be emitted
            stack.pop_back();
            emitIteration = false;
            break;
          } else {
            // if we are still iterating, go to the next value
            update(state, it->first.as<std::string>(), it->second[stack.back().second]);
          }
        } else {
          // Look the previous positions in the stack to check if this node is being already
          // iterated. todo this could be incorporated in the first condition
          auto stel = std::find_if(stack.begin(), stack.end(),[&] (const std::pair<const YAML::Node, unsigned int> n) { return n.first == it->second; });
          if (stel == stack.end()) {
            // Ths node is not being iterated, can be pushed at the top of the stack (depth-first)
            update(state, it->first.as<std::string>(), it->second[0]);
            stack.push_back(std::make_pair((it->second), 0));
          } else {
            // The node is being currently "held" on a value, emit the current value
            update(state, it->first.as<std::string>(), it->second[stel->second]);
          }
        }
      }
    }
    if (emitIteration) {
      emit(state);
    }
  } while (!stack.empty());
}





#endif /* ConfigurationParser_h */
