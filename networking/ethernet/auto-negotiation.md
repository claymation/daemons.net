# Auto-Negotiation

It's easy to take Auto-Negotiation for granted. It's purpose, after all, is to ensure that Ethernet usually Just Works™. But it's a pretty cool, if overlooked, piece of technology in its own right, and knowing how it works can be invaluable for when it doesn't.

According to the [IEEE Standard for Ethernet][]:

> The objective of the Auto-Negotiation function is to provide the means to exchange information between two devices that share a link segment and to automatically configure both devices to take maximum advantage of their abilities.

"Abilities" in this context refers to speed, duplex, and more esoteric capabilities such as flow control, master/slave timing, Energy Efficient Ethernet, and vendor-defined features.

Auto-Negotiation is a physical layer function, and is implemented in the PHY. The MAC is none the wiser to the gyrations afoot at the lower layers. The standard  defines distinct Auto-Negotiation schemes to support different Ethernet media systems:

  * [Clause 28][] defines the Auto-Negotiation function for twisted pair media systems (like 10BASE-T, 100BASE-TX, 1000BASE-T, and 10GBASE-T)
  * [Clause 37][] defines the Auto-Negotiation function for the 1000BASE-X family of 1000 Mb/s media systems
  * [Clause 73][] defines the Auto-Negotiation function for backplane Ethernet (1000BASE-KX, 10GBASE-KR, etc) and high-speed short-range shielded copper media systems (40GBASE-CR4, 100GBASE-CR10, etc)

All three schemes provide a means for link partners to advertise their capabilities and select a highest common denominator technology using a simple priority resolution scheme. They vary in the signaling used to advertise capabilities, and in the specific capabilities applicable to that media system.

### Clause 28 Auto-Negotiation

The decision to reuse the 8P8C (RJ-45) connector from 10BASE-T during the development of Fast Ethernet allowed for the design of multispeed ports that could operate at 10BASE-T or 100BASE-TX. To mitigate the increased potential for configuration errors under such a scheme, the designers provided a means for automatically configuring the port, namely, [Clause 28][] Auto-Negotiation.

#### Signaling

Clause 28 builds on 10BASE-T link integrity test pulses. Following in [Ben Eater's footsteps](https://www.youtube.com/watch?v=i8CmibhvZ0c), I hooked an oscilloscope up to an Ethernet cable between two hosts to observe the signaling first-hand. 

Here's what a 10BASE-T link integrity test pulse (or normal link pulse, NLP) looks like:

![Oscilloscope waveform of a 10BASE-T link integrity test pulse](/static/images/networking/ethernet/auto-negotiation/normal-link-pulse.png)

Zooming out and disconnecting one end of the Ethernet cable to restart the Auto-Negotiation process, we can see a fast link pulse (FLP) burst occurring every ~16 ms:

![Oscilloscope waveform showing several fast link pulse bursts](/static/images/networking/ethernet/auto-negotiation/fast-link-pulse-bursts.png)

Zooming in on one of the FLP bursts shows that it's made up of several NLPs:

![Oscilloscope waveform of a fast link pulse burst](/static/images/networking/ethernet/auto-negotiation/decoded-fast-link-pulse-burst.png)

Each FLP burst consists of up to 33 NLPs. 17 regularly spaced clock pulses (appearing every ~125 us) are used to synchronize the receiver's clock. A data pulse can be inserted between clock pulses to indicate a logical 1. An absent data pulse represents a logical 0. Because there are 17 clock pulses, there's room for 16 data pulses in each FLP burst, enabling the transmission of a 16-bit page of Auto-Negotiation data.

#### Base Page

#### Next Pages

#### Parallel Detection

Auto-Negotiation is optional for 10BASE-T and 100BASE-TX, which pre-date Clause 28. To ensure backwards compatibility with legacy devices, Parallel Detection allows an AN-capable device to fallback to either 10BASE-T or 100BASE-TX if it detects link integrity test pulses or idle code words, respectively.

#### Gigabit and Beyond

Clause 28 Auto-Negotiation is required for 1000BASE-T and subsequent twisted pair standards so that link partners can negotiate their master/slave timing relationship.

### Reading

  * IEEE Standard for Ethernet: [Clause 28][]
  * IEEE Standard for Ethernet: [Clause 37][]
  * IEEE Standard for Ethernet: [Clause 73][]
  * [An Introduction to the Auto-Negotiation Process][]


[IEEE Standard for Ethernet]: http://standards.ieee.org/getieee802/download/802.3-2012_section1.pdf
[Clause 28]: http://standards.ieee.org/getieee802/download/802.3-2012_section2.pdf
[Clause 37]: http://standards.ieee.org/getieee802/download/802.3-2012_section3.pdf
[Clause 73]: http://standards.ieee.org/getieee802/download/802.3-2012_section5.pdf
[An Introduction to the Auto-Negotiation Process]: https://www.iol.unh.edu/sites/default/files/knowledgebase/ethernet/Copper_ANEG_JEFF_LAPAK.pdf
