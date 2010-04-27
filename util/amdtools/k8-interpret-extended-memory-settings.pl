#!/usr/bin/perl -w
use Getopt::Long;

use strict;

my $NAME = $0;
my $VERSION = '0.01';
my $DATE = '2009-09-04';
my $AUTHOR = "Ward Vandewege <ward\@jhvc.com>";
my $COPYRIGHT = "2009";
my $LICENSE = "GPL v3 - http://www.fsf.org/licenses/gpl.txt";
my $URL = "http://coreboot.org";

my $DEBUG = 0;

our %info;

$|=1;

&main();

sub version_information {
  my ($NAME,$VERSION,$DATE,$COPYRIGHT,$AUTHOR,$LICENSE,$URL) = (shift,shift,shift,shift,shift,shift,shift);
  print "\nThis is $NAME version $VERSION ($DATE)\n";
  print "Copyright (c) $COPYRIGHT by $AUTHOR\n";
  print "License: $LICENSE\n";
  print "More information at $URL\n\n";
  exit;
}

sub usage_information {
  my $retval = "\n$NAME v$VERSION ($DATE)\n";
  $retval .= "\nYou have not supplied all required parameters. $NAME takes these arguments:\n";
  $retval .= " $NAME -f <filename1> -f <filename2>\n\n";
  $retval .= "  -f <filename1>    is the name of a file with k8 memory configuration values\n";
  $retval .= "  -f <filename2>    is the name of a second file with k8 memory configuration values, to compare with filename1\n";
  $retval .= "  -v (optional)  provides version information\n";
  $retval .= "\nSee the k8-read-mem-settings.sh script for an example of how to generate the input files to this script.\n\n";
  print $retval;
  exit;
}

sub parse_file {
    my $register = '';
    my $devreg = '';
    my $filename = shift;
    my %data = @_;
    open(TMP, $filename) || die "Could not open $filename: $!\n";
    while (<TMP>) {
        chomp;
        # Line format - pairs of lines:
        # 0:18.2 98.l: 80000000
        # 0:18.2 9C.l: 10111222
        # First field is pci device. Second field is register offset (hex)
        # where third field value (in hex) was read from.
        my @tmp = split(/ /);
        $tmp[1] =~ s/:$//;  # strip optional trailing colon on second field

        my $device = $tmp[0];
        my $packed = pack("H*",$tmp[2]);    # Pack our number so we can easily represent it in binary
        my $binrep = unpack("B*", $packed); # Binary string representation

        if ($tmp[1] eq '98.l') {
            $register = ($tmp[2] =~ /(..)$/)[0]; # last 2 digits are (hex) of what we wrote to the register, if second field is 98.l
            $devreg = "$device $register";
            if ("$binrep" =~ /^1/) {
                # bit 31 *must* be 1 if readout is to be correct
                print "$tmp[0] - $register<br>\n" if ($DEBUG);
            } else {
                print "ERROR: we read too fast: $tmp[2] does not have bit 31 set ($binrep)\n";
                exit;
            }
        } else {
            # last field is register value (hex)
            print "$tmp[2]h ($binrep)<br>\n" if ($DEBUG);
            $data{$devreg} = {} if (!defined($data{$devreg}));
            $data{$devreg}{$filename} = $packed;
        }
    }
    return %data;
}

sub interpret_differences {
    my $reg = shift;
    $reg = sprintf("%02s",$reg);
    my $tag1 = shift;
    my $val1 = shift;
    my $tag2 = shift;
    my $val2 = shift;
    my $retval = '';
    my $retval2 = '';

    # XOR values together - the positions with 1 after the XOR are the ones with the differences
    my $xor = $val1 ^ $val2;

    my @val1 = split(//,unpack("B*",$val1));
    my @val2 = split(//,unpack("B*",$val2));
    my @xor = split(//,unpack("B*",$xor));

    my %changed;

    if (!exists($info{$reg})) {
        print STDERR "MISSING DATA for register $reg\n";
        return '';
    }

    for (my $i=0; $i<=$#xor;$i++) {
      my $invi = 31 - $i;
      if ($xor[$i] eq '1') {
#print STDERR "REG: $reg INVI: $invi\n";
#print STDERR $info{$reg}{'fields'}{$invi} . "\n";
#print STDERR $info{$reg}{'fields'}{$invi}{'range'} . "\n";
        my $r = $info{$reg}{'fields'}{$invi}{'range'};
#        if (!exists($changed{$r})) {
#            $changed{$r}{'v1'} = '';
#            $changed{$r}{'v2'} = '';
#        }
#        $changed{$r}{'v1'} .= $val1[$i];
#        $changed{$r}{'v2'} .= $val2[$i];
        $changed{$r}{'v1'} = 1;
        $changed{$r}{'v2'} = 1;
      }
    }

    foreach my $r (keys %changed) {
        my $width = $info{$reg}{'ranges'}{$r}{'width'};
        #$changed{$r}{'v1'} = sprintf("%0" . $width . "sb",$changed{$r}{'v1'});
        #$changed{$r}{'v2'} = sprintf("%0" . $width . "sb",$changed{$r}{'v2'});
        #my $v1 = $changed{$r}{'v1'};
        #my $v2 = $changed{$r}{'v2'};
        my $v1 = substr(unpack("B*",$val1),31-$info{$reg}{'ranges'}{$r}{'end'},$info{$reg}{'ranges'}{$r}{'width'}) . 'b';
        my $v2 = substr(unpack("B*",$val2),31-$info{$reg}{'ranges'}{$r}{'end'},$info{$reg}{'ranges'}{$r}{'width'}) . 'b';

        my $desc = $info{$reg}{'ranges'}{$r}{'description'};
        $desc =~ s/\n+/<br>/g;

        $retval2 .= $info{$reg}{'ranges'}{$r}{'function'} . " (" . $info{$reg}{'ranges'}{$r}{'mnemonic'} . ") - Bits ($r)" . "<br>";
        $retval2 .= "&nbsp;&nbsp;<i>$desc</i><p>" if ($desc ne '');

        $v1 = $v1 . " (" . $info{$reg}{'ranges'}{$r}{'values'}{$v1} . ")" if (exists($info{$reg}{'ranges'}{$r}{'values'}{$v1}));
        $v2 = $v2 . " (" . $info{$reg}{'ranges'}{$r}{'values'}{$v2} . ")" if (exists($info{$reg}{'ranges'}{$r}{'values'}{$v2}));
        $retval2 .= sprintf("<b><a href=\"$tag1\">%44s</a>: %s</b>\n",$tag1, $v1);
        $retval2 .= sprintf("<b><a href=\"$tag2\">%44s</a>: %s</b>\n",$tag2, $v2);
        $retval2 .= "<p>";
    }


# this prints out the bitwise differences. TODO: clean up

#    for (my $i=0; $i<=$#xor;$i++) {
#        my $invi = 31 - $i;
#        if ($xor[$i] eq '1') {
#            my $m = $info{$reg}{'fields'}{$invi}{'mnemonic'};
#            my $f = $info{$reg}{'fields'}{$invi}{'function'};
#            my $range = $info{$reg}{'fields'}{$invi}{'range'};
#            if ($m && $f) {
#                $retval2 .= "Bit $invi ($info{$reg}{'fields'}{$invi}{'mnemonic'} - $info{$reg}{'fields'}{$invi}{'function'}):\n";
#                $retval2 .= sprintf("%32s: %d\n",$tag1, $val1[$i]);
#                $retval2 .= sprintf("%32s: %d\n",$tag2, $val2[$i]);
#            } else {
#                $retval2 .= "Bit $invi:\n";
#                $retval2 .= sprintf("%32s: %d\n",$tag1, $val1[$i]);
#                $retval2 .= sprintf("%32s: %d\n",$tag2, $val2[$i]);
#            }
#        }
#    }

    $retval .= "\n";
    if ($retval2 ne '') {
        $retval .= "\n\n$retval2\n";
        my $n = $info{$reg}{'name'};
        my $d = $info{$reg}{'description'};
        $n ||= '';
        $d ||= '';
        my $old = $retval;
        $retval = '';
        $retval .= sprintf("%40s -> %s<br>\n","XOR",unpack("B*",$xor)) if ($DEBUG);
        $retval .= "\n$n\n" if ($n ne '');
        $retval .= "  $d" if ($d ne '');
        $retval .= $old;
        $retval .= "\n";
    }

    return "<pre>$retval</pre>";
}

sub load_datafile {
  my $file = 'bkdg.data';
  my $return = '';

  if (-f $file) {
      unless ($return = do $file) {
        warn "couldn't parse $file: $@" if $@;
        warn "couldn't do $file: $!"    unless defined $return;
        warn "couldn't run $file"       unless $return;
      }
  } else {
    print "Warning: data file '$file' not found - $0 will only report on differing bits without explanation.\n";
  }

}

sub main {
  my @filenames;
  my $version = 0;
  my %data;

  GetOptions ("filename=s" => \@filenames,  "version" => \$version);

  &version_information($NAME,$VERSION,$DATE,$COPYRIGHT,$AUTHOR,$LICENSE,$URL) if ($version);

  &usage_information() if ($#filenames < 1);

  &load_datafile();

  foreach my $file (@filenames) {
    print STDERR "processing $file\n";
    %data = &parse_file($file,%data);
  }

  print "<html>\n<body>\n";

    foreach  my $key (sort keys %data) {
        my $first = pack("H*",'00000000');
        my $firstfile = '';
        foreach my $k2 (reverse sort keys %{$data{$key}}) {
            if (unpack("H*",$first) eq '00000000') {
                $first = $data{$key}{$k2};
                $firstfile = $k2;
            }
            if (unpack("H*",$first) ne unpack("H*",$data{$key}{$k2})) {
                my $reg = ($key =~ /\s+([a-z0-9]+)$/i)[0];
                print "$key\n";
                if ($DEBUG) {
                    print "<pre>";
                    printf("%44s -> %s (%s)\n",$firstfile,unpack("B*",$first),unpack("H*",$first));
                    printf("%44s -> %s (%s)\n",$k2,unpack("B*",$data{$key}{$k2}),unpack("H*",$data{$key}{$k2}));
                    print "</pre>";
                }

                print &interpret_differences($reg,$firstfile,$first,$k2,$data{$key}{$k2});
            }
        }
    }
  print "</body>\n</html>\n";

}

