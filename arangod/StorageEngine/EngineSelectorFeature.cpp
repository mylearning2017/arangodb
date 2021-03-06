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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include "ApplicationFeatures/ApplicationServer.h"
#include "Basics/FileUtils.h"
#include "EngineSelectorFeature.h"
#include "Logger/Logger.h"
#include "MMFiles/MMFilesEngine.h"
#include "ProgramOptions/ProgramOptions.h"
#include "ProgramOptions/Section.h"
#include "RestServer/DatabasePathFeature.h"
#include "RocksDBEngine/RocksDBEngine.h"
#include "StorageEngine/StorageEngine.h"

using namespace arangodb;
using namespace arangodb::options;

StorageEngine* EngineSelectorFeature::ENGINE = nullptr;

EngineSelectorFeature::EngineSelectorFeature(
    application_features::ApplicationServer* server)
    : ApplicationFeature(server, "EngineSelector"), _engine("auto") {
  setOptional(false);
  requiresElevatedPrivileges(false);
  startsAfter("Logger");
  startsAfter("DatabasePath");
}

void EngineSelectorFeature::collectOptions(std::shared_ptr<ProgramOptions> options) {
  options->addSection("server", "Server features");

  options->addOption("--server.storage-engine", 
                     "storage engine type",
                     new DiscreteValuesParameter<StringParameter>(&_engine, availableEngineNames()));
}

void EngineSelectorFeature::validateOptions(std::shared_ptr<ProgramOptions> options) {
  // engine from command line
  if(_engine == "auto"){
    _engine = MMFilesEngine::EngineName;
  }
}

void EngineSelectorFeature::prepare() {
  // read engine from file in database_directory ENGINE (mmfiles/rocksdb)
  auto databasePathFeature = application_features::ApplicationServer::getFeature<DatabasePathFeature>("DatabasePath");
  auto path = databasePathFeature->directory();
  _engineFilePath = basics::FileUtils::buildFilename(path, "ENGINE");
  LOG_TOPIC(DEBUG, Logger::STARTUP) << "looking for previously selected engine in file '" << _engineFilePath << "'";

  // file if engine in file does not match command-line option
  if (basics::FileUtils::isRegularFile(_engineFilePath)){
    std::string content = basics::FileUtils::slurp(_engineFilePath);
    if (content != _engine) {
      LOG_TOPIC(FATAL, Logger::STARTUP) << "engine selector - content of 'ENGINE' file and command-line option do not match: '" << content << "' != '" << _engine << "'";
      FATAL_ERROR_EXIT();
    }
  }

  // deactivate all engines but the selected one
  for (auto const& engine : availableEngines()) {
    StorageEngine* e = application_features::ApplicationServer::getFeature<StorageEngine>(engine.second);

    if (engine.first == _engine) {
      // this is the selected engine
      LOG_TOPIC(INFO, Logger::FIXME) << "using storage engine " << engine.first;
      e->enable();

      // register storage engine
      TRI_ASSERT(ENGINE == nullptr);
      ENGINE = e;
    } else {
      // turn off all other storage engines
      LOG_TOPIC(TRACE, Logger::STARTUP) << "disabling storage engine " << engine.first;
      e->disable();
    }
  }

  TRI_ASSERT(ENGINE != nullptr);
}

void EngineSelectorFeature::start() {
  //write engine File
  if(!basics::FileUtils::isRegularFile(_engineFilePath)){
    try {
      basics::FileUtils::spit(_engineFilePath, _engine);
    } catch (std::exception const& ex) {
      LOG_TOPIC(FATAL, Logger::STARTUP) << "engine selector - unable to write ENGINE file" << ex.what();
      FATAL_ERROR_EXIT();
    }
  }
}

void EngineSelectorFeature::unprepare() {
  // unregister storage engine
  ENGINE = nullptr;
}

// return the names of all available storage engines
std::unordered_set<std::string> EngineSelectorFeature::availableEngineNames() {
  std::unordered_set<std::string> result;
  for (auto const& it : availableEngines()) {
    result.emplace(it.first);
  }
  result.emplace("auto");
  return result;
}

// return all available storage engines
std::unordered_map<std::string, std::string> EngineSelectorFeature::availableEngines() { 
  return std::unordered_map<std::string, std::string>{
    {MMFilesEngine::EngineName, MMFilesEngine::FeatureName},
    {RocksDBEngine::EngineName, RocksDBEngine::FeatureName}
  };
}
  
char const* EngineSelectorFeature::engineName() {
  return ENGINE->typeName();
}
