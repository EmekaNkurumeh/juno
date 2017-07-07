/**
* Copyright (c) 2015 rxi
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the MIT license. See LICENSE for details.
*/

//  gcc *.c -o tarp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "microtar.h"
#include "../../util.h"

typedef struct {
  FILE *file;
  char *data;
  char *name;
} file_t;

static file_t **files;
static mtar_t tar;

static FILE *file;
static char *data;
static size_t len;

static void shutdown() {
  /* Free file array */
  // free(files);
  /* Finalize -- this needs to be the last thing done before closing */
  mtar_finalize(&tar);
  /* Close archive */
  mtar_close(&tar);
}

int main(int argc, char **argv) {
  atexit(shutdown);

  /* Allocate memory for all the files */
  // files = calloc(argc - 2, sizeof(file_t*));

  /* Open archive for writing */
  mtar_open(&tar, argv[1], "w");
  /* Read files into array */
  for (size_t i = 2; i < argc; i++) {
    puts("s");
    file = fopen(argv[i], "rb");
    if (!file) {
      ERROR("could not open %s", argv[i]);
    } else {
      /* Get file size */
      fseek(file, 0, SEEK_END);
      len = ftell(file);
      /* Load file */
      fseek(file, 0, SEEK_SET);
      if (fread(data, 1, len, file) != len) {
        ERROR("could not read %s", argv[i]);
      } else {
        printf("read: %s\n", argv[i]);
        fclose(file);
        /* Write file data to tar file */
        puts("test");
        size_t res = mtar_write_file_header(&tar, argv[i], len);
        puts("rest");
        if (res == MTAR_ESUCCESS) {
          res = mtar_write_data(&tar, data, len);
          if (res != MTAR_ESUCCESS)
            ERROR("%s", mtar_strerror(res));
        } else {
          ERROR("%s", mtar_strerror(res));
        }
        printf("wrote: %s \n", argv[i]);
      }
    }
  }

  /* Free file array */
  // free(files);
  /* Finalize -- this needs to be the last thing done before closing */
  mtar_finalize(&tar);
  /* Close archive */
  mtar_close(&tar);
  return 0;
}
