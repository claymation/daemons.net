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

As you would expect with flash, reading an erased flash gives bytes with all
bits set (0xff):

``` txt
clay@ubuntu:~$ sudo dd if=/dev/mtd0 bs=512 count=1 2>/dev/null | hd
00000000  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
*
00000200
```

Writing the flash can reset the bits to zero, but can not set them back to
one:

``` txt
# program all the bits to zero
clay@ubuntu:~$ sudo dd if=/dev/zero of=/dev/mtd0 bs=512 count=1
1+0 records in
1+0 records out
512 bytes copied, 0.0021276 s, 241 kB/s

# attempt to write arbitrary data
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


## NAND simulator

MTD includes a NAND simulator module that attaches a virtual MTD device as mtd0:

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

Higher layer kernel subsystems (flash filesytems, UBI) interact with raw
flash via the MTD API defined in [include/linux/mtd/mtd.h][], including:

``` c
int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf);
int mtd_erase(struct mtd_info *mtd, struct erase_info *instr);
int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
```

[include/linux/mtd/mtd.h]:
http://elixir.free-electrons.com/source/include/linux/mtd/mtd.h
