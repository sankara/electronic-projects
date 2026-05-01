# Issues faced and how it was resolved

1. When compiling with PlatformIO it fails with missing SNU.h. Looks like
the case of a missing library from the core platform package for this board. 
Filed an [issue](https://github.com/platformio/platform-atmelsam/issues/123).
Resolved by copying the library to local libs.
2. Similar to (1) there's an issue with a dependency on EEPROM by ezTime. Resolved it by cloning the library and 
disabling the #define. No caching here.