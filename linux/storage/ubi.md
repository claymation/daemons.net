# UBI

UBI provides a logical volume management (LVM) layer on top of raw flash
devices. UBI volumes are defined in terms of logical erase blocks (LEBs),
which UBI maps to the physical erase blocks (PEBs) on the NAND device.

UBI transpartently manages block wear leveling, bad blocks, and bit flips,
freeing higher layer software from these concerns.


## Physical Erase Block layout

    ┌─────┬─────┬──────────────────────────────────────────────────┐
    │ EC  │ VID │ User data (LEB)                                  │
    └─────┴─────┴──────────────────────────────────────────────────┘
    ↑     ↑     ↑
    │     │     │
    │     │     └── offset determined by flash min. I/O unit
    │     └──────── offset determined by flash min. I/O unit or NAND sub-page
    └────────────── offset 0

    EC:  Erase counter header (64 bytes)
    VID: Volume ID header (64 bytes)

