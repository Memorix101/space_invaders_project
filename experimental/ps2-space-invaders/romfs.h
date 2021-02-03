/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.
#
# ROMFS.
*/



#ifndef __ROMFS_H__
#define __ROMFS_H__


#define _NFILE 20

unsigned char romdisk_start[];

int romdisk_mount(const void *img);
int romdisk_umount();
int romdisk_find(const char *path, void **ptr, int *size);



#endif
