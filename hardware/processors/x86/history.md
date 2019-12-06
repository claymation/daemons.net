# History

## 8086 (1978)

* 16-bit data registers (AX, BX, CX, DX)
* 16-bit address registers (SP, BP, SI, DI)
* 16-bit segment registers (CS, DS, ES, SS)
* 16-bit external data bus
* 20-bit external address bus (1 MiB addressability)

## 8088 (1979)

An 8086 with an 8-bit external data bus. The processor in IBM's PC-XT.

## 80188 (1980)

Reduced chip count versions of the 8088.

## 80186 (1982)

Reduced chip count versions of the 8086.

## 80286 (1982)

* 24-bit external address bus (16 MiB)
* Protected mode with four privilege levels

## 80386 (1985)

* 32-bit registers
* 32-bit external data bus
* 32-bit external address bus (4 GiB)
* Segmented and flat memory models
* 4-KiB paged virtual memory

## 80486 (1989)

* 5-stage pipeline
* 8-KiB cache
* Optional integrated x87 math co-processor (FPU)

## Pentium (1993)

* 64-bit external data bus
* Some models with 64-bit MMX registers
* Superscalar (two pipelines = two instructions per clock)
* 8-KiB instruction + data caches
* Branch prediction unit
* APIC

## Later models

After the Pentium, the Intel processor family tree gets a bit confusing,
with myriad marketing names (Pentium Pro, Pentium II, Pentium III, Pentium 4, 
Pentium M, Pentium D, and Pentium Extreme Edition, to name a few) that sound
like minor variations on a theme, but in fact represent radically new designs
based on a bevy of microarchitectures:

* P6 Family
* NetBurst
* Pentium M
* Core
* Enhanced Core
* Atom
* Nehalem
* Westmere
* Sandy Bridge
* Ivy Bridge
* Ivy Bridge-E
* Haswell
* Haswell-E
* Airmont
* Silvermont
* Broadwell
* Skylake
* Kaby Lake
* Goldmont
* Knights Landing
* Goldmont Plus
* Coffee Lake
* Kinghts Mill
* Cascade Lake
* Ice Lake

See [WikiChip's Intel Microarchitectures][] page for the gory details.

# Reading

* [Intel Microprocessors 8008 to 8086 and beyond][]
* [Intel® 64 and IA-32 Architectures Software Developer Manuals][]
* [WikiChip's Intel Microarchitectures][]

[Intel Microprocessors 8008 to 8086 and beyond]: https://stevemorse.org/8086history/8086_evolution.pdf
[Intel® 64 and IA-32 Architectures Software Developer Manuals]: https://software.intel.com/en-us/articles/intel-sdm
[WikiChip's Intel Microarchitectures]: https://en.wikichip.org/wiki/intel/microarchitectures
