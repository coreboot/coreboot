#!/usr/bin/perl -w
#
# This program is (C) 2000 by Eric Biederman  
#

use FileHandle;
use Getopt::Long;
use Cwd;

my %params;
my $VERSION="";
# Hardcoded parameters for now...
$params{OBJCOPY}="objcopy";
$params{LD}="ld";
$params{CC}="cc";
$params{CFLAGS}="-O2";
$params{MYDATA}=".";
$params{PREFIX}=undef();
# Parameters that get set...
$params{RAMDISK}="";
$params{VMLINUX}="/usr/src/linux/vmlinux";
$params{TARGET}="elfImage";
$params{ROOT_DEV}='((0x3<<8)| 0)';
$params{COMMAND_LINE}='';
$params{PROGRAM_VERSION}='';
$params{OUTPUT_FORMAT}='elf32-i386';
$params{INITRD_BASE}=0x00800000; # 8MB


sub compile_file
{
	my ($params, $src, $dst) = @_;
	my ($options, $cmd);
	$options = "-DDEFAULT_ROOT_DEV='($params->{ROOT_DEV})'";
	$options .= " -DDEFAULT_COMMAND_LINE='\"$params->{COMMAND_LINE}\"'";
	$options .= " -DDEFAULT_PROGRAM_VERSION='\"$params->{PROGRAM_VERSION}\"'";
	$cmd = "$params->{CC} $params->{CFLAGS} $options -c $params->{PREFIX}/$src -o $dst";
	print " Running $cmd \n";
	system($cmd);
	die "$cmd\nrc = $?" unless ($? == 0);
	return $dst;
}

sub build_kernel_piggy
{
	my ($params, $section, $src, $dst) = @_;
	my ($buffer, $elf_sig, $bootsector_sig);

	$fd_in = new FileHandle;
	$fd_in->open("<$src") or die "Cannot open $src\n";
	$fd_in->read($buffer, 512) == 512 
		or die "Error reading boot sector of $src";
	($elf_sig, undef, $bootsector_sig) = unpack('a4a506v1', $buffer);

	if ($elf_sig eq "\x7FELF") {
		# It's an elf image
		# Assume the input file uses contiguous memory...
		system("$params->{OBJCOPY} ${src} -O binary ${dst}.obj");
		die "rc = $?" unless ($? == 0);
	}
	elsif ($bootsector_sig == 0xAA55) {
		# It's an x86 boot sector 
		# Pull out the kernel...
		my ($setupsects, $flags, $syssize, $swapdev, 
			$ramsize, $vidmode, $rootdev);
		(undef, $setupsects, $flags, $syssize, $swapdev, $ramsize, 
			$vidmode, $rootdev) = unpack('a497Cv6', $buffer);
		$fd_in->read($buffer, 32768) == 32768 
		    or die "Error reading setup sector of $src";
		my (undef, $header, $version, undef, $start_sys, $kver_addr)
			= unpack('a2a4Sa4SS', $buffer);
		if ($header ne 'HdrS') {
			die "Not an linux kernel";
		}
		if ($setupsects == 0) {
			$setupsects = 4;
		}
		$params->{PROGRAM_VERSION} = unpack('Z32768', substr($buffer, $kver_addr));
		
		my $fd_out = new FileHandle;
		$fd_out->open(">${dst}.obj") or die "Cannot open ${dst}.obj";
		$fd_in->seek(512 + (512*$setupsects), 0);
		while ($fd_in->read($buffer, 8192) > 0) {
			$fd_out->print($buffer);
		}
		$fd_in->close();
		$fd_out->close();
	}
	else {
		die "Unkown kernel file type";
	}

	my $fd = new FileHandle;
$fd->open("| $params->{LD} -r -o ${dst}  -T /dev/fd/0 -b binary ${dst}.obj");
$fd->print( << "EOSCRIPT");
OUTPUT_FORMAT($params->{OUTPUT_FORMAT});
SECTIONS {
    .$section : { 
        ${section}_data = . ; 
        *(*) 
        ${section}_data_end = . ; 
    }
}
EOSCRIPT
	$fd->close();
	die "rc = $?" unless ($? == 0);
	unlink("${dst}.obj");
	return $dst;
}

sub build_ramdisk_piggy
{
# Assumes input file uses continguos memory...
	my ($params, $section, $src, $dst) = @_;
	my $fd = new FileHandle;
	if (defined($src) && ($src ne "")) {
		print " Running cp ${src} ${dst}.obj \n";
		system("cp ${src} ${dst}.obj");
	}
	else {
		# Now create the dummy file
		$fd->open(">${dst}.obj") or die "${dst}.obj: $!";
		$fd->close();
	}
$fd->open("| $params->{LD} -r -o ${dst}  -T /dev/fd/0 -b binary ${dst}.obj");
$fd->print( << "EOSCRIPT");
OUTPUT_FORMAT($params->{OUTPUT_FORMAT});
SECTIONS {
    .$section : { 
        ${section}_data = . ; 
        *(*) 
        ${section}_data_end = . ; 
    }
}
EOSCRIPT
	$fd->close();
	die "rc = $?" unless ($? == 0);
	unlink("${dst}.obj");
	return $dst;
}

sub build_elf_image
{
	my ($params, $dst, @srcs) = @_;
	my $lscript = "mkelfImage.lds";
	my $fd = new FileHandle;
	$fd->open(">$lscript");
	$fd->print("initrd_base = $params->{INITRD_BASE};\n");
	$fd->close();
	my $script = "$params->{PREFIX}/elfImage.lds";
	my $cmd = "$params->{LD} -o ${dst}.fat  -T $script " . join(" ", @srcs);
	print " Running $cmd";
	system("$cmd");
	die "rc = $?" unless ($? == 0);
	my $cmd2 = "$params->{OBJCOPY} -O binary ${dst}.fat ${dst}"; 
	print " Running $cmd";
	system("$cmd2");
	die "rc = $?" unless ($? == 0);
	unlink("${dst}.obj",$lscript);
	unlink("${dst}.fat",$lscript);
	return $dst;
}

sub compute_ip_checksum
{
	my ($str) = @_;
	my ($checksum, $i, $size, $shorts);
	$checksum = 0;
	$size = length($str);
	$shorts = $size >> 1;
	# Perl has a fairly large loop overhead so a straight forward
	# implementation of the ip checksum is intolerably slow.
	# Instead we use the unpack checksum computation function,
	# and sum 16bit little endian words into a 32bit number, on at
	# most 64K of data at a time.  This ensures we do not overflow
	# the 32bit sum allowing carry wrap around to be implemented by
	# hand.
	for($i = 0; $i < $shorts; $i += 32768) {
		$checksum += unpack("%32v32768", substr($str, $i <<1, 65536));
		while($checksum > 0xffff) {
			$checksum = ($checksum & 0xffff) + ($checksum >> 16);
		}
	}
	if ($size & 1) {
		$checksum += unpack('C', substr($str, -1, 1));
		while($checksum > 0xffff) {
			$checksum = ($checksum & 0xffff) + ($checksum >> 16);
		}
	}
	$checksum = (~$checksum) & 0xFFFF;
	return $checksum;
}


sub add_ip_checksums
{
	my ($offset, $sum, $new) = @_;
	my $checksum;
	$sum = ~$sum & 0xFFFF;
	$new = ~$new & 0xFFFF;
	if ($offset & 1) {
		$new = (($new >> 8) & 0xff) | (($new << 8) & 0xff00);
	}
	$checksum = $sum + $new;
	if ($checksum > 0xFFFF) {
		$checksum -= 0xFFFF;
	}
	return (~$checksum) & 0xFFFF;
}


sub write_ip_checksum
{
	my ($file) = @_;
	my ($fd, $buffer, %ehdr, @phdrs, $key, $size, $i);
	my ($checksum, $offset) = 0;
	$fd = new FileHandle;
	$fd->open("+<$file") or die "Cannot open $file\n";
	$fd->read($buffer, 52) == 52 or die "Cannot read ELF header\n";
	@ehdr{e_ident, e_type, e_machine, e_version, e_entry, e_phoff, e_shoff,
		e_flags, e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, 
		e_shstrndx} = unpack('a16SSLLLLLSSSSSS', $buffer);
	$checksum = compute_ip_checksum($buffer);
	$offset += 52;


	## FIXME add some sanity checks here...
	#foreach $key (keys(%ehdr)) {
	#	 print "$key: $ehdr{$key}\n";
	#}

	$fd->seek($ehdr{e_phoff}, 0) or die "Cannot seek to $ehdr{e_phoff}\n";
	$size = $ehdr{e_phnum}*32;
	$fd->read($buffer, $size) == $size or die "Cannot read ELF Program header $size\n";
	for($i = 0; $i < $ehdr{e_phnum} ; $i++) {
		my %phdr;
		@phdr{p_type, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, 
			p_flags, p_align} 
			= unpack('LLLLLLLL', substr($buffer, $i*32, 32));
		push(@phdrs, \%phdr);
	}
	$checksum = add_ip_checksums($offset, $checksum, 
		compute_ip_checksum($buffer));
	$offset += $size;


	for($i = 0; $i < scalar(@phdrs); $i++) {
		my $phdr = $phdrs[$i];
		#print("\n");
		#foreach $key (keys(%$phdr)) {
		#	 printf("%10s: %08x\n", $key, $phdr->{$key});
		#}
		# Only worry about PT_LOAD segments 
		next unless ($phdr->{p_type} == 1);
		$fd->seek($phdr->{p_offset}, 0) or die "Cannot seek to $phdr->{p_offset}\n";
		$fd->read($buffer, $phdr->{p_filesz}) == $phdr->{p_filesz} 
			or die "Cannot read ELF segment $phdr->{p_filesz}\n";
		$buffer .= "\0" x ($phdr->{p_memsz} - $phdr->{p_filesz});
		$checksum = add_ip_checksums($offset, $checksum, 
			compute_ip_checksum($buffer));
		$offset += $phdr->{p_memsz}
	}
	printf("checksum: %04x\n", $checksum);

	my $phdr = $phdrs[0];
	# Do I have a PT_NOTE segment?
	if ($phdr->{p_type} == 4) {
		my $offset;
		$fd->seek($phdr->{p_offset}, 0) or die "Cannot seek to $phdr->{p_offset}\n";
		$fd->read($buffer, $phdr->{p_filesz}) == $phdr->{p_filesz} 
			or die "Cannot read ELF segment $phdr->{p_filesz}\n";
		
		$offset = 0;
		while($offset < length($buffer)) {
			my %note;
			@note{n_namesz, n_descsz, n_type} 
				= unpack('LLL', substr($buffer, $offset, 12));
			$offset += 12;
			$note{n_name} = unpack("a$note{n_namesz}", 
				substr($buffer, $offset, $note{n_namesz}));
			$offset += ($note{n_namesz} + 3) & ~3;
			$note{n_desc} = unpack("a$note{n_descsz}",
				substr($buffer, $offset, $note{n_descsz}));
			
			#printf("n_type: %08x n_name(%d): %s n_desc(%d): %s\n",
			#	$note{n_type}, 
			#	$note{n_namesz}, $note{n_name},
			#	$note{n_descsz}, $note{n_desc});
			if (($note{n_namesz} == 8) &&
				($note{n_name} eq "ELFBoot\0") &&
				($note{n_type} == 3)) {
				my ($foffset, $buffer);
				$foffset = $phdr->{p_offset} + $offset;
				$buffer = pack('S', $checksum);
				$fd->seek($foffset, 0) or die "Cannot seek to $foffset";
				$fd->print($buffer);
				#print("checksum note...\n");
				
			}
			$offset += ($note{n_descsz} + 3) & ~3;
		}
	}
	$fd->close();
	
}

sub build
{
	my ($params) = @_;
	my @objects;
	my $tempdir=getcwd();
	
	$params->{PREFIX} = "$params->{MYDATA}/$params->{OUTPUT_FORMAT}";
	push(@objects, compile_file($params, "head.S", 
		"$tempdir/head_$$.o"));

	push(@objects, compile_file($params, "convert_params.c",
		"$tempdir/convert_params_$$.o"));

	push(@objects,build_kernel_piggy($params, "kernel", 
		$params->{VMLINUX}, "$tempdir/kernel_piggy_$$.o"));
	
	push(@objects, build_ramdisk_piggy($params, "ramdisk",
		$params->{RAMDISK}, "$tempdir/ramdisk_piggy_$$.o"));
	build_elf_image($params, $params->{TARGET}, @objects);
	unlink(@objects);
	write_ip_checksum($params->{TARGET});
	
}

sub main
{
	my ($params) = @_;
	my $wantversion;
	GetOptions('command-line=s' => \$params->{COMMAND_LINE},
		   'ramdisk=s' => \$params->{RAMDISK},
		   'vmlinux=s' => \$params->{VMLINUX},
		   'kernel=s' => \$params->{VMLINUX},
		   'root-dev=s' => \$params->{ROOT_DEV},
		   'output=s' => \$params->{TARGET},
		   'version' => \$wantversion,
		   'ramdisk-base=i' =>\$params->{INITRD_BASE},
		   );
	if (defined($wantversion) && $wantversion) {
		print "$0 $VERSION\n";
		exit(0);
	}
	build($params);
}

main(\%params, @ARGV);

__END__

=head1 NAME

mkelfImage - make an elf network bootable image for linux

=head1 SYNOPSIS

B<mkelfImage> [--command-line=I<command line>] [--kernel=I<path to vmlinux>] [--ramdisk=I<path to ramdisk>] [--output=I<file>] [--ramdisk-base=<start addr>]

=head1 DESCRIPTION

B<mkelfImage> is a program that makes a elf boot image for linux kernel
images.  The image should work with any i386 multiboot compliant boot loader,
an ELF bootloader that passes no options, a loader compliant with the linuxBIOS
elf booting spec or with the linux kexec kernel patch.  A key feature
here is that nothing relies upon BIOS calls, but they are made when
necessary.  This is useful for systems running linuxbios.


=head1 BUGS

Not all kernel parameters can be passed with the multiboot image format.
ip configuration is not automatically passed to a node.
The ramdisk base is hard coded to 8MB by default.

=head1 SEE ALSO

The exec kernel patch.
LinusBIOS.
etherboot.
The multiboot standard.

=head1 COPYRIGHT

mkelfImage is under the GNU Public License version 2

=head1 AUTHOR

Eric Biederman <ebiederman@lnxi.com>

