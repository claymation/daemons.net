# Segmentation

## Real mode

How could the 8086 microprocessor, with its 16-bit registers and data bus,
address 20 bits (1 MiB) of memory? By adding _two_ 16-bit registers — a segment
register and an offset register — with the segment register shifted four bits
to the left (equivalent to multiplying by 16):

                 Segment register
        ┌───────┬───────┬───────┬───────┐
        │   1   │   2   │   3   │   4   │
        └───────┴───────┴───────┴───────┘
         15                            0

                         Offset register
                ┌───────┬───────┬───────┬───────┐
      +         │   1   │   2   │   3   │   4   │
                └───────┴───────┴───────┴───────┘
                 15                            0

        ===========================================

                     Physical Address
        ┌───────┬───────┬───────┬───────┬───────┐
        │   1   │   3   │   5   │   7   │   4   │
        └───────┴───────┴───────┴───────┴───────┘
         19                                    0

It's a clever trick, but has two quirks:

1. While any given segment and offset uniquely define exactly one physical
   address, the converse is not true; any given physical address can be
   constructed from several combinations of segments and offsets.

   For example, the following segment:offset pairs produce equivalent physical
   addresses:

         F000          FF00          FFF0          FFFF 
       +  FFFF       +  0FFF       +  00FF       +  000F
         =====         =====         =====         =====
         FFFFF    ≡    FFFFF    ≡    FFFFF    ≡    FFFFF

2. It is possible to overflow the 20-bit physical address, in which case the
   address wraps:

         FFFF
       +  FFFF
         =====
         0FFEF (NOT 10FFEF)

   Some software written for the 8086/8088 depended on this wrap-around. When
   the 80286 shipped in 1982 with a 24-bit address bus, system designers opted
   to disable its 20th address pin ("A20") for backwards compatibility with the
   previous processors. System software had to enable the A20 line in order to
   address more than 1 MiB of memory.


### Segments

IA-32 defines six segment registers, each with a specific purpose:

  * `cs`: code segment
  * `ss`: stack segment
  * `ds`: data segment
  * `es`: extra segment
  * `fs`: 
  * `gs`: 


## Protected mode

The 16-bit segment registers are repurposed to hold a segment selector, which
points to a segment descriptor in a segment descriptor table:

        Segment Selector
               in
      Segmentation Register
    ┌────────────┬────┬─────┐          Global/Local
    │ index      │ TI │ RPL │        Descriptor Table
    └────────────┴────┴─────┘     ┌────────────────────┐
     15    │    3   2  1   0      │         ...        │
           │                      ├────────────────────┤
           └─────────────────────>│ Segment Descriptor │
                                  ├────────────────────┤
                                  │         ...        │
                                  └────────────────────┘
                                             ^
                                             │
                                             └─────── gdtr/ldtr

The segment selector's Table Indicator (TI) bit determines whether the selector
points to the GDT (TI = 0) or LDT (TI = 1).

The segment selector's Requestor Privilege Level (RPL) sets the CPU's Current
Privilege Level (CPL) when the selector is loaded into the `cs` register.

Segment descriptors are 8-byte structures describing the base, size, type, and
other attributes of a memory segment:

     63          56  55       51           48       43  40 39          32
    ┌──────────────┬───┬─────┬───────────────┬─────┬──────┬──────────────┐
    │ base (24-31) │ G │ ... │ limit (16-19) │ ... │ type │ base (16-23) │
    ├──────────────┴───┴─────┴───────────┬───┴─────┴──────┴──────────────┤
    │              base (0-15)           │           limit (0-15)        │
    └────────────────────────────────────┴───────────────────────────────┘
     31                                                                 0

The 32-bit base can address 4 GiB. The 20-bit limit (size) supports up to 1 MiB
segments with byte-sized granularity (G = 0) or up to 4 GiB segments with 4 KiB
page-sized granularity (G = 1).

## Reading

  * [Intel® 64 and IA-32 Architectures Software Developer Manuals][]

[Intel® 64 and IA-32 Architectures Software Developer Manuals]: https://software.intel.com/en-us/articles/intel-sdm
