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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_DISPATCHER_APPLICATION_DISPATCHER_H
#define ARANGOD_DISPATCHER_APPLICATION_DISPATCHER_H 1

#include "Basics/Common.h"

#include "ApplicationServer/ApplicationFeature.h"
#include "Scheduler/ApplicationScheduler.h"

namespace arangodb {
namespace rest {
class ApplicationScheduler;
class Dispatcher;

////////////////////////////////////////////////////////////////////////////////
/// @brief application server with dispatcher
////////////////////////////////////////////////////////////////////////////////

class ApplicationDispatcher : virtual public ApplicationFeature {
 private:
  ApplicationDispatcher(ApplicationDispatcher const&) = delete;
  ApplicationDispatcher& operator=(ApplicationDispatcher const&) = delete;

 public:
  ApplicationDispatcher();

  ~ApplicationDispatcher();

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the scheduler
  //////////////////////////////////////////////////////////////////////////////

  void setApplicationScheduler(ApplicationScheduler*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the dispatcher
  //////////////////////////////////////////////////////////////////////////////

  Dispatcher* dispatcher() const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief builds the dispatcher queue
  //////////////////////////////////////////////////////////////////////////////

  void buildStandardQueue(size_t nrThreads, size_t maxSize);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief builds the additional AQL dispatcher queue
  //////////////////////////////////////////////////////////////////////////////

  void buildAQLQueue(size_t nrThreads, size_t maxSize);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief builds an additional dispatcher queue
  //////////////////////////////////////////////////////////////////////////////

  void buildExtraQueue(size_t name, size_t nrThreads, size_t maxSize);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the number of used threads
  //////////////////////////////////////////////////////////////////////////////

  size_t numberOfThreads();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the processor affinity
  //////////////////////////////////////////////////////////////////////////////

  void setProcessorAffinity(std::vector<size_t> const& cores);

 public:
  void setupOptions(std::map<std::string, basics::ProgramOptionsDescription>&) override final;

  bool prepare() override final;

  bool start() override final;

  bool open() override final;

  void close() override final;

  void stop() override final;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief builds the dispatcher
  //////////////////////////////////////////////////////////////////////////////

  void buildDispatcher(Scheduler*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief builds the dispatcher reporter
  //////////////////////////////////////////////////////////////////////////////

  void buildDispatcherReporter();

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief application dispatcher
  //////////////////////////////////////////////////////////////////////////////

  ApplicationScheduler* _applicationScheduler;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief dispatcher
  //////////////////////////////////////////////////////////////////////////////

  Dispatcher* _dispatcher;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief interval for reports
  //////////////////////////////////////////////////////////////////////////////

  double _reportInterval;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief total number of standard threads
  //////////////////////////////////////////////////////////////////////////////

  size_t _nrStandardThreads;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief total number of AQL threads
  //////////////////////////////////////////////////////////////////////////////

  size_t _nrAQLThreads;
};
}
}

#endif
