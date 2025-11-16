# TcMenuLog
[![PlatformIO](https://github.com/TcMenu/TcMenuLog/actions/workflows/platformio.yml/badge.svg)](https://github.com/TcMenu/TcMenuLog/actions/workflows/platformio.yml)
[![Test](https://github.com/TcMenu/TcMenuLog/actions/workflows/test.yml/badge.svg)](https://github.com/TcMenu/TcMenuLog/actions/workflows/test.yml)
[![License: Apache 2.0](https://img.shields.io/badge/license-Apache--2.0-green.svg)](https://github.com/TcMenu/TcMenuLog/blob/main/LICENSE)
[![GitHub release](https://img.shields.io/github/release/TcMenu/TcMenuLog.svg?maxAge=3600)](https://github.com/TcMenu/TcMenuLog/releases)
[![davetcc](https://img.shields.io/badge/davetcc-dev-blue.svg)](https://github.com/davetcc)
[![JSC TechMinds](https://img.shields.io/badge/JSC-TechMinds-green.svg)](https://www.jsctm.cz)

TcMenu organisation made this library available for you to use. It takes significant effort to keep all our libraries current and working on a wide range of boards. Please consider making at least a one off donation via the sponsor button if you find it useful. In forks, please keep text to here intact.

## Summary

This library provides logging facilities that are used by all our other libraries. You can consider this like a logging API with a simple implementation that backs onto the serial port by default.

The library works on most Arduino devices, PicoSDK and mbed. You can see our library compatibility matrix: https://tcmenu.github.io/documentation/

You can find examples packaged with it in the `examples` folder. The example should work on most platforms without needing changes. 

## Supporting another output format

The easiest way would be to create another implementation of `LoggingPort` that met the `Print` (or `PrintCompat`) interface, this will then integrate without changing anything. You can see examples of this in `IoLogging.h` for mbed and PicoSDK. 

## License

Licensed under the [Apache-2.0](LICENSE) license.
