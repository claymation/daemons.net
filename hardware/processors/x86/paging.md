# Paging

The processor views main memory as an array of 4 KiB page frames, and carves
virtual address space into 4 KiB pages that can be loaded into any page frame,
or none at all (paged out to a swap area, for example).

## 32-bit

The 32-bit linear address produced by the segmentation unit is mapped to a
32-bit physical address by the memory management unit (MMU) using a two-level
page table tree:
     
                  32-bit Linear Virtual Address
     ┌─────────────────┬────────────────┬─────────────────────┐
     │    Directory    │      Table     │        Offset       │
     └─────────────────┴────────────────┴─────────────────────┘
      31      │      22 21     │      12 11              │    0
              │                │                         │
              │                │                         │
              │                │     Page                │
              │                │     Table               │
              │                │   ┌───────┐             │
              │                │   │  ...  │             │
              │    Page        │   │  ...  │             │
              │  Directory     │   ├───────┤             │
              │  ┌───────┐     └──>│  pte  │─────────┐   │
              │  │  ...  │         ├───────┤         V   V
              │  │  ...  │         │  ...  │     0xAAAAABBB
              │  ├───────┤         │  ...  │   32-bit Physical
              └─>│  pte  │────────>└───────┘      Address
                 ├───────┤           4 KiB
                 │  ...  │
                 │  ...  │
        cr3 ────>└───────┘
                   4 KiB
