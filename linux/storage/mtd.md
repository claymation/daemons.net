# MTD

The Memory Technology Devices (MTD) layer provides a common interface to many
types of [raw flash](/hardware/storage/flash.html) devices. In addition to the
typical block read and write operations, it also provides an erase operation
and facilities for managing bad blocks and out-of-band data (for NAND flash).


## User-space interfaces

  * `/dev/mtdX`: read-write character device for I/O and ioctls
  * `/dev/mtdXro`: read-only character device for I/O and ioctls
  * `/sys/class/mtd`
  * `/proc/mtd` (deprecated)

You can read any number of bytes from a `/dev/mtdX` character device node; MTD
transparently reads the required number of blocks from the device and copies
the requested number of bytes to the userspace buffer. Writes, however, must
be done in multiples of the page size.

Here's an example from a (simulated, see below) NAND flash with a 512-byte
page size:

``` txt
clay@ubuntu:~$ sudo dd if=/dev/mtd0 of=/dev/null bs=1 count=1
1+0 records in
1+0 records out
1 byte copied, 0.000174195 s, 5.7 kB/s

clay@ubuntu:~$ sudo dd if=/dev/zero of=/dev/mtd0 bs=1 count=1
dd: error writing '/dev/mtd0': Invalid argument
1+0 records in
0+0 records out
0 bytes copied, 0.000312924 s, 0.0 kB/s
```

As you would expect, reading an erased flash gives bytes with all bits set
(0xff):

``` txt
clay@ubuntu:~$ sudo dd if=/dev/mtd0 bs=512 count=1 2>/dev/null | hd
00000000  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
*
00000200
```

Writing the flash can reset the bits to zero, but can not set them back to
one:

``` txt
# write (program) all the bits to zero
clay@ubuntu:~$ sudo dd if=/dev/zero of=/dev/mtd0 bs=512 count=1
1+0 records in
1+0 records out
512 bytes copied, 0.0021276 s, 241 kB/s

# all the bits are now zero
clay@ubuntu:~$ sudo dd if=/dev/mtd0 count=1 2>/dev/null | hd
00000000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000200

# attempt to write (program) arbitrary data
clay@ubuntu:~$ sudo dd if=/dev/urandom of=/dev/mtd0 bs=512 count=1
1+0 records in
1+0 records out
512 bytes copied, 0.000195163 s, 2.6 MB/s

# all the bits are still zero
clay@ubuntu:~$ sudo dd if=/dev/mtd0 count=1 2>/dev/null | hd
00000000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000200
```

The `flash_erase` tool from the [mtd-utils] distribution will return all the
bits in a block (or an entire mtd partition, more on that below) to all-ones:

``` txt
clay@ubuntu:~$ sudo flash_erase /dev/mtd0 0 0
Erasing 16 Kibyte @ 7ffc000 -- 100 % complete 

clay@ubuntu:~$ sudo dd if=/dev/mtd0 bs=512 count=1 2>/dev/null | hd
00000000  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
*
00000200
```

Behind the scenes, `flash_erase` invokes `ioctl(MEMERASE)` on the mtd device.
Many such ioctls are defined, forming the userspace control interface to the
MTD subsystem, including:

``` c
/* Get basic MTD characteristics info (better to use sysfs) */
#define MEMGETINFO              _IOR('M', 1, struct mtd_info_user)

/* Erase segment of MTD */
#define MEMERASE                _IOW('M', 2, struct erase_info_user)

/* Write out-of-band data from MTD */
#define MEMWRITEOOB             _IOWR('M', 3, struct mtd_oob_buf)

/* Read out-of-band data from MTD */
#define MEMREADOOB              _IOWR('M', 4, struct mtd_oob_buf)

/* Lock a chip (for MTD that supports it) */
#define MEMLOCK                 _IOW('M', 5, struct erase_info_user)

/* Unlock a chip (for MTD that supports it) */
#define MEMUNLOCK               _IOW('M', 6, struct erase_info_user)

/* Check if chip is locked (for MTD that supports it) */
#define MEMISLOCKED             _IOR('M', 23, struct erase_info_user)

/* Check if an eraseblock is bad */
#define MEMGETBADBLOCK          _IOW('M', 11, __kernel_loff_t)

/* Mark an eraseblock as bad */
#define MEMSETBADBLOCK          _IOW('M', 12, __kernel_loff_t)

/*
 * Most generic write interface; can write in-band and/or out-of-band in various
 * modes (see "struct mtd_write_req")
 */
#define MEMWRITE                _IOWR('M', 24, struct mtd_write_req)
```

The [mtd-utils] distribution includes a static library (libmtd.a) that wraps the
MTD ioctls in a tidy API:

``` c
/**
 * mtd_get_dev_info - get information about an MTD device.
 * @desc: MTD library descriptor
 * @node: name of the MTD device node
 * @mtd: the MTD device information is returned here
 *
 * This function gets information about MTD device defined by the @node device
 * node file and saves this information in the @mtd object. Returns %0 in case
 * of success and %-1 in case of failure. If MTD subsystem is not present in the
 * system, or the MTD device does not exist, errno is set to @ENODEV.
 */
int mtd_get_dev_info(libmtd_t desc, const char *node, struct mtd_dev_info *mtd);

/**
 * mtd_read - read data from an MTD device.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to read from
 * @offs: offset withing the eraseblock to read from
 * @buf: buffer to read data to
 * @len: how many bytes to read
 *
 * This function reads @len bytes of data from eraseblock @eb and offset @offs
 * of the MTD device defined by @mtd and stores the read data at buffer @buf.
 * Returns %0 in case of success and %-1 in case of failure.
 */
int mtd_read(const struct mtd_dev_info *mtd, int fd, int eb, int offs,
             void *buf, int len);

/**
 * mtd_write - write data to an MTD device.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to write to
 * @offs: offset withing the eraseblock to write to
 * @data: data buffer to write
 * @len: how many data bytes to write
 * @oob: OOB buffer to write
 * @ooblen: how many OOB bytes to write
 * @mode: write mode (e.g., %MTD_OOB_PLACE, %MTD_OOB_RAW)
 *
 * This function writes @len bytes of data to eraseblock @eb and offset @offs
 * of the MTD device defined by @mtd. Returns %0 in case of success and %-1 in
 * case of failure.
 *
 * Can only write to a single page at a time if writing to OOB.
 */
int mtd_write(libmtd_t desc, const struct mtd_dev_info *mtd, int fd, int eb,
              int offs, void *data, int len, void *oob, int ooblen,
              uint8_t mode);

/**
 * mtd_read_oob - read out-of-band area.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @start: page-aligned start address
 * @length: number of OOB bytes to read
 * @data: read buffer
 *
 * This function reads @length OOB bytes starting from address @start on
 * MTD device described by @fd. The address is specified as page byte offset
 * from the beginning of the MTD device. This function returns %0 in case of
 * success and %-1 in case of failure.
 */
int mtd_read_oob(libmtd_t desc, const struct mtd_dev_info *mtd, int fd,
                 uint64_t start, uint64_t length, void *data);

/**
 * mtd_write_oob - write out-of-band area.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @start: page-aligned start address
 * @length: number of OOB bytes to write
 * @data: write buffer
 *
 * This function writes @length OOB bytes starting from address @start on
 * MTD device described by @fd. The address is specified as page byte offset
 * from the beginning of the MTD device. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_write_oob(libmtd_t desc, const struct mtd_dev_info *mtd, int fd,
                  uint64_t start, uint64_t length, void *data);

int mtd_probe_node(libmtd_t desc, const char *node);
/**
 * mtd_lock - lock eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to lock
 *
 * This function locks eraseblock @eb. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_lock(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_unlock - unlock eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to lock
 *
 * This function unlocks eraseblock @eb. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_unlock(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_erase - erase multiple eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: index of first eraseblock to erase
 * @blocks: the number of eraseblocks to erase
 *
 * This function erases @blocks starting at eraseblock @eb of MTD device
 * described by @fd. Returns %0 in case of success and %-1 in case of failure.
 */
int mtd_erase_multi(libmtd_t desc, const struct mtd_dev_info *mtd,
                        int fd, int eb, int blocks);

/**
 * mtd_erase - erase an eraseblock.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to erase
 *
 * This function erases eraseblock @eb of MTD device described by @fd. Returns
 * %0 in case of success and %-1 in case of failure.
 */
int mtd_erase(libmtd_t desc, const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_is_locked - see if the specified eraseblock is locked.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to check
 *
 * This function checks to see if eraseblock @eb of MTD device described
 * by @fd is locked. Returns %0 if it is unlocked, %1 if it is locked, and
 * %-1 in case of failure. If the ioctl is not supported (support was added in
 * Linux kernel 2.6.36) or this particular device does not support it, errno is
 * set to @ENOTSUPP.
 */
int mtd_is_locked(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_torture - torture an eraseblock.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to torture
 *
 * This function tortures eraseblock @eb. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_torture(libmtd_t desc, const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_is_bad - check if eraseblock is bad.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to check
 *
 * This function checks if eraseblock @eb is bad. Returns %0 if not, %1 if yes,
 * and %-1 in case of failure.
 */
int mtd_is_bad(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_mark_bad - mark an eraseblock as bad.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to mark as bad
 *
 * This function marks eraseblock @eb as bad. Returns %0 in case of success and
 * %-1 in case of failure.
 */
int mtd_mark_bad(const struct mtd_dev_info *mtd, int fd, int eb);
```

## NAND simulator

MTD includes a NAND simulator module (`nandsim`) that attaches a virtual MTD
device:

``` txt
clay@ubuntu:~$ sudo modprobe nandsim
clay@ubuntu:~$ mtdinfo -a
Count of MTD devices:           1
Present MTD devices:            mtd0
Sysfs interface supported:      yes

mtd0
Name:                           NAND simulator partition 0
Type:                           nand
Eraseblock size:                16384 bytes, 16.0 KiB
Amount of eraseblocks:          8192 (134217728 bytes, 128.0 MiB)
Minimum input/output unit size: 512 bytes
Sub-page size:                  256 bytes
OOB size:                       16 bytes
Character device major/minor:   90:0
Bad blocks are allowed:         true
Device is writable:             true
```

## Kernel API

Higher layer kernel subsystems (flash filesytems, [UBI][]) interact with raw
flash via the MTD API defined in [include/linux/mtd/mtd.h][], including:

``` c
int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf);
int mtd_erase(struct mtd_info *mtd, struct erase_info *instr);
int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
```

[UBI]: ubi.html
[include/linux/mtd/mtd.h]:
http://elixir.free-electrons.com/source/include/linux/mtd/mtd.h
[mtd-utils]: http://git.infradead.org/mtd-utils.git
