# Flash

Flash is a generic term for non-volatile solid-state memory. It differs from
traditional block storage in fundamental ways:

  * Flash requires distinct erase and write operations
  * Flash eraseblocks are many times larger than block device sectors
  * Flash eraseblocks wear out after 10^3 - 10^5 program/erase cycles
  * Higher layers have to contend with bad eraseblocks (block devices
    typically insulate higher layers from bad blocks)

Counterintuitively, the flash erase operation sets bits to the logic high (1)
state, while the program operation sets bits to the logic low (0) state.


## NOR flash

NOR flash is available in megabit to gigabit densities and serial (SPI) or
parallel interfaces.

NOR flash is organized into sectors (typ. 64 kB) of pages (typ. 256 bytes).
The sector is the smallest unit that can be erased. Erasing is typically slow,
on the order of hundreds of milliseconds per sector, requiring several seconds
to erase the entire device.

NOR flash is byte-addressable, enabling random-access I/O. Individual bytes
can be read and programmed. Reads are fast: sustained transfer rates of tens
of megabytes per second are possible. Writes are slow: kilobytes per second.

Because of its good random-access read performance, NOR flash can be used for
execute-in-place (XIP) applications, such as bootloaders. 

### Command set

NOR flash chips commonly use one-byte commands to initiate read, program, and
erase cycles and to read status and identification registers. Common commands
include:

  * write enable / disable
  * read identification
  * read / write status register
  * read data bytes
  * page program
  * sector erase
  * bulk erase

### CFI

The [Common Flash Interface (CFI)][] specification defines a protocol for
software to query a flash device's identity and geometry.


## NAND flash

NAND flash offers higher densities (up to several terabits) and faster program
and erase cycles than NOR flash, at the expense of no random-access
capability.

NAND flash density is determined by the cell technology:

  * SLC (single-level cell) NAND stores one bit per cell; fastest, most durable,
    most expensive
  * MLC (multilevel cell) NAND stores two bits per cell; slower, less durable,
    less expensive
  * TLC (triple-level cell) NAND stores three bits per cell; slowest, least
    durable, least expensive

Recent innovations in 3D NAND technology allow MLC and TLC memory to achieve
performance and durability comparable to SLC memory.

NAND flash is organized into blocks (typ. 128 - 256 kB) of pages (typ. 2 - 4
kB). The block (aka eraseblock) is the smallest unit that can be erased. The
page is nominally the smallest unit that can be read or programmed, though
some NAND flashes support sub-page reads and writes.

Most NAND flash is parallel, though serial devices are available.

### ONFI

The [Open NAND Flash Interface (ONFI)][] specification describes a parameter
page that software can use to determine chip identity and geometry.

### Flash translation layer

A flash translation layer (FTL) can be used to emulate a traditional block
device on top of raw NAND flash.

Common FTL implementations include:

  * CompactFlash (CF)
  * MultiMedia Card (MMC/eMMC)
  * Secure Digital (SD/SDHC/SDXC)
  * USB flash drives

[Common Flash Interface (CFI)]:
http://www.jedec.org/sites/default/files/docs/jesd68-01.pdf
[Open NAND Flash Interface (ONFI)]:
http://www.onfi.org/~/media/onfi/specs/onfi_4_0-gold.pdf?la=en
