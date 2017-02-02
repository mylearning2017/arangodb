////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 ArangoDB GmbH, Cologne, Germany
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
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_PREGEL_ALGORITHM_H
#define ARANGODB_PREGEL_ALGORITHM_H 1

#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>
#include <cstdint>
#include <functional>

#include "Basics/Common.h"
#include "GraphFormat.h"
#include "MessageCombiner.h"
#include "MessageFormat.h"
#include "WorkerContext.h"

namespace arangodb {
namespace pregel {

template <typename V, typename E, typename M>
class VertexComputation;

template <typename V, typename E, typename M>
class VertexCompensation;

class IAggregator;
class WorkerConfig;
class MasterContext;

struct IAlgorithm {
  virtual ~IAlgorithm() {}

  // virtual bool isFixpointAlgorithm() const {return false;}

  virtual bool supportsAsyncMode() const { return false; }

  virtual bool supportsCompensation() const { return false; }

  virtual bool supportsLazyLoading() const { return false; }

  virtual IAggregator* aggregator(std::string const& name) const {
    return nullptr;
  }

  virtual MasterContext* masterContext(VPackSlice userParams) const {
    return nullptr;
  }
  
  // ============= Configure runtime parameters ============
  
  virtual uint64_t maxGlobalSuperstep() const {
    return 500;
  }

  std::string const& name() const { return _name; }

 protected:
  IAlgorithm(std::string const& name) : _name(name){};

 private:
  std::string _name;
};

// specify serialization, whatever
template <typename V, typename E, typename M>
struct Algorithm : IAlgorithm {
 public:
  virtual WorkerContext* workerContext(VPackSlice userParams) const {
    return new WorkerContext(userParams);
  }
  virtual GraphFormat<V, E>* inputFormat() const = 0;
  virtual MessageFormat<M>* messageFormat() const = 0;
  virtual MessageCombiner<M>* messageCombiner() const { return nullptr; };
  virtual VertexComputation<V, E, M>* createComputation(
      WorkerConfig const*) const = 0;
  virtual VertexCompensation<V, E, M>* createCompensation(
      WorkerConfig const*) const {
    return nullptr;
  }
  virtual std::set<std::string> initialActiveSet() {
    return std::set<std::string>();
  }
  
  virtual uint32_t messageBatchSize(uint64_t gss,
                                    uint64_t sendCount,
                                    uint64_t threadCount,
                                    double superstepDuration) const {
    if (gss == 0) {
      return 500;
    } else {
      double msgsPerSec = sendCount / superstepDuration;
      msgsPerSec /= threadCount;  // per thread
      msgsPerSec *= 0.06;
      return msgsPerSec > 250.0 ? (uint32_t)msgsPerSec : 250;
    }
  }

 protected:
  Algorithm(std::string const& name) : IAlgorithm(name){};
};

template <typename V, typename E, typename M>
class SimpleAlgorithm : public Algorithm<V, E, M> {
 protected:
  std::string _sourceField, _resultField;

  SimpleAlgorithm(std::string const& name, VPackSlice userParams)
      : Algorithm<V, E, M>(name) {
    VPackSlice field = userParams.get("sourceField");
    _sourceField = field.isString() ? field.copyString() : "value";
    field = userParams.get("resultField");
    _resultField = field.isString() ? field.copyString() : "result";
  }
};
}
}
#endif
