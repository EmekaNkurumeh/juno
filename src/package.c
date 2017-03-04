/**
 * Copyright (c) 2017 emekankurumeh
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

/* Force miniz.c to use regular fseek/ftell functions */
#define fseeko fseek
#define ftello ftell
#define fseeko64 fseek
#define ftello64 ftell
#define fopen64 fopen
#define freopen64 freopen

#define MINIZ_HEADER_FILE_ONLY
#include "lib/miniz.c"
#undef MINIZ_HEADER_FILE_ONLY

#if _WIN32
  #define mkdir(path, mode) mkdir(path)
#endif

#include "package.h"
#include "fs.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

static void error(const char *fmt, ...) {
  va_list argp;
  printf("package error: ");
  va_start(argp, fmt);
  vprintf(fmt, argp);
  va_end(argp);
  printf("\n");
  exit(EXIT_FAILURE);
}

static char *concat(const char *str, ...) {
  va_list args;
  const char *s;
  /* Get len */
  int len = strlen(str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    len += strlen(s);
  }
  va_end(args);
  /* Build string */
  char *res = malloc(len + 1);
  if (!res) return NULL;
  strcpy(res, str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    strcat(res, s);
  }
  va_end(args);
  return res;
}

static int isDir(const char *path) {
  struct stat s;
  int res = stat(path, &s);
  return (res == FS_ESUCCESS) && S_ISDIR(s.st_mode);
}

static int isSeparator(int chr) {
  return (chr == '/' || chr == '\\');
}


static int makeDirs(const char *path) {
  int err = FS_ESUCCESS;
  char *str = concat(path, "/", NULL);
  char *p = str;
  if (!str) {
    err = FS_EOUTOFMEM;
    goto end;
  }
  if (p[0] == '/') p++;
  if (p[0] && p[1] == ':' && p[2] == '\\') p += 3;
  while (*p) {
    if (isSeparator(*p)) {
      *p = '\0';
      if (!isDir(str)) {
        if (mkdir(str, S_IRWXU) == -1) {
          err = FS_ECANTMKDIR;
          goto end;
        }
      }
      *p = '/';
    }
    p++;
  }
end:
  free(str);
  return err;
}

char * concat_path(const char *dir, const char *filename) {
  int dirlen = strlen(dir);
  if ( dir[dirlen - 1] == '/' || *dir == '\0' ) {
    return concat(dir, filename, NULL);
  } else {
    return concat(dir, "/", filename, NULL);
  }
}

static void write_data(char *data, const char *outname) {
  FILE *wp = fopen(outname, "wb");

  if (!wp) {
    error("couldn't open output file '%s'", outname);
  }

  /* Write file data */
  fwrite(data, sizeof(char), strlen(data) + 1, wp);

  /* Close file, free data buffer, and return ok */
  fclose(wp);
}

static void write_file(const char *inname, const char *outname) {
  FILE *rp = fopen(inname, "rb");
  char *data;

  if (!rp) {
    error("couldn't open input file '%s'", inname);
  }

  /* Get size */
  fseek(rp, 0, SEEK_END);
  int size = ftell(rp);
  fseek(rp, 0, SEEK_SET);

  /* Get file data */
  data = (char *)malloc(sizeof(char *) * size + 1);
  int sz = fread(data, sizeof(char), size, rp);
  data[sz] = '\0';

  /* Write file data */
  write_data(data, outname);

  /* Close file, free data buffer, and return ok */
  fclose(rp);
  free(data);
}

static void write_file_zip(const char *zip, const char *inname, const char *outname) {
  FILE *fp = fopen(inname, "rb");
  char *data;

  if (!fp) {
    error("couldn't open input file '%s'", inname);
  }

  /* Get size */
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  /* Get file data */
  data = (char *)malloc(sizeof(char *) * size + 1);
  int sz = fread(data, sizeof(char), size, fp);

  /* Write the file */
  data[sz] = '\0';
  mz_zip_add_mem_to_archive_file_in_place(zip, outname, data, sz, "no comment", (uint16)strlen("no comment"), MZ_BEST_COMPRESSION);

  /* Close file, free data buffer, and return ok */
  fclose(fp);
  free(data);
}

static void write_dir(const char *zip, const char *indir, const char *outdir) {
  char *inbuf = "";
  char *outbuf = "";

  /* Mount file directory */
  fs_mount(indir);

  /* Write files */
  fs_FileListNode *list = fs_listDir(".");
  int i = 1;
  fs_FileListNode *n = list;

  while (n) {
    /* Get full input name and full output name */
    inbuf = concat_path(indir, n->name);
    outbuf = concat_path(outdir, n->name);

    /* Loop through directories write files and create folders */
    if (fs_isDir(n->name)) {
      fs_unmount(indir);
      write_dir(zip, inbuf, outbuf);
    } else {
      write_file_zip(zip, inbuf, outbuf);
    }
    i++;
    n = n->next;
  }
  fs_freeFileList(list);
  fs_unmount(indir);
  free(inbuf);
  free(outbuf);
}

int package_run(int argc, char **argv) {
  /* Check for `--pack` argument; return failure if it isn't present */
  if (argc < 2) {
    return PACKAGE_EFAILURE;
  }
  if ( strcmp(argv[1], "--pack") != 0) {
    return PACKAGE_EFAILURE;
  }

  /* Check arguments */
  if (argc < 4) {
    error("expected arguments: %s dirname outfile", argv[1]);
  }

  if (!isDir(argv[2])) {
    exit(PACKAGE_EFAILURE);
  }


  /* Generate write directories */
  char *exe_dir = "";
  char *zip_dir = "";
  char plist[1000];

  switch (type) {
    case PACKAGE_TEXE:
    case PACKAGE_TZIP:
      exe_dir = argv[3];
      zip_dir = concat_path(argv[3], "pak0");

    break;
    case PACKAGE_TAPP:
      exe_dir = concat(argv[3], ".app/Contents/MacOS", NULL);
      zip_dir = concat_path(concat(argv[3], ".app/Contents/Resources", NULL), "pak0");
      sprintf(plist, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
      "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n" \
      "<plist version=\"1.0\"> \n" \
      "<dict>\n" \
      "	<key>CFBundleDisplayName</key> \n" \
      "	<string>%s</string>\n" \
      "	<key>CFBundleExecutable</key>\n" \
      "	<string>juno</string>\n" \
      "	<key>CFBundleIdentifier</key>\n" \
      "	<string></string>\n" \
      "	<key>CFBundleName</key>\n" \
      "	<string>%s</string>\n" \
      "	<key>CFBundlePackageType</key> \n" \
      "	<string>APPL</string>\n" \
      "	<key>CFBundleShortVersionString</key>\n" \
      "	<string>0.0</string> \n" \
      "	<key>CFBundleSignature</key> \n" \
      "	<string>?\??\?</string>\n" \
      "	<key>CFBundleVersion</key> \n" \
      "	<string>0.0</string> \n" \
      "</dict> \n" \
      "</plist>\n", argv[3], argv[3]);
    break;
  }

  /* Remove old files */
  remove(concat(argv[3], ".app", NULL));
  makeDirs(concat(argv[3], ".app", NULL));
  makeDirs(concat(argv[3], ".app/Contents", NULL));
  makeDirs(concat(argv[3], ".app/Contents/Frameworks", NULL));
  makeDirs(concat(argv[3], ".app/Contents/MacOS", NULL));
  makeDirs(concat(argv[3], ".app/Contents/Resources", NULL));
  printf("%d\n", isDir(concat(argv[3], ".app", NULL)));

  /* Make package and return success*/
  write_file(argv[0], concat_path(exe_dir, basename(argv[0])));
  chmod(concat_path(exe_dir, basename(argv[0])), strtol("777", 0, 8));
  write_dir(zip_dir, argv[2], "");

  if (type == PACKAGE_TAPP) {
    write_data(plist, concat(argv[3], ".app/Contents/Info.plist", NULL));
  }

  return PACKAGE_ESUCCESS;
}
