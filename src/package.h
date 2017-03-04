/**
 * Copyright (c) 2017 emekankurumeh
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef PACKAGE_H
#define PACKAGE_H

enum {
  PACKAGE_ESUCCESS =  0,
  PACKAGE_EFAILURE = -1,
};

int package_run(int argc, char **argv);

#endif
