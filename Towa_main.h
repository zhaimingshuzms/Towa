#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

#ifndef TOWA_MAIN_H_
#define TOWA_MAIN_H_

//gcc -Wall Towa_main.c `pkg-config fuse3 --cflags --libs` -o Towa_main

void * Towa_init (struct fuse_conn_info *conn,struct fuse_config *cfg);

int Towa_getattr (const char* path, struct stat* st_buf,struct fuse_file_info *fi);

int Towa_readdir (const char *path, void *buffer, fuse_fill_dir_t filler, off_t oTowaet, struct fuse_file_info *fi, enum fuse_readdir_flags fl);

int Towa_mkdir (const char *path, mode_t mode);

int Towa_rmdir (const char *path);

int Towa_mknod (const char* path, mode_t mode, dev_t rdev);

int Towa_unlink (const char *path);

int Towa_read (const char* path, char* buffer, size_t size, off_t oTowaet, struct fuse_file_info* fi);

int Towa_write (const char* path, const char* buffer, size_t size, off_t oTowaet, struct fuse_file_info* fi);

int Towa_open (const char *path, struct fuse_file_info *fi);

#endif