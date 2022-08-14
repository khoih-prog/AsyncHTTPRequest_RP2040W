# AsyncHTTPRequest_RP2040W

[![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncHTTPRequest_RP2040W.svg?)](https://www.ardu-badge.com/AsyncHTTPRequest_RP2040W)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/AsyncHTTPRequest_RP2040W.svg)](https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/releases)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/AsyncHTTPRequest_RP2040W.svg)](http://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/issues)

---
---

## Table of Contents

* [Why do we need the new Async AsyncHTTPRequest_RP2040W library](#why-do-we-need-this-async-AsyncHTTPRequest_RP2040W-library)
  * [Features](#features)
  * [Supports](#supports)
  * [Principles of operation](#principles-of-operation)
  * [Currently supported Boards](#currently-supported-boards)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Packages' Patches](#packages-patches)
  * [1. For Portenta_H7 boards using Arduino IDE in Linux](#1-for-portenta_h7-boards-using-arduino-ide-in-linux)
  * [2. To fix compile error relating to dns_gethostbyname and LwIP stack](#2-to-fix-compile-error-relating-to-dns_gethostbyname-and-lwip-stack)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error) 
* [Examples](#examples)
  * [1. AsyncHTTPRequest](examples/AsyncHTTPRequest)
  * [2. AsyncCustomHeader](examples/AsyncCustomHeader)
  * [3. AsyncDweetGet](examples/AsyncDweetGet)
  * [4. AsyncDweetPost](examples/AsyncDweetPost)
  * [5. AsyncSimpleGET](examples/AsyncSimpleGET)
  * [6. AsyncWebClientRepeating](examples/AsyncWebClientRepeating)
  * [7. **multiFileProject**](examples/multiFileProject)
* [Example AsyncHTTPRequest](#example-asynchttprequest)
  * [1. File AsyncHTTPRequest.ino](#1-file-asynchttprequestino)
  * [2. File defines.h](#2-file-definesh) 
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. AsyncHTTPRequest running on RASPBERRY_PI_PICO_W using CYW43439 WiFi](#1-asynchttprequest-running-on-RASPBERRY_PI_PICO_W-using-CYW43439-WiFi)
  * [2. AsyncDweetPOST running on RASPBERRY_PI_PICO_W using CYW43439 WiFi](#2-AsyncDweetPOST-running-on-RASPBERRY_PI_PICO_W-using-CYW43439-WiFi)
  * [3. AsyncWebClientRepeating running on RASPBERRY_PI_PICO_W using CYW43439 WiFi](#3-AsyncWebClientRepeating-running-on-RASPBERRY_PI_PICO_W-using-CYW43439-WiFi)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License and credits](#license-and-credits)
* [Copyright](#copyright)

---
---


## Why do we need this Async [AsyncHTTPRequest_RP2040W library](https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W)

### Features

1. Asynchronous HTTP Request library for **RASPBERRY_PI_PICO_W using CYW43439 WiFi**
2. Providing a subset of HTTP.
3. Relying on **[`Khoi Hoang's AsyncTCP_RP2040W`](https://github.com/khoih-prog/AsyncTCP_RP2040W)**
4. Methods similar in format and usage to XmlHTTPrequest in Javascript.

### Supports

1. **GET, POST, PUT, PATCH, DELETE and HEAD**
2. Request and response headers
3. Chunked response
4. Single String response for short (<~5K) responses (heap permitting).
5. Optional onData callback.
6. Optional onReadyStatechange callback.

### Principles of operation

This library adds a simple HTTP layer on top of the [`AsyncTCP_RP2040W`](https://github.com/khoih-prog/AsyncTCP_RP2040W) library to **facilitate REST communication from a Client to a Server**. The paradigm is similar to the XMLHttpRequest in Javascript, employing the notion of a ready-state progression through the transaction request.

**Synchronization can be accomplished using callbacks on ready-state change, a callback on data receipt, or simply polling for ready-state change**. Data retrieval can be incremental as received, or bulk retrieved when the transaction completes provided there is enough heap to buffer the entire response.

The underlying buffering uses a new xbuf class. It handles both character and binary data. Class xbuf uses a chain of small (64 byte) segments that are allocated and added to the tail as data is added and deallocated from the head as data is read, achieving the same result as a dynamic circular buffer limited only by the size of heap. The xbuf implements indexOf and readUntil functions.

For short transactions, buffer space should not be an issue. In fact, it can be more economical than other methods that use larger fixed length buffers. Data is acked when retrieved by the caller, so there is some limited flow control to limit heap usage for larger transfers.

Request and response headers are handled in the typical fashion.

Chunked responses are recognized and handled transparently.

This library is based on, modified from:

1. [Bob Lemaire's asyncHTTPrequest Library](https://github.com/boblemaire/asyncHTTPrequest)

---

### Currently Supported Boards

1. **RASPBERRY_PI_PICO_W with CYW43439 WiFi** using [**arduino-pico core v2.4.0+**](https://github.com/earlephilhower/arduino-pico)

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/blob/main/pics/RP2040W-pinout.svg">
</p>


---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [`Earle Philhower's arduino-pico core v2.4.0+`](https://github.com/earlephilhower/arduino-pico) for **RASPBERRY_PI_PICO_W with CYW43439 WiFi**, etc. [![GitHub release](https://img.shields.io/github/release/earlephilhower/arduino-pico.svg)](https://github.com/earlephilhower/arduino-pico/releases/latest)
 3. [`AsyncTCP_RP2040W library v1.0.0+`](https://github.com/khoih-prog/AsyncTCP_RP2040W) for RASPBERRY_PI_PICO_W with CYW43439 WiFi. To install. check [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncTCP_RP2040W.svg?)](https://www.ardu-badge.com/AsyncTCP_RP2040W)

---
---

## Installation

### Use Arduino Library Manager
The best and easiest way is to use `Arduino Library Manager`. Search for `AsyncHTTPRequest_RP2040W`, then select / install the latest version. You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncHTTPRequest_RP2040W.svg?)](https://www.ardu-badge.com/AsyncHTTPRequest_RP2040W) for more detailed instructions.

### Manual Install

1. Navigate to [AsyncHTTPRequest_RP2040W](https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W) page.
2. Download the latest release `AsyncHTTPRequest_RP2040W-main.zip`.
3. Extract the zip file to `AsyncHTTPRequest_RP2040W-main` directory 
4. Copy the whole `AsyncHTTPRequest_RP2040W-main` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**AsyncHTTPRequest_RP2040W** library](https://registry.platformio.org/libraries/khoih-prog/AsyncHTTPRequest_RP2040W) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/AsyncHTTPRequest_RP2040W/installation). Search for **AsyncHTTPRequest_RP2040W** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---


### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "AsyncHTTPRequest_RP2040W.hpp"     //https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "AsyncHTTPRequest_RP2040W.h"           //https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)


---
---

### Examples

 1. [AsyncHTTPRequest](examples/AsyncHTTPRequest)
 2. [AsyncCustomHeader](examples/AsyncCustomHeader)
 3. [AsyncDweetGet](examples/AsyncDweetGet)
 4. [AsyncDweetPost](examples/AsyncDweetPost)
 5. [AsyncSimpleGET](examples/AsyncSimpleGET)
 6. [AsyncWebClientRepeating](examples/AsyncWebClientRepeating)
 7. [multiFileProject](examples/multiFileProject)
    
    
---

### Example [AsyncHTTPRequest](examples/AsyncHTTPRequest)

Please take a look at other examples, as well.

#### 1. File [AsyncHTTPRequest.ino](examples/AsyncHTTPRequest/AsyncHTTPRequest.ino)

```cpp
#include "defines.h"

#define ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN_TARGET      "AsyncHTTPRequest_RP2040W v1.0.0"
#define ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN             1000000

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <AsyncHTTPRequest_RP2040W.h>         // https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W

AsyncHTTPRequest request;

int status = WL_IDLE_STATUS;

void sendRequest()
{
  static bool requestOpenResult;
  
  if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone)
  {
    //requestOpenResult = request.open("GET", "http://worldtimeapi.org/api/timezone/Europe/London.txt");
    requestOpenResult = request.open("GET", "http://worldtimeapi.org/api/timezone/America/Toronto.txt");
    //requestOpenResult = request.open("GET", "http://213.188.196.246/api/timezone/America/Toronto.txt");
    
    if (requestOpenResult)
    {
      Serial.println("Request sent");
      
      // Only send() if open() returns true, or crash
      request.send();
    }
    else
    {
      Serial.println("Can't send bad request");
    }
  }
  else
  {
    Serial.println("Can't send request");
  }
}

void requestCB(void* optParm, AsyncHTTPRequest* request, int readyState) 
{
  (void) optParm;
  
  if (readyState == readyStateDone) 
  {
    Serial.println("\n**************************************");
    Serial.println(request->responseText());
    Serial.println("**************************************");
    
    request->setDebug(false);
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.print("\nStart AsyncHTTPRequest on "); Serial.println(BOARD_NAME);
  Serial.println(ASYNCTCP_RP2040W_VERSION);
  Serial.println(ASYNC_HTTP_REQUEST_RP2040W_VERSION);

#if defined(ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN)
  if (ASYNC_HTTP_REQUEST_RP2040W_VERSION_INT < ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN_TARGET);
  }
#endif

  ///////////////////////////////////
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  Serial.print(F("Connecting to SSID: "));
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);

  delay(1000);
   
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED)
  {
    delay(500);
        
    // Connect to WPA/WPA2 network
    status = WiFi.status();
  }

  printWifiStatus();

  ///////////////////////////////////

  request.setDebug(false);
  
  request.onReadyStateChange(requestCB);
}

void sendRequestRepeat()
{
  static unsigned long sendRequest_timeout = 0;

#define SEND_REQUEST_INTERVAL     60000L

  // sendRequest every SEND_REQUEST_INTERVAL (60) seconds: we don't need to sendRequest frequently
  if ((millis() > sendRequest_timeout) || (sendRequest_timeout == 0))
  {
    sendRequest();
    
    sendRequest_timeout = millis() + SEND_REQUEST_INTERVAL;
  }
}

void loop()
{
  sendRequestRepeat();
}
```

---

#### 2. File [defines.h](examples/AsyncHTTPRequest/defines.h)


```cpp
#ifndef defines_h
#define defines_h

#if !( defined(ARDUINO_RASPBERRY_PI_PICO_W) )
  #error For RASPBERRY_PI_PICO_W only
#endif

char ssid[] = "your_ssid";        // your network SSID (name)
char pass[] = "12345678";         // your network password (use for WPA, or use as key for WEP), length must be 8+

#endif    //defines_h
```

---
---

### Debug Terminal Ouput Samples

#### 1. [AsyncHTTPRequest](examples/AsyncHTTPRequest) running on RASPBERRY_PI_PICO_W using CYW43439 WiFi

```
Start AsyncHTTPRequest on RASPBERRY_PI_PICO_W with RP2040W CYW43439 WiFi
AsyncTCP_RP2040W v1.0.0
AsyncHTTPRequest_RP2040W v1.0.0
Using mac index = 15
Connected! IP address: 192.168.2.101
Request sent
**************************************
abbreviation: EST
client_ip: aaa.bbb.ccc.ddd
datetime: 2022-01-23T19:06:29.846071-05:00
day_of_week: 0
day_of_year: 23
dst: false
dst_from: 
dst_offset: 0
dst_until: 
raw_offset: -18000
timezone: America/Toronto
unixtime: 1642982789
utc_datetime: 2022-01-24T00:06:29.846071+00:00
utc_offset: -05:00
week_number: 3
**************************************
Request sent

**************************************
abbreviation: EST
client_ip: aaa.bbb.ccc.ddd
datetime: 2022-01-23T19:08:29.871390-05:00
day_of_week: 0
day_of_year: 23
dst: false
dst_from: 
dst_offset: 0
dst_until: 
raw_offset: -18000
timezone: America/Toronto
unixtime: 1642982909
utc_datetime: 2022-01-24T00:08:29.871390+00:00
utc_offset: -05:00
week_number: 3
**************************************
```

---

#### 2. [AsyncDweetPOST](examples/AsyncDweetPOST) running on RASPBERRY_PI_PICO_W using CYW43439 WiFi

```
Start AsyncDweetPOST on RASPBERRY_PI_PICO_W with RP2040W CYW43439 WiFi
AsyncTCP_RP2040W v1.0.0
AsyncHTTPRequest_RP2040W v1.0.0
Connecting to SSID: HueNet1
SSID: HueNet1
Local IP Address: 192.168.2.94
signal strength (RSSI):-25 dBm

Making new POST request

**************************************
{"this":"succeeded","by":"dweeting","the":"dweet","with":{"thing":"pinA0-Read","created":"2022-08-14T00:17:43.768Z","content":{"sensorValue":88},"transaction":"34ca3083-054b-42ef-bcb0-91f7150dc680"}}
**************************************
"sensorValue":88
Value string: 88
Actual value: 88
```

---

#### 3. [AsyncWebClientRepeating](examples/AsyncWebClientRepeating) running on RASPBERRY_PI_PICO_W using CYW43439 WiFi

```
Start AsyncWebClientRepeating on RASPBERRY_PI_PICO_W with RP2040W CYW43439 WiFi
AsyncTCP_RP2040W v1.0.0
AsyncHTTPRequest_RP2040W v1.0.0
Connecting to SSID: HueNet1
SSID: HueNet1
Local IP Address: 192.168.2.94
signal strength (RSSI):-26 dBm

**************************************

           `:;;;,`                      .:;;:.           
        .;;;;;;;;;;;`                :;;;;;;;;;;:     TM 
      `;;;;;;;;;;;;;;;`            :;;;;;;;;;;;;;;;      
     :;;;;;;;;;;;;;;;;;;         `;;;;;;;;;;;;;;;;;;     
    ;;;;;;;;;;;;;;;;;;;;;       .;;;;;;;;;;;;;;;;;;;;    
   ;;;;;;;;:`   `;;;;;;;;;     ,;;;;;;;;.`   .;;;;;;;;   
  .;;;;;;,         :;;;;;;;   .;;;;;;;          ;;;;;;;  
  ;;;;;;             ;;;;;;;  ;;;;;;,            ;;;;;;. 
 ,;;;;;               ;;;;;;.;;;;;;`              ;;;;;; 
 ;;;;;.                ;;;;;;;;;;;`      ```       ;;;;;`
 ;;;;;                  ;;;;;;;;;,       ;;;       .;;;;;
`;;;;:                  `;;;;;;;;        ;;;        ;;;;;
,;;;;`    `,,,,,,,,      ;;;;;;;      .,,;;;,,,     ;;;;;
:;;;;`    .;;;;;;;;       ;;;;;,      :;;;;;;;;     ;;;;;
:;;;;`    .;;;;;;;;      `;;;;;;      :;;;;;;;;     ;;;;;
.;;;;.                   ;;;;;;;.        ;;;        ;;;;;
 ;;;;;                  ;;;;;;;;;        ;;;        ;;;;;
 ;;;;;                 .;;;;;;;;;;       ;;;       ;;;;;,
 ;;;;;;               `;;;;;;;;;;;;                ;;;;; 
 `;;;;;,             .;;;;;; ;;;;;;;              ;;;;;; 
  ;;;;;;:           :;;;;;;.  ;;;;;;;            ;;;;;;  
   ;;;;;;;`       .;;;;;;;,    ;;;;;;;;        ;;;;;;;:  
    ;;;;;;;;;:,:;;;;;;;;;:      ;;;;;;;;;;:,;;;;;;;;;;   
    `;;;;;;;;;;;;;;;;;;;.        ;;;;;;;;;;;;;;;;;;;;    
      ;;;;;;;;;;;;;;;;;           :;;;;;;;;;;;;;;;;:     
       ,;;;;;;;;;;;;;,              ;;;;;;;;;;;;;;       
         .;;;;;;;;;`                  ,;;;;;;;;:         
                                                         
                                                         
                                                         
                                                         
    ;;;   ;;;;;`  ;;;;:  .;;  ;; ,;;;;;, ;;. `;,  ;;;;   
    ;;;   ;;:;;;  ;;;;;; .;;  ;; ,;;;;;: ;;; `;, ;;;:;;  
   ,;:;   ;;  ;;  ;;  ;; .;;  ;;   ,;,   ;;;,`;, ;;  ;;  
   ;; ;:  ;;  ;;  ;;  ;; .;;  ;;   ,;,   ;;;;`;, ;;  ;;. 
   ;: ;;  ;;;;;:  ;;  ;; .;;  ;;   ,;,   ;;`;;;, ;;  ;;` 
  ,;;;;;  ;;`;;   ;;  ;; .;;  ;;   ,;,   ;; ;;;, ;;  ;;  
  ;;  ,;, ;; .;;  ;;;;;:  ;;;;;: ,;;;;;: ;;  ;;, ;;;;;;  
  ;;   ;; ;;  ;;` ;;;;.   `;;;:  ,;;;;;, ;;  ;;,  ;;;;   

**************************************
```


---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level from 0 to 4

```cpp
#define ASYNC_HTTP_RP2040W_DEBUG_PORT           Serial

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNCTCP_RP2040W_LOGLEVEL_             1
#define _ASYNC_HTTP_LOGLEVEL_                   1
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the [**arduino-pico core**](https://github.com/earlephilhower/arduino-pico)

Sometimes, the library will only work if you update the [**arduino-pico core**](https://github.com/earlephilhower/arduino-pico) core to the latest version because I am using newly added functions.

---

### Issues ###

Submit issues to: [AsyncHTTPRequest_RP2040W issues](https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/issues)

---

## TO DO

 1. Fix bug. Add enhancement
 2. Add many more examples.

---

## DONE

 1. Add support to RASPBERRY_PI_PICO_W using CYW43439 WiFi
 2. Add debugging features.
 3. Add PUT, PATCH, DELETE and HEAD besides GET and POST.
 4. Fix `multiple-definitions` linker error and weird bug related to `src_cpp`.
 5. Optimize library code by using `reference-passing` instead of `value-passing`

---
---

### Contributions and Thanks

This library is based on, modified, bug-fixed and improved from:

1. [Bob Lemaire's **asyncHTTPrequest Library**](https://github.com/boblemaire/asyncHTTPrequest) to use the better **asynchronous** features of [`AsyncTCP_RP2040W`](https://github.com/khoih-prog/AsyncTCP_RP2040W).


<table>
  <tr>
    <td align="center"><a href="https://github.com/boblemaire"><img src="https://github.com/boblemaire.png" width="100px;" alt="boblemaire"/><br /><sub><b>⭐️ Bob Lemaire</b></sub></a><br /></td>
  </tr> 
</table>

---

### Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License and credits ###

- The library is licensed under [GPLv3](https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/blob/main/LICENSE)

---

## Copyright

Copyright (C) <2018>  <Bob Lemaire, IoTaWatt, Inc.>

Copyright 2022- Khoi Hoang



