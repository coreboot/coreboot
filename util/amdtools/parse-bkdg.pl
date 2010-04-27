#!/usr/bin/perl -w

my $NAME = $0;
my $VERSION = '0.01';
my $DATE = '2009-09-04';
my $AUTHOR = "Ward Vandewege <ward\@jhvc.com>";
my $COPYRIGHT = "2009";
my $LICENSE = "GPL v3 - http://www.fsf.org/licenses/gpl.txt";
my $URL = "http://coreboot.org";

my $DEBUG = 0;

use strict;

# Run the bkdg for k8 through pdftotext first (from the poppler package)

my @registers = ();
my $raw_register = '';

my $name = '';
my $description = '';
my $step = 0;
my $oldstep = 0;

my $previous_res = 0;
my $previous_start = 0;
my $previous_stop = 0;
my $strip_empties = 0;

my $previous_bits = '';

our %info;

my %typos;

$typos{'CkeDreStrength'} = 'CkeDrvStrength';

while (<>) {
    my $line = $_;
    chomp($line);

    foreach my $k (keys %typos) {
        $line =~ s/$k/$typos{$k}/;
    }

    # Make sure we do not include headers in our output
    if (($line =~ /^Chapter 4/) || ($line =~ /Chapter 4$/)) {
        $oldstep = $step;
        $step = 99;
        next;
    }
    if ($step == 99) {  # header
        if ($line =~ /Processors$/) {
            $step = $oldstep;
            $strip_empties = 1;
        }
        next;
    }

    if ($strip_empties) {
        # Headers are followed by two blank lines. Strip them.
        if ($line =~ /^\s*$/) {
            next;
        } else {
            $strip_empties = 0;
        }
    }


    if (($step % 6 == 0) && ($line =~ /^\d+\.\d+\.\d+\s+(.*)$/)) {
        $step = 1;
        next;
    }
    if ($step == 1) {
        $description = "$line\n";
        $step = 2;
        next;
    }
    #print STDERR "STEP: $step\n";
    #print STDERR "$line\n";

    if ((($step == 0) || ($step == 6) || ($step == 2)) && ($line =~ /^(.*)\s+Function\s+\d+:\s+Offset\s+..h$/)) {
        $name = $1;
        $name =~ s/ +$//;
        $step = 3;
        $description =~ s/\n+$//ms;

        if ($previous_bits ne '') {
          &finish_record($previous_bits);
          $previous_bits = ''; # reset previous_bits (used in step 6)
        }


        next;
    } elsif ($step == 2) {
        $description .= "$line\n";
        next;
    }

    if (($step == 3) && ($line =~ /^\s+Index (.+h)$/)) {
        $raw_register= $1;
        @registers = split(/,/,$raw_register);
        for (my $i=0;$i<=$#registers;$i++) {
            $registers[$i] =~ s/ //g;
            $registers[$i] =~ s/h$//;
        }
        # OK, we have our register(s), so now we can print out the name and description lines.
        print "\$info{'$registers[0]'}{'name'} = \"$name\";\n";
        print "\$info{'$registers[0]'}{'description'} = \"$description\";\n";
        $step = 4;
        next;
    }

    if (($step == 4) && ($line =~ /^Bits\s+Mnemonic\s+Function\s+R\/W\s+Reset$/)) {
        $step = 5;
        next;
    }

    if (($step == 5) && (!($line =~ /^Field Descriptions$/))) {
        $line =~ s/^ +//; # Strip leading spaces
        my @f = split(/  +/,$line);

        # skip blank lines
        next if (!exists($f[0]));

        # skip headers (they could be repeated if the table crosses a page boundary
        next if ($f[0] eq 'Bits');

        # Clean up funky field separator
        if ($f[0] =~ /\d+.+\d+/) {
            $f[0] =~ s/[^\d]+/-/g;
        }

        my ($start, $stop, $width) = (0,0,0);
        if ($f[0] =~ /-/) {
            $f[0] =~ s/^(\d+)[^\d]+(\d+)$/$1-$2/;
            ($stop,$start) = ($1,$2);
            $width = $stop-$start+1;
        } else {
          if ($f[0] =~ /^\d+$/) {
            $start = $stop = $f[0];
            $width = 1;
          } else {
            # continuation from previous line
            $start = $stop = $width = 0;
          }
        }

        # Some lines have only bit entries
        if (($#f < 1) && ($f[0] =~ /^\d+(|\-\d+)/)) {
          $f[4] = '';
          $f[3] = '';
          $f[2] = '';
          $f[1] = '';
        } elsif ($#f < 1) {
          # Some lines are a continuation of the function field a line above
          $f[4] = '';
          $f[3] = '';
          $f[2] = $f[0];
          $f[1] = '';
          $f[0] = '';
          my $tmp = "\$info{'$registers[0]'}{'ranges'}{" . $previous_res . "}{'function'} .= \"" . $f[2] . "\";\n";
          print &multiply($tmp,$previous_res,$previous_start,$previous_stop);
          next;
        }

        # Some lines have only bit and reset entries
        if ($#f < 2) {
          $f[4] = $f[1];
          $f[3] = '';
          $f[2] = '';
          $f[1] = '';
        }

        # Some lines have no mnemonic and no function
        if ($#f < 3) {
          $f[4] = $f[2];
          $f[3] = $f[1];
          $f[2] = '';
          $f[1] = '';
        }

        # functions with 'reserved' mnemonic have no function
        if ($f[1] =~ /^reserved$/i) {
            $f[4] = $f[3];
            $f[3] = $f[2];
            $f[2] = '';
        }

        $previous_res = $f[0];
        $previous_start = $start;
        $previous_stop = $stop;

        # the 'range' field is not useful in this instance, but used in the 'fields' version of this block to easily go
        # from a bit position to the corresponding range.
        my $str = "
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'function'} = \"" . $f[2] . "\";
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'mnemonic'} = \"" . $f[1] . "\";
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'description'} = \"" . "\";
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'begin'} = $start;
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'end'} = $stop;
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'width'} = $width;
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'rw'} = \"" . $f[3] . "\";
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'reset'} = \"" . $f[4] . "\";
\$info{'$registers[0]'}{'ranges'}{'" . $f[0] . "'}{'range'} = \"" . $f[0] . "\";
";
        my $output;

        $output = &multiply($str,$f[0],$start,$stop);

        # Load the data structure here, too
        eval($output);

        print $output . "\n\n";
    } elsif (($step == 5) && ($line =~ /^Field Descriptions$/)) {
        $step = 6;
        next;
    }

    if ($step == 6) {
        if ($line =~ /^(.*?)\((.*?)\).+Bit(s|) +(.*?)\. (.*)$/) {
          my $bits = $4;
          my $desc = $5;
          $bits =~ s/[^\d]+/-/;

          if ($previous_bits ne '') {
            # We're done with a field description block
            print "\$info{'$registers[0]'}{'ranges'}{'$previous_bits'}{'description'} = \"" . $info{$registers[0]}{'ranges'}{$previous_bits}{'description'} . "\";\n";
            foreach my $k (keys %{$info{$registers[0]}{'ranges'}{$previous_bits}{'values'}}) {
              print "\$info{'$registers[0]'}{'ranges'}{'$previous_bits'}{'values'}{'$k'} = \"" . $info{$registers[0]}{'ranges'}{$previous_bits}{'values'}{$k} . "\";\n";
            }
          }

          if (exists($info{$registers[0]}{'ranges'}{$bits})) {
            print STDERR "match ($bits) on $line\n";
            $info{$registers[0]}{'ranges'}{$bits}{'description'} = $desc . "\n";
            $previous_bits = $bits;
          }
        } elsif ($previous_bits ne '') {
          $info{$registers[0]}{'ranges'}{$previous_bits}{'description'} .= $line . "\n";
          if ($line =~ /([0-9a-f]+b|[0-9a-f]+h) = (.*)$/i) {
            $info{$registers[0]}{'ranges'}{$previous_bits}{'values'}{$1} = $2;
          }
        }
    }

}
&finish_record($previous_bits);


print "1;\n";

sub multiply {
  my $str = shift;
  my $range = shift;
  my $start = shift;
  my $stop = shift;
  my $output = '';
  for (my $i=$start;$i<=$stop;$i++) {
    my $tmp = $str;
    $tmp =~ s/\{'$range'\}/{'$i'}/g;
    $tmp =~ s/\{'ranges'\}/{'fields'}/g;
    $tmp .=
    $output .= $tmp;
  }

  #$output .= $str if (($stop - $start + 1) > 1);
  $output .= $str;

  return $output;
}

sub finish_record {
  my $previous_bits = shift;
   # We're done with a field description block
          print "\$info{'$registers[0]'}{'ranges'}{'$previous_bits'}{'description'} = \"" . $info{$registers[0]}{'ranges'}{$previous_bits}{'description'} . "\";\n";
          foreach my $k (keys %{$info{$registers[0]}{'ranges'}{$previous_bits}{'values'}}) {
            print "\$info{'$registers[0]'}{'ranges'}{'$previous_bits'}{'values'}{'$k'} = \"" . $info{$registers[0]}{'ranges'}{$previous_bits}{'values'}{$k} . "\";\n";
          }

          # End of table. If this data applies to more than one register, print duplication lines.
          for (my $i=1;$i<=$#registers;$i++) {
            print "\$info{'$registers[$i]'} = \$info{'$registers[0]'};\n";
          }

}
