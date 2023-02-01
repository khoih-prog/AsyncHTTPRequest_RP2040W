# AsyncHTTPRequest_RP2040W Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncHTTPRequest_RP2040W.svg?)](https://www.ardu-badge.com/AsyncHTTPRequest_RP2040W)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/AsyncHTTPRequest_RP2040W.svg)](https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/releases)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/AsyncHTTPRequest_RP2040W.svg)](http://github.com/khoih-prog/AsyncHTTPRequest_RP2040W/issues)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>


---
---

## Table of Contents

* [Changelog](#changelog)
  * [Release v1.3.0](#Release-v130)
  * [Release v1.2.2](#Release-v122)
  * [Release v1.2.1](#Release-v121)
  * [Release v1.2.0](#Release-v120)
  * [Release v1.1.1](#Release-v111)
  * [Release v1.1.0](#Release-v110)
  * [Release v1.0.1](#Release-v101)
  * [Initial Release v1.0.0](#Initial-Release-v100)

---
---

## Changelog

### Release v1.3.0

1. Fix bug of `_parseURL()`. Check [Bug with _parseURL() #21](https://github.com/khoih-prog/AsyncHTTPSRequest_Generic/issues/21)
2. Improve `README.md` so that links can be used in other sites, such as `PIO`

### Release v1.2.2

1. Default to reconnect to the same `host:port` after connected for new HTTP sites. Check [Host/Headers not always sent with 1.10.1 #44](https://github.com/khoih-prog/AsyncHTTPRequest_Generic/issues/44)
2. Fix bug
3. Use `allman astyle` and add `utils`

### Release v1.2.1

1. Fix bug of wrong `reqStates`. Check [Release 1.9 breaks previously running code #39](https://github.com/khoih-prog/AsyncHTTPRequest_Generic/issues/39) and [Callback behaviour is buggy (ESP8266) #43](https://github.com/khoih-prog/AsyncHTTPRequest_Generic/issues/43)
2. Optional larger `DEFAULT_RX_TIMEOUT` from default 3s, for slower networks

### Release v1.2.0

1. Fix bug. 
2. Clean up

### Release v1.1.1

1. Not try to reconnect to the same host:port after connected. Check [setReuse feature #12](https://github.com/khoih-prog/AsyncHTTPSRequest_Generic/issues/12)

### Release v1.1.0

1. Fix long timeout if using `IPAddress`.
2. Optimize code
3. Display only successful responseText in examples
4. Improve debug messages by adding functions to display error messages instead of `cryptic error number`


### Release v1.0.1

1. Fix bug in examples

### Initial Release v1.0.0

1. Initial coding to support **RASPBERRY_PI_PICO_W with CYW43439 WiFi**, using [**arduino-pico core v2.4.0+**](https://github.com/earlephilhower/arduino-pico)
