# Segmentation

## Real mode

How could the 8086 microprocessor, with its 16-bit registers and data bus,
address 20 bits (1 MiB) of memory? Segmentation. Physical addresses were
constructed from two 16-bit registers: segment and offset.

Segment registers:

  * `cs`: code segment
  * `ss`: stack segment
  * `ds`: data segment
  * `es`: extra segment

Examples:

    cs:ip == (cs << 4) + ip
    ss:sp == (ss << 4) + sp
    ds:si == (ds << 4) + si
    es:di == (es << 4) + di

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
