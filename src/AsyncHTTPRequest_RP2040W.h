/****************************************************************************************************************************
  AsyncHTTPRequest_RP2040W.h

  For RP2040W with CYW43439 WiFi

  AsyncHTTPRequest_RP2040W is a library for the RP2040W with CYW43439 WiFi

  Based on and modified from asyncHTTPrequest Library (https://github.com/boblemaire/asyncHTTPrequest)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W

  Copyright (C) <2018>  <Bob Lemaire, IoTaWatt, Inc.>
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>

  Version: 1.3.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      14/08/2022 Initial coding for RP2040W with CYW43439 WiFi
  1.0.1   K Hoang      15/08/2022 Fix bug in examples
  1.1.0   K Hoang      01/09/2022 Fix bug. Improve debug messages. Optimize code
  1.1.1   K Hoang      19/10/2022 Not try to reconnect to the same host:port after connected
  1.2.0   K Hoang      21/10/2022 Fix bug. Clean up
  1.2.1   K Hoang      22/10/2022 Fix bug of wrong reqStates
  1.2.1   K Hoang      10/11/2022 Default to reconnect to the same host:port after connected for new HTTP sites. Fix bug
  1.3.0   K Hoang      01/02/2023 Fix _parseURL() bug
 *****************************************************************************************************************************/

#pragma once

#ifndef ASYNC_HTTP_REQUEST_RP2040W_H
#define ASYNC_HTTP_REQUEST_RP2040W_H

#include "AsyncHTTPRequest_RP2040W.hpp"
#include "AsyncHTTPRequest_RP2040W_Impl.h"

#endif    // ASYNC_HTTP_REQUEST_RP2040W_H
