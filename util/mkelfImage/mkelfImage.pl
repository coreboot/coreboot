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
$params{CC}="gcc";
$params{CFLAGS}="-O2";
$params{MYDATA}=".";
$params{PREFIX}=undef();
# Parameters that get set...
$params{RAMDISK}="";
$params{VMLINUX}="/usr/src/linux/vmlinux";
$params{TARGET}="elfImage";
$params{ROOT_DEV}='((0x3<<16)| 0)';
$params{COMMAND_LINE}='';
$params{OUTPUT_FORMAT}='elf32-i386';
$params{INITRD_BASE}=0x00800000; # 8MB


sub compile_file
{
	my ($params, $src, $dst) = @_;
	my ($options, $cmd);
	$options = "-DDEFAULT_ROOT_DEV='((unsigned short)$params->{ROOT_DEV})'";
	$options .= " -DDEFAULT_COMMAND_LINE='\"$params->{COMMAND_LINE}\"'";
	if (defined($params->{RAMDISK_IMAGE_START})) {
		$options .= " -DDEFAULT_RAMDISK_IMAGE_START=" . 
		    "'((unsigned short)$params->{RAMDISK_IMAGE_START})'";
	}
	if ($params->{RAMDISK_PROMPT_FLAG}) {
		$options .= " -DDEFAULT_RAMDISK_PROMPT_FLAG";
	}
	if ($params->{RAMDISK_LOAD_FLAG}) {
		$options .= " -DDEFAULT_RAMDISK_LOAD_FLAG";
	}
	$cmd = "$params->{CC} $params->{CFLAGS} $options -c $params->{PREFIX}/$src -o $dst";
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
		system("objcopy ${src} -O binary ${dst}.obj");
		die "rc = $?" unless ($? == 0);
	}
	elsif ($bootsector_sig == 0xAA55) {
		# It's an x86 boot sector 
		# Pull out the kernel...
		my ($setupsects, $flags, $syssize, $swapdev, 
			$ramsize, $vidmode, $rootdev);
		(undef, $setupsects, $flags, $syssize, $swapdev, $ramsize, 
			$vidmode, $rootdev) = unpack('a497Cv6', $buffer);
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
	my $cmd = "$params->{LD} -o ${dst}  -T $script " . join(" ", @srcs);
	system("$cmd");
	die "rc = $?" unless ($? == 0);
	unlink("${dst}.obj",$lscript);
	return $dst;
}

sub build
{
	my ($params) = @_;
	my @objects;
	my $tempdir=getcwd();
	
	$params->{PREFIX} = "$params->{MYDATA}/$params->{OUTPUT_FORMAT}";
	push(@objects,build_kernel_piggy($params, "kernel", 
		$params->{VMLINUX}, "$tempdir/kernel_piggy_$$.o"));
	
	push(@objects, build_ramdisk_piggy($params, "ramdisk",
		$params->{RAMDISK}, "$tempdir/ramdisk_piggy_$$.o"));

	push(@objects, compile_file($params, "convert_params.c",
		"$tempdir/convert_params_$$.o"));
	push(@objects, compile_file($params, "head.S", 
		"$tempdir/head_$$.o"));
	build_elf_image($params, $params->{TARGET}, @objects);
	unlink(@objects);
	
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
		   # FIXME figure out what the old RAMDISK_IMAGE flags are about.
		   #'ramdisk-image-start=i' => \$params{RAMDISK_IMAGE_START},
		   #'ramdisk-prompt-flag!' => \$params{RAMDISK_PROMPT_FLAG},
		   #'ramdisk-load-flag!' => \$params{RAMDISK_LOAD_FLAG},
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
here is that nothing relies upon BIOS, but they are made when
necessary useful for systems running linuxbios. 

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

