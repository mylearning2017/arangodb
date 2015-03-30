////////////////////////////////////////////////////////////////////////////////
/// @brief please upgrade handler
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
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
/// @author Copyright 2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "RestPleaseUpgradeHandler.h"

using namespace triagens::basics;
using namespace triagens::rest;
using namespace triagens::arango;
using namespace std;

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

RestPleaseUpgradeHandler::RestPleaseUpgradeHandler (HttpRequest* request)
  : HttpHandler(request) {
}

// -----------------------------------------------------------------------------
// --SECTION--                                                   Handler methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

bool RestPleaseUpgradeHandler::isDirect () const {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

HttpHandler::status_t RestPleaseUpgradeHandler::execute () {
  _response = createResponse(HttpResponse::OK);
  _response->setContentType("text/plain; charset=utf-8");

  auto& buffer = _response->body();
  buffer.appendText("Database: ");
  buffer.appendText(_request->databaseName());
  buffer.appendText("\r\n\r\n");
  buffer.appendText("It appears that your database must be upgraded. ");
  buffer.appendText("Normally this can be done using\r\n\r\n");
  buffer.appendText("  /etc/init.d/arangodb stop\r\n");
  buffer.appendText("  /etc/init.d/arangodb upgrade\r\n");
  buffer.appendText("  /etc/init.d/arangodb start\r\n\r\n");
  buffer.appendText("Please check the log file for details.\r\n");

  return status_t(HANDLER_DONE);
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void RestPleaseUpgradeHandler::handleError (const TriagensError&) {
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void RestPleaseUpgradeHandler::handleError (const Exception&) {
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
