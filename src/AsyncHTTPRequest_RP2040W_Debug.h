/****************************************************************************************************************************
  AsyncHTTPRequest_RP2040W_Debug.h

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

#ifndef ASYNC_HTTP_REQUEST_RP2040W_DEBUG_H
#define ASYNC_HTTP_REQUEST_RP2040W_DEBUG_H

////////////////////////////////////////

#ifdef ASYNC_HTTP_RP2040W_DEBUG_PORT
  #define A_DBG_PORT      ASYNC_HTTP_RP2040W_DEBUG_PORT
#else
  #define A_DBG_PORT      Serial
#endif

////////////////////////////////////////

// Change _ASYNC_HTTP_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _ASYNC_HTTP_LOGLEVEL_
  #define _ASYNC_HTTP_LOGLEVEL_       1
#endif

////////////////////////////////////////

const char AHTTP_MARK[] = "[AHTTP] ";

#define AHTTP_PRINT_MARK   AHTTP_PRINT(AHTTP_MARK)
#define AHTTP_PRINT_SP     A_DBG_PORT.print(" ")

#define AHTTP_PRINT        A_DBG_PORT.print
#define AHTTP_PRINTLN      A_DBG_PORT.println

////////////////////////////////////////

#define AHTTP_LOGERROR(x)         if(_ASYNC_HTTP_LOGLEVEL_>0) { AHTTP_PRINT_MARK; AHTTP_PRINTLN(x); }
#define AHTTP_LOGERROR0(x)        if(_ASYNC_HTTP_LOGLEVEL_>0) { AHTTP_PRINT(x); }
#define AHTTP_LOGERROR1(x,y)      if(_ASYNC_HTTP_LOGLEVEL_>0) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINTLN(y); }
#define AHTTP_LOGERROR2(x,y,z)    if(_ASYNC_HTTP_LOGLEVEL_>0) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINTLN(z); }
#define AHTTP_LOGERROR3(x,y,z,w)  if(_ASYNC_HTTP_LOGLEVEL_>0) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINT(z); AHTTP_PRINT_SP; AHTTP_PRINTLN(w); }

////////////////////////////////////////

#define AHTTP_LOGWARN(x)          if(_ASYNC_HTTP_LOGLEVEL_>1) { AHTTP_PRINT_MARK; AHTTP_PRINTLN(x); }
#define AHTTP_LOGWARN0(x)         if(_ASYNC_HTTP_LOGLEVEL_>1) { AHTTP_PRINT(x); }
#define AHTTP_LOGWARN1(x,y)       if(_ASYNC_HTTP_LOGLEVEL_>1) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINTLN(y); }
#define AHTTP_LOGWARN2(x,y,z)     if(_ASYNC_HTTP_LOGLEVEL_>1) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINTLN(z); }
#define AHTTP_LOGWARN3(x,y,z,w)   if(_ASYNC_HTTP_LOGLEVEL_>1) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINT(z); AHTTP_PRINT_SP; AHTTP_PRINTLN(w); }

////////////////////////////////////////

#define AHTTP_LOGINFO(x)          if(_ASYNC_HTTP_LOGLEVEL_>2) { AHTTP_PRINT_MARK; AHTTP_PRINTLN(x); }
#define AHTTP_LOGINFO0(x)         if(_ASYNC_HTTP_LOGLEVEL_>2) { AHTTP_PRINT(x); }
#define AHTTP_LOGINFO1(x,y)       if(_ASYNC_HTTP_LOGLEVEL_>2) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINTLN(y); }
#define AHTTP_LOGINFO2(x,y,z)     if(_ASYNC_HTTP_LOGLEVEL_>2) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINTLN(z); }
#define AHTTP_LOGINFO3(x,y,z,w)   if(_ASYNC_HTTP_LOGLEVEL_>2) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINT(z); AHTTP_PRINT_SP; AHTTP_PRINTLN(w); }

////////////////////////////////////////

#define AHTTP_LOGDEBUG(x)         if(_ASYNC_HTTP_LOGLEVEL_>3) { AHTTP_PRINT_MARK; AHTTP_PRINTLN(x); }
#define AHTTP_LOGDEBUG0(x)        if(_ASYNC_HTTP_LOGLEVEL_>3) { AHTTP_PRINT(x); }
#define AHTTP_LOGDEBUG1(x,y)      if(_ASYNC_HTTP_LOGLEVEL_>3) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINTLN(y); }
#define AHTTP_LOGDEBUG2(x,y,z)    if(_ASYNC_HTTP_LOGLEVEL_>3) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINTLN(z); }
#define AHTTP_LOGDEBUG3(x,y,z,w)  if(_ASYNC_HTTP_LOGLEVEL_>3) { AHTTP_PRINT_MARK; AHTTP_PRINT(x); AHTTP_PRINT_SP; AHTTP_PRINT(y); AHTTP_PRINT_SP; AHTTP_PRINT(z); AHTTP_PRINT_SP; AHTTP_PRINTLN(w); }

////////////////////////////////////////

#endif    // ASYNC_HTTP_REQUEST_RP2040W_DEBUG_H

