/****************************************************************************************************************************
  multiFileProject.ino

  For RP2040W with CYW43439 WiFi

  AsyncHTTPRequest_RP2040W is a library for the RP2040W with CYW43439 WiFi

  Based on and modified from asyncHTTPrequest Library (https://github.com/boblemaire/asyncHTTPrequest)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#if !( defined(ARDUINO_RASPBERRY_PI_PICO_W) )
  #error For RASPBERRY_PI_PICO_W only
#endif

#define ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN_TARGET      "AsyncHTTPRequest_RP2040W v1.2.2"
#define ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN             1002002

// Uncomment for certain HTTP site to optimize
//#define NOT_SEND_HEADER_AFTER_CONNECTED        true

// Level from 0-4
#define ASYNC_HTTP_DEBUG_PORT     Serial
#define _ASYNC_HTTP_LOGLEVEL_     2

#include "multiFileProject.h"

// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include <AsyncHTTPRequest_RP2040W.h>         // https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  Serial.println("\nStart multiFileProject");
  Serial.println(ASYNC_HTTP_REQUEST_RP2040W_VERSION);

#if defined(ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN)

  if (ASYNC_HTTP_REQUEST_RP2040W_VERSION_INT < ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN_TARGET);
  }

#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
}
