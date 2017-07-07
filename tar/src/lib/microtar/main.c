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
  char *data;
  char *name;
  size_t len;
} file_t;

static file_t **files;
static mtar_t tar;


int main(int argc, char **argv) {
  /* Allocate memory for all the files */
  files = calloc(argc - 2, sizeof(*files));
  /* Read files into array */
  FILE *f;
  for (size_t i = 2; i < argc; i++) {
    files[i]->name = argv[i];
    f = fopen(files[i]->name, "rb");
    files[i]->len = fread(files[i]->data );
    if (!f) {
      ERROR("could not read %s", argv[i]);
    } else {
      printf("read: %s\n", files[i]->name);
    }
  }

  /* Open archive for writing */
  mtar_open(&tar, argv[1], "w");

  /* Write file data to tar file */
  for (size_t i = 0; i < argc - 2; i++) {

    mtar_write_file_header(&tar, files[i]->name, strlen(str1));
    mtar_write_data(&tar, str1, strlen(str1));

    files[i]->name = argv[i];
    files[i]->data = fopen(files[i]->name, "rb");
    if (!files[i]->data) {
      ERROR("could not read %s", argv[i]);
    } else {
      printf("read: %s\n", files[i]->name);
    }
  }

  /* Write file data to tar file */
  // mtar_write_file_header(&tar, "test1.txt", strlen(str1));
  // mtar_write_data(&tar, str1, strlen(str1));
  // mtar_write_file_header(&tar, "test2.txt", strlen(str2));
  // mtar_write_data(&tar, str2, strlen(str2));

  /* Free file array */
  free(files);
  /* Finalize -- this needs to be the last thing done before closing */
  mtar_finalize(&tar);
  /* Close archive */
  mtar_close(&tar);
  return 0;
}
