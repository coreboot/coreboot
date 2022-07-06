IFWI Boot Flow
==============

 ╔═══════════════════════════════════════════════════════════╗
 ║                 BIOS Region (512 KiB maximum)             ║
 ║ ╔════════════════════════════════════════════╗ ╔════════╗ ║
 ║ ║                  IBBM                      ║ ║  IBBL  ║ ║
 ║ ╚════════════════════════════════════════════╝ ╚════════╝ ║
 ╚═══════════════════════════════════════════════════════════╝

 When we enter the bootblock, the first 128k will be copied
 into the SRAM. This will contain the IBBL partition (bootblock)
 and whatever will fit in the IBB partition.

 We can't touch the bootblock as we are running from it. The RBP
 can handle a maximum of 0x9be2 per copy, so the contents
 of the SRAM is typically copied in 4 equally sized chunks of
 0x8000.

 ╔════════════════════╗    ╔════════════════════╗
 ║   SRAM (128 KiB)   ║    ║        CAR         ║
 ║ ╔═════╗ ╔════════╗ ║    ║ ╔════╗             ║
 ║ ║>IBBM║ ║  IBBL  ║ ║    ║ ║IBBM║             ║
 ║ ╚═════╝ ╚════════╝ ║    ║ ╚════╝             ║
 ╚════════════════════╝    ╚════════════════════╝
     🠗                                                🠕
      🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖🠖

 This means that from the bootblock, we copy the IBB in chunks of
 0x8000 (or less if the IBB is smaller than a chunk) into CAR.

                                        ╔════════════════════╗
                                        ║        CAR         ║
                                        ║ ╔════════════════╗ ║
                                        ║ ║      IBBM      ║ ║
                                        ║ ╚════════════════╝ ║
                                        ╚════════════════════╝

 We mark it as executable by flushing the L1 cache and then jump
 to the verstage in CAR. The first thing we do is set the IBBL to
 be uncachable, as we no longer have a use for it.

 ╔════════════════════╗
 ║   SRAM (128 KiB)   ║
 ║ ╔════════════════╗ ║
 ║ ║                ║ ║
 ║ ╚════════════════╝ ║
 ╚════════════════════╝

 We keep copying the chunks until there is none left, then send
 one final acknowledgement to the CSE - so it can know that the
 bootflow is complete.
