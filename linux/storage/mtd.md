# MTD

The Memory Technology Devices (MTD) subsystem provides a common interface to
many types of [raw flash](/hardware/storage/flash.html) storage devices. It
includes facilities for partioning flash, erasing flash, and managing bad
blocks.


## Listing devices and partitions

The `mtdinfo` command, provided as part of the [mtd-utils][] distribution,
displays all available MTD devices and partitions:

	$ mtdinfo
	Count of MTD devices:           5
	Present MTD devices:            mtd0, mtd1, mtd2, mtd3, mtd4
	Sysfs interface supported:      yes

Given a device argument (or `--all`), `mtdinfo` prints detailed device
information:

	$ mtdinfo /dev/mtd0
	mtd0
	Name:                           NAND 128MiB 1,8V 8-bit
	Type:                           nand
	Eraseblock size:                16384 bytes, 16.0 KiB
	Amount of eraseblocks:          8192 (134217728 bytes, 128.0 MiB)
	Minimum input/output unit size: 512 bytes
	Sub-page size:                  256 bytes
	OOB size:                       16 bytes
	Character device major/minor:   90:0
	Bad blocks are allowed:         true
	Device is writable:             true

Like many such userspace tools, `mtdinfo` is a thin wrapper around the MTD
SysFS interface in `/sys/class/mtd`.


## Devices

MTD provides a pair of character devices for each flash device and partition:

  * `/dev/mtdX` (read-write)
  * `/dev/mtdXro` (read-only)

Most [mtd-utils][] commands take a `/dev/mtdX` device node as an argument.

You can read/write directly from/to `/dev/mtdX` devices, but there are
complications:

  * reads can be any size, but writes must be a multiple of the device's
    minimum write size (as reported by `/sys/class/mtd/mtdX/writesize`)
  * blocks must be erased before they can be written
  * on NAND devices, reading from or writing to a bad block may produce
    unexpected results


## Partitions

MTD allows flash devices to be carved into partitions. Several on-flash
partition table formats are supported, as well as partitions defined on the
kernel command line or in the device tree.


## Utilities

The `flash_erase` utility from the [mtd-utils][] distribution erases all bytes
(data and out-of-band) of a device, partition, or subset of blocks:

	$ flash_erase /dev/mtd0 0 0
	Erasing 16 Kibyte @ 7ffc000 -- 100 % complete 

Reading an erased flash returns all-ones:

	$ dd if=/dev/mtd0 bs=512 count=1 | hd
	00000000  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
	*
	00000200

The `mtdpart` utility adds and deletes MTD partitions (but does not update
on-flash partition tables).


### NAND simulator

MTD includes a NAND simulator module (`nandsim`) that attaches a virtual MTD
device:

	$ modprobe nandsim

	$ mtdinfo -a
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


## Programming Interfaces

### `ioctl` ABI

Behind the scenes, the [mtd-utils][] commands interact with the MTD subsystem
via an `ioctl` interface:

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


### libmtd

The [mtd-utils] source builds a static library (libmtd.a) that wraps the MTD
ioctls in a tidy API, e.g., `mtd_mark_bad()`:

``` c
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

Sadly, libmtd.a is not included in the [mtd-utils][] binary distribution.


### Kernel API

Higher layer kernel subsystems (flash filesytems, [UBI][]) interact with raw
flash via the MTD API defined in [include/linux/mtd/mtd.h][], including:

``` c
int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf);
int mtd_erase(struct mtd_info *mtd, struct erase_info *instr);
int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
```

Device drivers register MTD devices with the MTD core via
[`mtd_device_register()`](http://elixir.free-electrons.com/ident?v=4.10&i=mtd_device_register).


[mtd-utils]: http://git.infradead.org/mtd-utils.git
[UBI]: ubi.html
[include/linux/mtd/mtd.h]:
http://elixir.free-electrons.com/source/include/linux/mtd/mtd.h
