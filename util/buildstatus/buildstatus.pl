#owner:	Ron Minnich
#email:	rminnich@lanl.gov
#status: unstable
#explanation: 
#flash-types: 
#payload-types: 
#OS-types: 
#OS-issues:
#console-types:
#vga:
#last-known-good: 0/0/0000
#Comments: 
#Links:
#Mainboard-revision: 
#AKA: 
$mainboardcount = 0;

sub mainboard {
	my $sname = shift(@_);
	my $mainboard = $sname;
	$mainboard =~ s/.*src.mainboard.(.*).STATUS/$1/;
	open(STATUS, $sname) || die("Can' open $name: $!");
	$mainboardlist[$mainboardcount] = $mainboard;
	$mainboardcount++;
	LOOP: while(<STATUS>) {
		chop;
		next LOOP if (/^#.*/) ;
		($varname, $value) = split(/:/);
		# no lead spaces/tabs on value!
		$value =~ s/^[ \t]+//;
		# @ gets eaten up ...
		$value =~ s/@/AT_AT_SIGN/;
		$varname =~ s/-/_/g;
		$cmd = " \$$varname { '$mainboard' } = \"$value\"";
		#print "Command: $cmd\n";
		eval $cmd;
	}
}

sub cell {
  my $item = shift(@_);
  print "<td>$item</td>\n";
}
sub dumpstatus {
  print "<table BORDER COLS=8 WIDTH=\"100%\" NOSAVE>\n";
  print "<caption> <h2>LinuxBIOS Mainboard Status </h2></caption>\n";
  print "<tr>\n";
  cell("Name");
  cell("Revision");
  cell("AKA");
  cell("Status");
  cell("owner");
  cell("email");
  cell("Last Known Good");
  cell("Explanation");
  print "</tr>\n";
	foreach $i (0 .. $#mainboardlist) {
	  print "<tr>\n";
		$board = $mainboardlist[$i];
	        cell($board);
	  cell($Mainboard_revision{$board});
	        cell($AKA{$board});
	        cell($status{$board});
	        cell($owner{$board});
		$mail = $email{$board};
		$mail =~ s/AT_AT_SIGN/@/;
	  #cell($email{$board});
	  cell($mail);
	        cell($last_known_good{$board});
	        cell($explanation{$board});
	  #print "$board, $status{$board}, $owner{$board}\n";
	  print "<tr>\n";
	}
  print "</table>\n";
}

sub dumpbootinfo {
  print "<table BORDER COLS=6 WIDTH=\"100%\" NOSAVE>\n";
  print "<caption> <h2>LinuxBIOS Boot information </h2></caption>\n";
  print "<tr>\n";
  cell("Name");
  cell("Flash types");
  cell("Payload Types");
  cell("OS types");
  cell("OS issues");
  cell("Comments");

  print "</tr>\n";
	foreach $i (0 .. $#mainboardlist) {
	  print "<tr>\n";
		$board = $mainboardlist[$i];
	        cell($board);
	  cell($flash_types{$board});
	        cell($payload_types{$board});
	        cell($OS_types{$board});
	        cell($OS_issues{$board});
	        cell($Comments{$board});
	  print "<tr>\n";
	}
  print "</table>\n";
}

sub dumpconsoleinfo{
  print "<table BORDER COLS=3 WIDTH=\"100%\" NOSAVE>\n";
  print "<caption> <h2>LinuxBIOS Console Information </h2></caption>\n";
  print "<tr>\n";
  cell("Name");
  cell("Console types");
  cell("VGA capability");
  print "</tr>\n";
	foreach $i (0 .. $#mainboardlist) {
	  print "<tr>\n";
		$board = $mainboardlist[$i];
	        cell($board);
	  cell($console_types{$board});
	        cell($vga{$board});
	  print "<tr>\n";
	}
  print "</table>\n";
}

sub dumplinks {
  print "<table BORDER COLS=2 WIDTH=\"100%\" NOSAVE>\n";
  print "<caption> <h2>LinuxBIOS Links and Documentation </h2></caption>\n";
  print "<tr>\n";
  cell("Name");
  cell("Links");
  print "</tr>\n";
	foreach $i (0 .. $#mainboardlist) {
	  print "<tr>\n";
		$board = $mainboardlist[$i];
	        cell($board);
	  cell($Links{$board});
	  print "<tr>\n";
	}
  print "</table>\n";
}

# make it look like C argv[]
unshift(@ARGV, "XX");

$prefix=".";
if ($#ARGV == 1) { $prefix = $ARGV[1]; }

open(DIRS, "ls $prefix/src/mainboard/*/*/STATUS|") || die("open ls failed: $!");
while(<DIRS>) {
	chop;
	printf(STDERR "$_ \n");
	mainboard($_);
}
dumpstatus();
dumpbootinfo();
dumpconsoleinfo();
dumplinks();
