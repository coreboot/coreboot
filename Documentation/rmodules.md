# Relocatable Modules (rmodules)

Relocatable modules are currently only used on x86.  Relocatable
modules are executables. Exectuables which can be executed anywhere in
memory. Anywhere means that the module does not need to be executed
at a defined memory address which is known at build/link time. For
coreboot stages like bootblock and romstage it is known at build
time at which addresses they are executed.  For some exectuables it
is however not known at which specific address they are executed in
runtime (for example postcar and ramstage). Relocateable modules
usually allocate the space for the modules just before they are
supposed to be executed. After enough space is allocated, CBMEM will
return the location of the allocated space. Now the relocation can be
done by fixing up all relocation entries in the relocatable module
based on the location of the binary (which was returned by CBMEM
at runtime).

## Implementation Details

### Build Time

At build time the rmodtool (util/cbfstool/rmodtool.c) is used to
create relocatable modules. The rmodtool basically takes an ELF
file as an input and writes an ELF as output. It basically does
a simple conversion from one ELF file to another slighty changed
ELF file. First the tool makes sure that the ELF file fits a few
requirements. For example there can only be one segment (loadable
program header) in the input ELF file. After that it goes through
the ELF relocation table and takes any entry that applies to the one
segment we want to load at runtime. The rmodtool will then write all
these relocation entires in a new ELF section called ".reloc". After
that the ELF relocation table will be cleared.

One can split the rmodules in two different kinds:
1. coreboot stages (postcar, ramstage)
2. simple binaries (smm, smmstub, sipi\_vector)

They are actually handled the same by the build system and only differ
in the fact, that they are either coreboot stages or they are not.

In the end the ELF files will have three different ELF sections,
which are all created by the rmodtool.
1. relocation header (.header)
2. program (.program)
3. relocation entries (.relocs)

### Runtime

Either rmodule\_load (lib/rmodule.c) is used directly or through the
rmodule\_stage\_load (lib/rmodule.c) wrapper. It is used to load the
stages (postcar and ramstage) or small programs like (sipi\_vector,
smm, smmstub) into memory before jumping to them. In the case of a
coreboot stage, CBMEM is used to allocate space for the stage in memory
via the rmodule\_cbfs\_allocater (lib/rmodule.c). At this point the
location of the stage in memory is known and all relocation (address
fixups) need to be done now. This is basically just a simple loop that
goes through each relocation entry. Each relocation entry is just an
address pointing to a location that needs relocation. The relocation
itself is just a simple addition, that adds an offset from where the
image was "supposed" to be at link time, to where it is now relocated.

### module\_parameters

module\_parameters is a section inside the rmodule ELF file. Its
basically a way to pass runtime information to an rmodule
before jumping to it. The caller will use rmodule\_parameters()
(lib/rmodule.c) to get the runtime address of the module\_parameters
and the callee (the rmodule itself) usually appends the section to
specific types via compiler attributes. For example:
```
static const
volatile __attribute((aligned(4), __section__(".module_parameters")))
struct smm_runtime smm_runtime;
```

## x86 why rmodules
//TODO
x86: postcar and ramstage cannot conflict with payload regarding
memory placement. Therefore payload location is usually fixed and
postcar/ramstage can be placed at a location in memory that is
figured out at runtime.
