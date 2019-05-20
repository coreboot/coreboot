# ME Cleaner
It's possible to 'clean' the ME partition within the flash medium as part
of the build process. While cleaning as much code as possible is removed
from the ME firmware partition. In this state the ME errors out and doesn't
operate any more.

**Using a 'cleaned' ME partition may lead to issues and its use should be
carefully evaulated.**

## Observations with 'cleaned' ME

* Instable LPC bus
  * SuperIO is malfunctioning
  * TPM is malfunctioning
  * Random system shutdowns on high bus activity

## Filing bug reports

Always test with unmodified IFD and ME section before reporting bugs to the
coreboot project.
