/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Judy.h>
#include <sds.h>

void * alloc(size_t sz);
void * ralloc(void *p, size_t sz);
#define newa(ty, n) ((ty*)alloc(sizeof(ty) * (n)))
#define new(ty) newa(ty, 1)

void get_bus_params(const char *module, sds *pservice, sds *pobject);

void __setproctitle_init(char **argv);
#define setproctitle_init(argc, argv, ...) __setproctitle_init(argv)
void setproctitle(const char *fmt, ...);

int readline(FILE *fp, sds *pline);

typedef struct {
  Pvoid_t p;
  size_t sz;
} table;

void table_init(table *tbl);
void table_add(table *tbl, const char *key, void *value);
void * table_get(table *tbl, const char *key);
void * table_swap(table *tbl, const char *key, void *value);
char * table_next(table *tbl, char *prev, void **value);
void table_free(table *tbl);

#endif
