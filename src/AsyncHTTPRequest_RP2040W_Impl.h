/****************************************************************************************************************************
  AsyncHTTPRequest_RP2040W_Impl.h

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

  Version: 1.2.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      14/08/2022 Initial coding for RP2040W with CYW43439 WiFi
  1.0.1   K Hoang      15/08/2022 Fix bug in examples
  1.1.0   K Hoang      01/09/2022 Fix bug. Improve debug messages. Optimize code
  1.1.1   K Hoang      19/10/2022 Not try to reconnect to the same host:port after connected
  1.2.0   K Hoang      21/10/2022 Fix bug. Clean up
  1.2.1   K Hoang      22/10/2022 Fix bug of wrong reqStates
  1.2.1   K Hoang      10/11/2022 Default to reconnect to the same host:port after connected for new HTTP sites. Fix bug
 *****************************************************************************************************************************/

#pragma once

#ifndef ASYNC_HTTP_REQUEST_RP2040W_IMPL_H
#define ASYNC_HTTP_REQUEST_RP2040W_IMPL_H

#define CANT_SEND_BAD_REQUEST       ("Can't send() bad request")

// Merge xbuf
////////////////////////////////////////

xbuf::xbuf(const uint16_t segSize) : _head(nullptr), _tail(nullptr), _used(0), _free(0), _offset(0)
{
  _segSize = (segSize + 3) & -4;//((segSize + 3) >> 2) << 2;
}

////////////////////////////////////////

xbuf::~xbuf()
{
  flush();
}

////////////////////////////////////////

size_t xbuf::write(const uint8_t byte)
{
  return write((uint8_t*) &byte, 1);
}

////////////////////////////////////////

size_t xbuf::write(const char* buf)
{
  return write((uint8_t*)buf, strlen(buf));
}

////////////////////////////////////////

size_t xbuf::write(const String& string)
{
  return write((uint8_t*)string.c_str(), string.length());
}

////////////////////////////////////////

size_t xbuf::write(const uint8_t* buf, const size_t len)
{
  size_t supply = len;

  while (supply)
  {
    if (!_free)
    {
      addSeg();
    }

    size_t demand = _free < supply ? _free : supply;
    memcpy(_tail->data + ((_offset + _used) % _segSize), buf + (len - supply), demand);
    _free -= demand;
    _used += demand;
    supply -= demand;
  }

  return len;
}

////////////////////////////////////////

size_t xbuf::write(xbuf* buf, const size_t len)
{
  size_t supply = len;

  if (supply > buf->available())
  {
    supply = buf->available();
  }

  size_t read = 0;

  while (supply)
  {
    if (!_free)
    {
      addSeg();
    }

    size_t demand = _free < supply ? _free : supply;
    read += buf->read(_tail->data + ((_offset + _used) % _segSize), demand);
    _free -= demand;
    _used += demand;
    supply -= demand;
  }

  return read;
}

////////////////////////////////////////

uint8_t xbuf::read()
{
  uint8_t byte = 0;
  read((uint8_t*) &byte, 1);

  return byte;
}

////////////////////////////////////////

uint8_t xbuf::peek()
{
  uint8_t byte = 0;
  peek((uint8_t*) &byte, 1);

  return byte;
}

////////////////////////////////////////

size_t xbuf::read(uint8_t* buf, const size_t len)
{
  size_t read = 0;

  while (read < len && _used)
  {
    size_t supply = (_offset + _used) > _segSize ? _segSize - _offset : _used;
    size_t demand = len - read;
    size_t chunk = supply < demand ? supply : demand;

    memcpy(buf + read, _head->data + _offset, chunk);
    _offset += chunk;
    _used -= chunk;
    read += chunk;

    if (_offset == _segSize)
    {
      remSeg();
      _offset = 0;
    }
  }

  if ( ! _used)
  {
    flush();
  }

  return read;
}

////////////////////////////////////////

size_t xbuf::peek(uint8_t* buf, const size_t len)
{
  size_t read   = 0;
  xseg* seg     = _head;
  size_t offset = _offset;
  size_t used   = _used;

  while (read < len && used)
  {
    size_t supply = (offset + used) > _segSize ? _segSize - offset : used;
    size_t demand = len - read;
    size_t chunk  = supply < demand ? supply : demand;

    memcpy(buf + read, seg->data + offset, chunk);

    offset  += chunk;
    used    -= chunk;
    read    += chunk;

    if (offset == _segSize)
    {
      seg = seg->next;
      offset = 0;
    }
  }

  return read;
}

////////////////////////////////////////

size_t xbuf::available()
{
  return _used;
}

////////////////////////////////////////

int xbuf::indexOf(const char target, const size_t begin)
{
  char targetstr[2] = " ";
  targetstr[0] = target;

  return indexOf(targetstr, begin);
}

////////////////////////////////////////

int xbuf::indexOf(const char* target, const size_t begin)
{
  size_t targetLen = strlen(target);

  if (targetLen > _segSize || targetLen > _used)
    return -1;

  size_t searchPos = _offset + begin;
  size_t searchEnd = _offset + _used - targetLen;

  if (searchPos > searchEnd)
    return -1;

  size_t searchSeg = searchPos / _segSize;
  xseg* seg = _head;

  while (searchSeg)
  {
    seg = seg->next;
    searchSeg --;
  }

  size_t segPos = searchPos % _segSize;

  while (searchPos <= searchEnd)
  {
    size_t compLen = targetLen;

    if (compLen <= (_segSize - segPos))
    {
      if (memcmp(target, seg->data + segPos, compLen) == 0)
      {
        return searchPos - _offset;
      }
    }
    else
    {
      size_t compLen = _segSize - segPos;

      if (memcmp(target, seg->data + segPos, compLen) == 0)
      {
        compLen = targetLen - compLen;

        if (memcmp(target + targetLen - compLen, seg->next->data, compLen) == 0)
        {
          return searchPos - _offset;
        }
      }
    }

    searchPos++;
    segPos++;

    if (segPos == _segSize)
    {
      seg = seg->next;
      segPos = 0;
    }
  }

  return -1;
}

////////////////////////////////////////

String xbuf::readStringUntil(const char target)
{
  return readString(indexOf(target) + 1);
}

////////////////////////////////////////

String xbuf::readStringUntil(const char* target)
{
  int index = indexOf(target);

  if (index < 0)
    return String();

  return readString(index + strlen(target));
}

////////////////////////////////////////

String xbuf::readString(int endPos)
{
  String result;

  if ( ! result.reserve(endPos + 1))
  {
    return result;
  }

  if (endPos > _used)
  {
    endPos = _used;
  }

  if (endPos > 0 && result.reserve(endPos + 1))
  {
    while (endPos--)
    {
      result += (char)_head->data[_offset++];
      _used--;

      if (_offset >= _segSize)
      {
        remSeg();
      }
    }
  }

  return result;
}

////////////////////////////////////////

String xbuf::peekString(int endPos)
{
  String result;

  xseg* seg     = _head;
  size_t offset = _offset;

  if (endPos > _used)
  {
    endPos = _used;
  }

  if (endPos > 0 && result.reserve(endPos + 1))
  {
    while (endPos--)
    {
      result += (char)seg->data[offset++];

      if ( offset >= _segSize)
      {
        seg = seg->next;
        offset = 0;
      }
    }
  }

  return result;
}

////////////////////////////////////////

void xbuf::flush()
{
  while (_head)
    remSeg();

  _tail = nullptr;
  _offset = 0;
  _used = 0;
  _free = 0;
}

////////////////////////////////////////

void xbuf::addSeg()
{
  if (_tail)
  {
    _tail->next = (xseg*) new uint32_t[_segSize / 4 + 1];

    if (_tail->next == NULL)
    {
      AHTTP_LOGERROR(("xbuf::addSeg: NULL next"));
    }
    else
    {
      _tail = _tail->next;
    }
  }
  else
  {
    _tail = _head = (xseg*) new uint32_t[_segSize / 4 + 1];

    if (_tail == NULL)
      AHTTP_LOGERROR(("xbuf::addSeg: NULL tail"));
  }

  if (_tail)
    _tail->next = nullptr;

  _free += _segSize;
}

////////////////////////////////////////

void xbuf::remSeg()
{
  if (_head)
  {
    xseg *next = _head->next;
    delete[] (uint32_t*) _head;
    _head = next;

    if ( ! _head)
    {
      _tail = nullptr;
    }
  }

  _offset = 0;
}

////////////////////////////////////////
////////////////////////////////////////

AsyncHTTPRequest::AsyncHTTPRequest(): _readyState(readyStateUnsent), _HTTPcode(0), _chunked(false),
  _debug(DEBUG_IOTA_HTTP_SET)
  , _timeout(DEFAULT_RX_TIMEOUT), _lastActivity(0), _requestStartTime(0), _requestEndTime(0), _URL(nullptr)
  , _connectedHost(nullptr), _connectedPort(-1), _client(nullptr), _contentLength(0), _contentRead(0)
  , _readyStateChangeCB(nullptr), _readyStateChangeCBarg(nullptr), _onDataCB(nullptr), _onDataCBarg(nullptr)
  , _request(nullptr), _response(nullptr), _chunks(nullptr), _headers(nullptr)
{
}

////////////////////////////////////////

AsyncHTTPRequest::~AsyncHTTPRequest()
{
  if (_client)
    _client->close(true);

  SAFE_DELETE(_URL)
  SAFE_DELETE(_headers)
  SAFE_DELETE(_request)
  SAFE_DELETE(_response)
  SAFE_DELETE(_chunks)
  SAFE_DELETE_ARRAY(_connectedHost)
}

////////////////////////////////////////

void AsyncHTTPRequest::setDebug(bool debug)
{
  if (_debug || debug)
  {
    _debug = true;
  }

  _debug = debug;
}

////////////////////////////////////////

bool AsyncHTTPRequest::debug()
{
  return (_debug);
}

////////////////////////////////////////

bool  AsyncHTTPRequest::open(const char* method, const char* URL)
{
  AHTTP_LOGDEBUG3("open(", method, ", url =", URL);

  if (_readyState != readyStateUnsent && _readyState != readyStateDone)
  {
    AHTTP_LOGERROR("open: not ready");

    return false;
  }

  _requestStartTime = millis();

  SAFE_DELETE(_URL)
  SAFE_DELETE(_headers)
  SAFE_DELETE(_request)
  SAFE_DELETE(_response)
  SAFE_DELETE(_chunks)

  _URL          = nullptr;
  _headers      = nullptr;
  _response     = nullptr;
  _request      = nullptr;
  _chunks       = nullptr;
  _chunked      = false;
  _contentRead  = 0;
  _readyState   = readyStateUnsent;

  _requestReadyToSend = false;

  if (strcmp(method, "GET") == 0)
  {
    _HTTPmethod = HTTPmethodGET;
  }
  else if (strcmp(method, "POST") == 0)
  {
    _HTTPmethod = HTTPmethodPOST;
  }
  else if (strcmp(method, "PUT") == 0)
  {
    _HTTPmethod = HTTPmethodPUT;
  }
  else if (strcmp(method, "PATCH") == 0)
  {
    _HTTPmethod = HTTPmethodPATCH;
  }
  else if (strcmp(method, "DELETE") == 0)
  {
    _HTTPmethod = HTTPmethodDELETE;
  }
  else if (strcmp(method, "HEAD") == 0)
  {
    _HTTPmethod = HTTPmethodHEAD;
  }
  else
  {
    AHTTP_LOGERROR("open: Bad method");

    return false;
  }

  if (!_parseURL(URL))
  {
    AHTTP_LOGERROR("open: error parsing URL");

    return false;
  }

#if NOT_SEND_HEADER_AFTER_CONNECTED

  if ( _client && _client->connected() )
  {
    if ( (strcmp(_URL->host, _connectedHost) == 0) && (_URL->port == _connectedPort) )
    {
      AHTTP_LOGINFO(F("open: already connected"));

      _lastActivity = millis();

      _requestReadyToSend = true;

      return _connect();
    }
    else
    {
      AHTTP_LOGINFO(F("open: not connected: different host or port"));

      return false;
    }
  }

#else

  if ( _client && _client->connected() && (strcmp(_URL->host, _connectedHost) != 0 || _URL->port != _connectedPort))
  {
    AHTTP_LOGERROR(F("open: not connected"));

    return false;
  }

#endif

  char* hostName = new char[strlen(_URL->host) + 10];

  if (hostName)
  {
    sprintf(hostName, "%s:%d", _URL->host, _URL->port);
    _addHeader("host", hostName);

    AHTTP_LOGINFO1("open: connecting to hostname =", hostName);

    SAFE_DELETE_ARRAY(hostName)

    _lastActivity = millis();

    _requestReadyToSend = true;

    return _connect();
  }
  else
  {
    AHTTP_LOGERROR("open: error alloc");

    return false;
  }
}
////////////////////////////////////////

void AsyncHTTPRequest::onReadyStateChange(readyStateChangeCB cb, void* arg)
{
  _readyStateChangeCB    = cb;
  _readyStateChangeCBarg = arg;
}

////////////////////////////////////////

void  AsyncHTTPRequest::setTimeout(int seconds)
{
  _timeout = seconds;
}

////////////////////////////////////////

bool  AsyncHTTPRequest::send()
{
  if (!_requestReadyToSend)
  {
    AHTTP_LOGERROR(CANT_SEND_BAD_REQUEST);

    return false;
  }

  MUTEX_LOCK(false)

  if ( ! _buildRequest())
    return false;

  _send();

  _AHTTP_unlock;

  return true;
}

////////////////////////////////////////

bool AsyncHTTPRequest::send(const String& body)
{
  if (!_requestReadyToSend)
  {
    AHTTP_LOGERROR(CANT_SEND_BAD_REQUEST);

    return false;
  }

  MUTEX_LOCK(false)

  _addHeader("Content-Length", String(body.length()).c_str());

  if ( ! _buildRequest())
  {
    _AHTTP_unlock;

    return false;
  }

  _request->write(body);
  _send();

  _AHTTP_unlock;

  return true;
}

////////////////////////////////////////

bool  AsyncHTTPRequest::send(const char* body)
{
  if (!_requestReadyToSend)
  {
    AHTTP_LOGERROR(CANT_SEND_BAD_REQUEST);

    return false;
  }

  MUTEX_LOCK(false)

  _addHeader("Content-Length", String(strlen(body)).c_str());

  if ( ! _buildRequest())
  {
    _AHTTP_unlock;

    return false;
  }

  _request->write(body);
  _send();

  _AHTTP_unlock;

  return true;
}

////////////////////////////////////////

bool  AsyncHTTPRequest::send(const uint8_t* body, size_t len)
{
  if (!_requestReadyToSend)
  {
    AHTTP_LOGERROR(CANT_SEND_BAD_REQUEST);

    return false;
  }

  MUTEX_LOCK(false)

  _addHeader("Content-Length", String(len).c_str());

  if ( ! _buildRequest())
  {
    _AHTTP_unlock;

    return false;
  }

  _request->write(body, len);
  _send();

  _AHTTP_unlock;

  return true;
}

////////////////////////////////////////

bool AsyncHTTPRequest::send(xbuf* body, size_t len)
{
  if (!_requestReadyToSend)
  {
    AHTTP_LOGERROR(CANT_SEND_BAD_REQUEST);

    return false;
  }

  MUTEX_LOCK(false)

  _addHeader("Content-Length", String(len).c_str());

  if ( ! _buildRequest())
  {
    _AHTTP_unlock;

    return false;
  }

  _request->write(body, len);
  _send();

  _AHTTP_unlock;

  return true;
}

////////////////////////////////////////

void AsyncHTTPRequest::abort()
{
  AHTTP_LOGERROR("abort()");

  if (! _client)
  {
    return;
  }

  MUTEX_LOCK_NR

  _client->abort();

  _AHTTP_unlock;
}
////////////////////////////////////////

reqStates   AsyncHTTPRequest::readyState()
{
  return _readyState;
}

////////////////////////////////////////

int AsyncHTTPRequest::responseHTTPcode()
{
  return _HTTPcode;
}

////////////////////////////////////////

String AsyncHTTPRequest::responseHTTPString()
{
  switch (_HTTPcode)
  {
    case 0:
      return ("OK");

    case HTTPCODE_CONNECTION_REFUSED:
      return ("CONNECTION_REFUSED");

    case HTTPCODE_SEND_HEADER_FAILED:
      return ("SEND_HEADER_FAILED");

    case HTTPCODE_SEND_PAYLOAD_FAILED:
      return ("SEND_PAYLOAD_FAILED");

    case HTTPCODE_NOT_CONNECTED:
      return ("NOT_CONNECTED");

    case HTTPCODE_CONNECTION_LOST:
      return ("CONNECTION_LOST");

    case HTTPCODE_NO_STREAM:
      return ("NO_STREAM");

    case HTTPCODE_NO_HTTP_SERVER:
      return ("NO_HTTP_SERVER");

    case HTTPCODE_TOO_LESS_RAM:
      return ("TOO_LESS_RAM");

    case HTTPCODE_ENCODING:
      return ("ENCODING");

    case HTTPCODE_STREAM_WRITE:
      return ("STREAM_WRITE");

    case HTTPCODE_TIMEOUT:
      return ("TIMEOUT");

    // HTTP positive code
    case 100:
      return ("Continue");

    case 101:
      return ("Switching Protocols");

    case 200:
      return ("HTTP OK");

    case 201:
      return ("Created");

    case 202:
      return ("Accepted");

    case 203:
      return ("Non-Authoritative Information");

    case 204:
      return ("No Content");

    case 205:
      return ("Reset Content");

    case 206:
      return ("Partial Content");

    case 300:
      return ("Multiple Choices");

    case 301:
      return ("Moved Permanently");

    case 302:
      return ("Found");

    case 303:
      return ("See Other");

    case 304:
      return ("Not Modified");

    case 305:
      return ("Use Proxy");

    case 307:
      return ("Temporary Redirect");

    case 400:
      return ("Bad Request");

    case 401:
      return ("Unauthorized");

    case 402:
      return ("Payment Required");

    case 403:
      return ("Forbidden");

    case 404:
      return ("Not Found");

    case 405:
      return ("Method Not Allowed");

    case 406:
      return ("Not Acceptable");

    case 407:
      return ("Proxy Authentication Required");

    case 408:
      return ("Request Time-out");

    case 409:
      return ("Conflict");

    case 410:
      return ("Gone");

    case 411:
      return ("Length Required");

    case 412:
      return ("Precondition Failed");

    case 413:
      return ("Request Entity Too Large");

    case 414:
      return ("Request-URI Too Large");

    case 415:
      return ("Unsupported Media Type");

    case 416:
      return ("Requested range not satisfiable");

    case 417:
      return ("Expectation Failed");

    case 500:
      return ("Internal Server Error");

    case 501:
      return ("Not Implemented");

    case 502:
      return ("Bad Gateway");

    case 503:
      return ("Service Unavailable");

    case 504:
      return ("Gateway Time-out");

    case 505:
      return ("HTTP Version not supported");

    default:
      return "UNKNOWN";
  }
}

////////////////////////////////////////

String AsyncHTTPRequest::responseText()
{
  MUTEX_LOCK(String())

  if ( ! _response || _readyState < readyStateLoading || ! available())
  {
    AHTTP_LOGWARN(("responseText() no data"));

    _AHTTP_unlock;

    return String();
  }

  size_t avail = available();

  String localString = _response->readString(avail);

  if (localString.length() < avail)
  {
    AHTTP_LOGWARN(("!responseText() no buffer"))
    _HTTPcode = HTTPCODE_TOO_LESS_RAM;
    _client->abort();
    _AHTTP_unlock;

    return String();
  }

  _contentRead += localString.length();

  _AHTTP_unlock;

  return localString;
}

////////////////////////////////////////

#define GLOBAL_STR_LEN      (16 * 1024)

////////////////////////////////////////

char globalLongString[GLOBAL_STR_LEN + 1];

////////////////////////////////////////

char* AsyncHTTPRequest::responseLongText()
{
  MUTEX_LOCK(NULL)

  if ( ! _response || _readyState < readyStateLoading || ! available())
  {
    AHTTP_LOGWARN("responseText() no data");

    _AHTTP_unlock;

    return NULL;
  }

  size_t avail = available();
  size_t lenToCopy = (avail <= GLOBAL_STR_LEN) ? avail : GLOBAL_STR_LEN;

  strncpy(globalLongString, _response->readString(avail).c_str(), lenToCopy );
  globalLongString[ lenToCopy + 1 ] = 0;

  _contentRead += _response->readString(avail).length();

  _AHTTP_unlock;

  return globalLongString;
}

////////////////////////////////////////

size_t AsyncHTTPRequest::responseRead(uint8_t* buf, size_t len)
{
  if ( ! _response || _readyState < readyStateLoading || ! available())
  {
    AHTTP_LOGWARN("responseRead() no data");

    return 0;
  }

  MUTEX_LOCK(0)

  size_t avail = available() > len ? len : available();
  _response->read(buf, avail);

  _contentRead += avail;

  _AHTTP_unlock;

  return avail;
}

////////////////////////////////////////

size_t  AsyncHTTPRequest::available()
{
  if (_readyState < readyStateLoading)
    return 0;

  if (_chunked && (_contentLength - _contentRead) < _response->available())
  {
    return _contentLength - _contentRead;
  }

  return _response->available();
}

////////////////////////////////////////

size_t  AsyncHTTPRequest::responseLength()
{
  if (_readyState < readyStateLoading)
    return 0;

  return _contentLength;
}

////////////////////////////////////////

void  AsyncHTTPRequest::onData(onDataCB cb, void* arg)
{
  _onDataCB    = cb;
  _onDataCBarg = arg;
}

////////////////////////////////////////

uint32_t AsyncHTTPRequest::elapsedTime()
{
  if (_readyState <= readyStateOpened)
    return 0;

  if (_readyState != readyStateDone)
  {
    return millis() - _requestStartTime;
  }

  return _requestEndTime - _requestStartTime;
}

////////////////////////////////////////

String AsyncHTTPRequest::version()
{
  return String(ASYNC_HTTP_REQUEST_RP2040W_VERSION);
}

/*______________________________________________________________________________________________________________

               PPPP    RRRR     OOO    TTTTT   EEEEE    CCC    TTTTT   EEEEE   DDDD
               P   P   R   R   O   O     T     E       C   C     T     E       D   D
               PPPP    RRRR    O   O     T     EEE     C         T     EEE     D   D
               P       R  R    O   O     T     E       C   C     T     E       D   D
               P       R   R    OOO      T     EEEEE    CCC      T     EEEEE   DDDD
  _______________________________________________________________________________________________________________*/

////////////////////////////////////////

bool  AsyncHTTPRequest::_parseURL(const char* url)
{
  return _parseURL(String(url));
}

////////////////////////////////////////

bool  AsyncHTTPRequest::_parseURL(const String& url)
{
  SAFE_DELETE(_URL)

  int hostBeg = 0;

  _URL = new URL;

  if (_URL)
  {
    _URL->scheme = new char[strlen(ASYNC_HTTP_PREFIX) + 1];

    if (! (_URL->scheme) )
      return false;
  }
  else
    return false;

  strcpy(_URL->scheme, ASYNC_HTTP_PREFIX);

  if (url.substring(0, strlen(ASYNC_HTTP_PREFIX)).equalsIgnoreCase(ASYNC_HTTP_PREFIX))
  {
    hostBeg += strlen(ASYNC_HTTP_PREFIX);
  }
  else if (url.substring(0, strlen(ASYNC_HTTPS_PREFIX)).equalsIgnoreCase(ASYNC_HTTPS_PREFIX) )
  {
    return false;
  }

  int pathBeg = url.indexOf('/', hostBeg);

  if (pathBeg < 0)
    return false;

  int hostEnd = pathBeg;
  int portBeg = url.indexOf(':', hostBeg);

  if (portBeg > 0 && portBeg < pathBeg)
  {
    _URL->port = url.substring(portBeg + 1, pathBeg).toInt();
    hostEnd = portBeg;
  }

  _URL->host = new char[hostEnd - hostBeg + 1];

  if (_URL->host == nullptr)
    return false;

  strcpy(_URL->host, url.substring(hostBeg, hostEnd).c_str());

  int queryBeg = url.indexOf('?');

  if (queryBeg < 0)
    queryBeg = url.length();

  _URL->path = new char[queryBeg - pathBeg + 1];

  if (_URL->path == nullptr)
    return false;

  strcpy(_URL->path, url.substring(pathBeg, queryBeg).c_str());

  _URL->query = new char[url.length() - queryBeg + 1];

  if (_URL->query == nullptr)
    return false;

  strcpy(_URL->query, url.substring(queryBeg).c_str());

  return true;
}

////////////////////////////////////////

bool  AsyncHTTPRequest::_connect()
{
  if ( ! _client)
  {
    _client = new AsyncClient();

    if (! _client)
    {
      return false;
    }
  }

  SAFE_DELETE_ARRAY(_connectedHost)

  _connectedHost = new char[strlen(_URL->host) + 1];

  if (_connectedHost == nullptr)
    return false;

  strcpy(_connectedHost, _URL->host);
  _connectedPort = _URL->port;

  _client->onConnect([](void *obj, AsyncClient * client)
  {
    ((AsyncHTTPRequest*)(obj))->_onConnect(client);
  }, this);

  _client->onDisconnect([](void *obj, AsyncClient * client)
  {
    ((AsyncHTTPRequest*)(obj))->_onDisconnect(client);
  }, this);

  _client->onPoll([](void *obj, AsyncClient * client)
  {
    ((AsyncHTTPRequest*)(obj))->_onPoll(client);
  }, this);

  _client->onError([](void *obj, AsyncClient * client, uint32_t error)
  {
    ((AsyncHTTPRequest*)(obj))->_onError(client, error);
  }, this);

  if ( ! _client->connected())
  {
    AHTTP_LOGINFO3(("_client->connecting to"), _URL->host, (","), _URL->port);

    if ( ! _client->connect(_URL->host, _URL->port))
    {
      AHTTP_LOGERROR3("client.connect failed:", _URL->host, ", port =", _URL->port);

      _HTTPcode = HTTPCODE_NOT_CONNECTED;
      _setReadyState(readyStateDone);

      return false;
    }
    else
    {
      AHTTP_LOGINFO3(("client.connect OK to"), _URL->host, (","), _URL->port);
    }
  }
  else
  {
    _onConnect(_client);
  }

  _lastActivity = millis();

  return true;
}

////////////////////////////////////////

bool   AsyncHTTPRequest::_buildRequest()
{
  // Build the header.
  if ( ! _request)
  {
    _request = new xbuf;

    if ( ! _request)
      return false;
  }

  _request->write(_HTTPmethodStringwithSpace[_HTTPmethod]);
  _request->write(_URL->path);
  _request->write(_URL->query);

  _request->write(" HTTP/1.1\r\n");

  SAFE_DELETE(_URL)

  _URL = nullptr;
  header* hdr = _headers;

  while (hdr)
  {
    _request->write(hdr->name);
    _request->write(':');
    _request->write(hdr->value);
    _request->write("\r\n");

    hdr = hdr->next;
  }

  SAFE_DELETE(_headers)

  _headers = nullptr;
  _request->write("\r\n");

  return true;
}

////////////////////////////////////////

size_t  AsyncHTTPRequest::_send()
{
  if ( ! _request)
    return 0;

  if ( ! _client->connected())
  {
    // KH fix bug https://github.com/khoih-prog/AsyncHTTPRequest_Generic/issues/38
    _timeout = DEFAULT_RX_TIMEOUT;

    return 0;
  }
  else if ( ! _client->canSend())
  {
    return 0;
  }

  size_t supply = _request->available();
  size_t demand = _client->space();

  if (supply > demand)
    supply = demand;

  size_t sent = 0;

#define MAX_CHUNK_SIZE       255

  uint8_t* temp = new uint8_t[MAX_CHUNK_SIZE + 1];

  if (!temp)
    return 0;

  while (supply)
  {
    size_t chunk = supply < MAX_CHUNK_SIZE ? supply : MAX_CHUNK_SIZE;

    memset(temp, 0, MAX_CHUNK_SIZE + 1);
    supply  -= _request->read(temp, chunk);
    sent    += _client->add((char*)temp, chunk);
  }

  SAFE_DELETE_ARRAY(temp)

  if (_request->available() == 0)
  {
    SAFE_DELETE(_request)

    _request = nullptr;

    AHTTP_LOGDEBUG("_send: _request->available() == 0 => _request = nullptr");
  }

  _client->send();

  _lastActivity = millis();

  AHTTP_LOGDEBUG1("_send: _client->send() @ millis =", _lastActivity);

  return sent;
}

////////////////////////////////////////

void  AsyncHTTPRequest::_setReadyState(reqStates newState)
{
  if (_readyState != newState)
  {
    _readyState = newState;

    if (_readyStateChangeCB)
    {
      _readyStateChangeCB(_readyStateChangeCBarg, this, _readyState);
    }
  }
}

////////////////////////////////////////

void  AsyncHTTPRequest::_processChunks()
{
  while (_chunks->available())
  {
    size_t _chunkRemaining = _contentLength - _contentRead - _response->available();
    _chunkRemaining -= _response->write(_chunks, _chunkRemaining);

    if (_chunks->indexOf("\r\n") == -1)
    {
      return;
    }

    String chunkHeader = _chunks->readStringUntil("\r\n");

    size_t chunkLength = strtol(chunkHeader.c_str(), nullptr, 16);
    _contentLength += chunkLength;

    if (chunkLength == 0)
    {
      char* connectionHdr = respHeaderValue("connection");

      if (connectionHdr && (strcasecmp_P(connectionHdr, PSTR("close")) == 0))
      {
        _client->close();
      }

      _requestEndTime = millis();
      _lastActivity = 0;
      _timeout = 0;
      _setReadyState(readyStateDone);

      return;
    }
  }
}

/*______________________________________________________________________________________________________________

  EEEEE   V   V   EEEEE   N   N   TTTTT         H   H    AAA    N   N   DDDD    L       EEEEE   RRRR     SSS
  E       V   V   E       NN  N     T           H   H   A   A   NN  N   D   D   L       E       R   R   S
  EEE     V   V   EEE     N N N     T           HHHHH   AAAAA   N N N   D   D   L       EEE     RRRR     SSS
  E        V V    E       N  NN     T           H   H   A   A   N  NN   D   D   L       E       R  R        S
  EEEEE     V     EEEEE   N   N     T           H   H   A   A   N   N   DDDD    LLLLL   EEEEE   R   R    SSS
  _______________________________________________________________________________________________________________*/

////////////////////////////////////////

void  AsyncHTTPRequest::_onConnect(AsyncClient* client)
{
  MUTEX_LOCK_NR

  _client = client;
  _setReadyState(readyStateOpened);

  _response = new xbuf;

  if (!_response)
  {
    _AHTTP_unlock;

    return;
  }

  _contentLength = 0;
  _contentRead   = 0;
  _chunked       = false;

  _client->onAck([](void* obj, AsyncClient * client, size_t len, uint32_t time)
  {
    (void) client;
    (void) len;
    (void) time;

    ((AsyncHTTPRequest*)(obj))->_send();
  }, this);

  _client->onData([](void* obj, AsyncClient * client, void* data, size_t len)
  {
    (void) client;

    ((AsyncHTTPRequest*)(obj))->_onData(data, len);
  }, this);

  if (_client->canSend())
  {
    _send();
  }

  _lastActivity = millis();

  _AHTTP_unlock;
}

////////////////////////////////////////

void  AsyncHTTPRequest::_onPoll(AsyncClient* client)
{
  (void) client;

  MUTEX_LOCK_NR

  if (_timeout && (millis() - _lastActivity) > (_timeout * 1000))
  {
    _client->close();
    _HTTPcode = HTTPCODE_TIMEOUT;
  }

  if (_onDataCB && available())
  {
    _onDataCB(_onDataCBarg, this, available());
  }

  _AHTTP_unlock;
}

////////////////////////////////////////

void  AsyncHTTPRequest::_onError(AsyncClient* client, int8_t error)
{
  (void) client;

  AHTTP_LOGERROR1("_onError handler error =", error);

  _HTTPcode = error;
}

////////////////////////////////////////

void  AsyncHTTPRequest::_onDisconnect(AsyncClient* client)
{
  (void) client;

  MUTEX_LOCK_NR

  if (_readyState < readyStateOpened)
  {
    _HTTPcode = HTTPCODE_NOT_CONNECTED;
  }
  else if (_HTTPcode > 0 &&
           (_readyState < readyStateHdrsRecvd || (_contentRead + _response->available()) < _contentLength))
  {
    AHTTP_LOGDEBUG("_onDisconnect: HTTPCODE_CONNECTION_LOST");
    _HTTPcode = HTTPCODE_CONNECTION_LOST;
  }

  SAFE_DELETE(_client)

  _client = nullptr;

  SAFE_DELETE_ARRAY(_connectedHost)

  _connectedHost = nullptr;

  _connectedPort  = -1;
  _requestEndTime = millis();
  _lastActivity   = 0;
  _setReadyState(readyStateDone);

  _AHTTP_unlock;
}

////////////////////////////////////////

void  AsyncHTTPRequest::_onData(void* Vbuf, size_t len)
{
  MUTEX_LOCK_NR

  _lastActivity = millis();

  // Transfer data to xbuf
  if (_chunks)
  {
    _chunks->write((uint8_t*)Vbuf, len);

    _processChunks();
  }
  else
  {
    _response->write((uint8_t*)Vbuf, len);
  }

  // if headers not complete, collect them. If still not complete, just return.
  if (_readyState == readyStateOpened)
  {
    if ( ! _collectHeaders())
    {
      _AHTTP_unlock;

      return;
    }
  }

  // If there's data in the buffer and not Done, advance readyState to Loading.
  if (_response->available() && _readyState != readyStateDone)
  {
    _setReadyState(readyStateLoading);
  }

  // If not chunked and all data read, close it up.
  if ( ! _chunked && (_response->available() + _contentRead) >= _contentLength)
  {
    char* connectionHdr = respHeaderValue("connection");

    if (connectionHdr && (strcasecmp_P(connectionHdr, PSTR("close")) == 0))
    {
      _client->close();
    }

    _requestEndTime = millis();
    _lastActivity = 0;
    _timeout = 0;
    _setReadyState(readyStateDone);
  }

  // If onData callback requested, do so.
  if (_onDataCB && available())
  {
    _onDataCB(_onDataCBarg, this, available());
  }

  _AHTTP_unlock;

}

////////////////////////////////////////

bool  AsyncHTTPRequest::_collectHeaders()
{
  // Loop to parse off each header line. Drop out and return false if no \r\n (incomplete)
  do
  {
    String headerLine = _response->readStringUntil("\r\n");

    // If no line, return false.
    if ( ! headerLine.length())
    {
      return false;
    }

    // If empty line, all headers are in, advance readyState.
    if (headerLine.length() == 2)
    {
      _setReadyState(readyStateHdrsRecvd);
    }
    // If line is HTTP header, capture HTTPcode.
    else if (headerLine.substring(0, 7) == "HTTP/1.")
    {
      _HTTPcode = headerLine.substring(9, headerLine.indexOf(' ', 9)).toInt();
    }
    // Ordinary header, add to header list.
    else
    {
      int colon = headerLine.indexOf(':');

      if (colon != -1)
      {
        String name = headerLine.substring(0, colon);
        name.trim();
        String value = headerLine.substring(colon + 1);
        value.trim();
        _addHeader(name.c_str(), value.c_str());
      }
    }
  } while (_readyState == readyStateOpened);

  // If content-Length header, set _contentLength
  header *hdr = _getHeader("Content-Length");

  if (hdr)
  {
    _contentLength = strtol(hdr->value, nullptr, 10);
  }

  // If chunked specified, try to set _contentLength to size of first chunk
  hdr = _getHeader("Transfer-Encoding");

  if (hdr && strcasecmp_P(hdr->value, PSTR("chunked")) == 0)
  {
    AHTTP_LOGDEBUG("*transfer-encoding: chunked");

    _chunked = true;
    _contentLength = 0;
    _chunks = new xbuf;
    _chunks->write(_response, _response->available());
    _processChunks();
  }

  return true;
}


/*_____________________________________________________________________________________________________________

                        H   H  EEEEE   AAA   DDDD   EEEEE  RRRR    SSS
                        H   H  E      A   A  D   D  E      R   R  S
                        HHHHH  EEE    AAAAA  D   D  EEE    RRRR    SSS
                        H   H  E      A   A  D   D  E      R  R       S
                        H   H  EEEEE  A   A  DDDD   EEEEE  R   R   SSS
  ______________________________________________________________________________________________________________*/

////////////////////////////////////////

void AsyncHTTPRequest::setReqHeader(const char* name, const char* value)
{
  if (_readyState <= readyStateOpened && _headers)
  {
    _addHeader(name, value);
  }
}

////////////////////////////////////////

void AsyncHTTPRequest::setReqHeader(const char* name, int32_t value)
{
  if (_readyState <= readyStateOpened && _headers)
  {
    setReqHeader(name, String(value).c_str());
  }
}

////////////////////////////////////////

int AsyncHTTPRequest::respHeaderCount()
{
  if (_readyState < readyStateHdrsRecvd)
    return 0;

  int count = 0;
  header* hdr = _headers;

  while (hdr)
  {
    count++;
    hdr = hdr->next;
  }

  return count;
}

////////////////////////////////////////

char* AsyncHTTPRequest::respHeaderName(int ndx)
{
  if (_readyState < readyStateHdrsRecvd)
    return nullptr;

  header* hdr = _getHeader(ndx);

  if ( ! hdr)
    return nullptr;

  return hdr->name;
}

////////////////////////////////////////

char* AsyncHTTPRequest::respHeaderValue(const char* name)
{
  if (_readyState < readyStateHdrsRecvd)
    return nullptr;

  header* hdr = _getHeader(name);

  if ( ! hdr)
    return nullptr;

  return hdr->value;
}

////////////////////////////////////////

char* AsyncHTTPRequest::respHeaderValue(int ndx)
{
  if (_readyState < readyStateHdrsRecvd)
    return nullptr;

  header* hdr = _getHeader(ndx);

  if ( ! hdr)
    return nullptr;

  return hdr->value;
}

////////////////////////////////////////

bool AsyncHTTPRequest::respHeaderExists(const char* name)
{
  if (_readyState < readyStateHdrsRecvd)
    return false;

  header* hdr = _getHeader(name);

  if ( ! hdr)
    return false;

  return true;
}

////////////////////////////////////////

String AsyncHTTPRequest::headers()
{
  MUTEX_LOCK(String())

  String _response = "";
  header* hdr = _headers;

  while (hdr)
  {
    _response += hdr->name;
    _response += ':';
    _response += hdr->value;
    _response += "\r\n";
    hdr = hdr->next;
  }

  _response += "\r\n";

  _AHTTP_unlock;

  return _response;
}

////////////////////////////////////////

AsyncHTTPRequest::header*  AsyncHTTPRequest::_addHeader(const char* name, const char* value)
{
  MUTEX_LOCK(nullptr)

  header* hdr = (header*) &_headers;

  while (hdr->next)
  {
    if (strcasecmp(name, hdr->next->name) == 0)
    {
      header* oldHdr  = hdr->next;
      hdr->next       = hdr->next->next;
      oldHdr->next    = nullptr;

      SAFE_DELETE(oldHdr)
    }
    else
    {
      hdr = hdr->next;
    }
  }

  hdr->next = new header;

  if (hdr->next)
  {
    hdr->next->name = new char[strlen(name) + 1];

    if (hdr->next->name)
      strcpy(hdr->next->name, name);
    else
    {
      SAFE_DELETE(hdr->next)

      return nullptr;
    }

    hdr->next->value = new char[strlen(value) + 1];

    if (hdr->next->value)
      strcpy(hdr->next->value, value);
    else
    {
      SAFE_DELETE_ARRAY(hdr->next->name)
      SAFE_DELETE(hdr->next)

      return nullptr;
    }
  }
  else
  {
    return nullptr;
  }

  _AHTTP_unlock;

  return hdr->next;
}

////////////////////////////////////////

AsyncHTTPRequest::header* AsyncHTTPRequest::_getHeader(const char* name)
{
  MUTEX_LOCK(nullptr)

  header* hdr = _headers;

  while (hdr)
  {
    if (strcasecmp(name, hdr->name) == 0)
      break;

    hdr = hdr->next;
  }

  _AHTTP_unlock;

  return hdr;
}

////////////////////////////////////////

AsyncHTTPRequest::header* AsyncHTTPRequest::_getHeader(int ndx)
{
  MUTEX_LOCK(nullptr)

  header* hdr = _headers;

  while (hdr)
  {
    if ( ! ndx--)
      break;

    hdr = hdr->next;
  }

  _AHTTP_unlock;

  return hdr;
}

////////////////////////////////////////

#endif    // ASYNC_HTTP_REQUEST_RP2040W_IMPL_H
