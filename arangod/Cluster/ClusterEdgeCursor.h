////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_CLUSTER_CLUSTER_EDGE_CURSOR_H
#define ARANGOD_CLUSTER_CLUSTER_EDGE_CURSOR_H 1

#include "VocBase/TraverserOptions.h"

namespace arangodb {
class CollectionNameResolver;
namespace traverser {

class Traverser;

class ClusterEdgeCursor : public EdgeCursor {

 public:
  ClusterEdgeCursor(StringRef vid, uint64_t, ClusterTraverser*);

  ~ClusterEdgeCursor() {
  }

  bool next(std::function<void(arangodb::StringRef const&, arangodb::velocypack::Slice, size_t)> callback) override;

  void readAll(std::function<void(arangodb::StringRef const&, arangodb::velocypack::Slice, size_t&)> callback) override;

 private:

  std::vector<arangodb::velocypack::Slice> _edgeList;

  size_t _position;
  CollectionNameResolver const* _resolver;
  arangodb::traverser::Traverser* _traverser;
};
}
}

#endif
