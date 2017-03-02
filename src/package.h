/**
 * Copyright (c) 2017 emekankurumeh
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef PACKAGE_H
#define PACKAGE_H

#if _WIN32
    #define type PACKAGE_TEXE
#elif __linux__
    #define type PACKAGE_TZIP
#elif __FreeBSD__
    #define type PACKAGE_TZIP
#elif __APPLE__
    #define type PACKAGE_TAPP
#else
    #define type PACKAGE_TZIP
#endif

enum {
  PACKAGE_TZIP,
  PACKAGE_TAPP,
  PACKAGE_TEXE
};

enum {
  PACKAGE_ESUCCESS =  0,
  PACKAGE_EFAILURE = -1,
};

int package_run(int argc, char **argv);

#endif
