#!/usr/bin/env perl
#
# SPDX-License-Identifier: GPL-2.0-only

package gerrit_stats;

# To install any needed modules install the cpanm app, and use it to install the required modules:
#  sudo cpan App::cpanminus
#  sudo /usr/local/bin/cpanm JSON::Util Net::OpenSSH DateTime Devel::Size

use strict;
use warnings;
use English qw( -no_match_vars );
use File::Find;
use File::Path;
use Getopt::Long;
use Getopt::Std;
use JSON::Util;
use Net::OpenSSH;
use Data::Dumper qw(Dumper);
use DateTime;
use Devel::Size qw(size total_size);

my $old_version;
my $new_version;
my $infodir="$ENV{'HOME'}/.commit_info/" . `git config -l | grep remote.origin.url | sed 's|.*@||' | sed 's|:.*||'`;
chomp($infodir);
my $URL_WITH_USER;
my $SKIP_GERRIT_CHECK;
my $print_commit_list = 1;

#disable print buffering
$OUTPUT_AUTOFLUSH = 1;
binmode STDOUT, ":utf8";

Main();

#-------------------------------------------------------------------------------
# Main
#-------------------------------------------------------------------------------
sub Main {
    check_arguments();

    my %submitters = ();
    my %authors = ();
    my %owners = ();
    my %reviewers = ();
    my %author_added = ();
    my %author_removed = ();
    my $total_added = 0;
    my $total_removed = 0;
    my $number_of_commits = 0;
    my $number_of_submitters = 0;
    my $submit_epoch = "";
    my $first_submit_epoch = "";
    if (!$URL_WITH_USER) {
        get_user()
    }

     # make sure the versions exist
    check_versions();

    #fetch patches if needed.  Get ids of first and last commits
    my @commits = `git log --pretty=%h "$old_version..$new_version" 2>/dev/null`;
    get_commits(@commits);
    my $last_commit_id = $commits[0];
    my $first_commit_id = $commits[@commits - 1];
    chomp $last_commit_id;
    chomp $first_commit_id;

    print "Statistics from commit $first_commit_id to commit $last_commit_id\n";
    print "Patch, Date, Owner, Author, Submitter, Inserted lines, Deleted lines, Subject, Reviewers\n";

    #loop through all commits
    for my $commit_id (@commits) {
        $commit_id =~ s/^\s+|\s+$//g;

        my $submitter = "";
        my %patch_reviewers = ();
        my $info;
        my $owner;
        my $author;
        my $author_email;
        my $inserted_lines = 0;
        my $deleted_lines = 0;
        my $subject;

        $number_of_commits++;
        print "\"$commit_id\", ";

        #read the data file for the current commit
        if (-f  "$infodir/$commit_id" && -s "$infodir/$commit_id" > 20) {
            open( my $HANDLE, "<", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
            $info = <$HANDLE>;
            close $HANDLE;

            my $commit_info = JSON::Util->decode($info);

            #get the easy data
            $owner = $commit_info->{'owner'}{'name'};
            if (! $owner) {
                $owner = $commit_info->{'owner'}{'username'};
            }
            if (! $owner) {
                $owner = "";
            }
            $author = $commit_info->{'currentPatchSet'}{'author'}{'name'};
            $author_email = $commit_info->{'currentPatchSet'}{'author'}{'email'};
            if (! $author) {
                $author = $commit_info->{'currentPatchSet'}{'author'}{'username'};
            }

            $inserted_lines = $commit_info->{'currentPatchSet'}{'sizeInsertions'};
            $deleted_lines = $commit_info->{'currentPatchSet'}{'sizeDeletions'};
            $subject = $commit_info->{'subject'};

            #get the patch's submitter
            my $approvals = $commit_info->{'currentPatchSet'}{'approvals'};
            for my $approval (@$approvals) {
                if ($approval->{'type'} eq "SUBM") {
                    $submit_epoch = $approval->{'grantedOn'};
                    $submitter = $approval->{'by'}{'name'};
                }
            }

            #get all the reviewers for all patch revisions
            my $patchsets = $commit_info->{'patchSets'};
            for my $patch (@$patchsets) {
                if (! $author) {
                    $author = $patch->{'author'}{'name'};
                }
                my $approvals = $patch->{'approvals'};
                for my $approval (@$approvals) {

                    if ( (! $submitter) && ($approval->{'type'} eq "SUBM")) {
                        $submit_epoch = $approval->{'grantedOn'};
                        $submitter = $approval->{'by'}{'name'};
                    }

                    if ($approval->{'type'} eq "Code-Review") {
                        my $patch_reviewer = $approval->{'by'}{'name'};
                        if ($patch_reviewer) {
                            if (exists $patch_reviewers{$patch_reviewer}) {
                                $patch_reviewers{$patch_reviewer}++;
                            } else {
                                $patch_reviewers{$patch_reviewer} = 1;
                            }
                        }
                    }
                }
            }

        } else {
            # get the info from git
            my $logline = `git log --pretty="%ct@@@%s@@@%an@@@%aE@@@%cn" $commit_id^..$commit_id --`;
            $logline =~ m/^(.*)@@@(.*)@@@(.*)@@@(.*)@@@(.*)\n/;
            ($submit_epoch, $subject, $author, $author_email, $submitter) = ($1, $2, $3, $4, $5);
            $owner = $author;
            $logline = `git log --pretty= --shortstat $commit_id^..$commit_id --`;
            if ($logline =~ m/\s+(\d+)\s+insertion/) {
                $inserted_lines = $1;
            }
            if ($logline =~ m/\s+(\d+)\s+deletion/) {
                $deleted_lines = $1 * -1;
            }
            my @loglines = `git log $commit_id^..$commit_id -- | grep '\\sReviewed-by:'`;
            for my $line (@loglines){
                if ($line =~ m/.*:\s+(.*)\s</) {
                    my $patch_reviewer = $1;
                    if ($patch_reviewer) {
                        if (exists $patch_reviewers{$patch_reviewer}) {
                            $patch_reviewers{$patch_reviewer}++;
                        } else {
                            $patch_reviewers{$patch_reviewer} = 1;
                        }
                    }
                }
            }

        }

        # Not entirely certain why this is needed, but for a number of patches have been submitted
        # the submit time in gerrit is set to April 9, 2015.
        if ($submit_epoch == 1428586219){
            my $logline = `git log --pretty="%ct" $commit_id^..$commit_id --`;
            $logline =~ m/^(.*)\n/;
            $submit_epoch = $1;
        }

        #add the count and owner to the submitter hash
        if (exists $submitters{$submitter}) {
            $submitters{$submitter}++;
        } else {
            $submitters{$submitter} = 1;
            $number_of_submitters++;
        }

        #create a readable date
        my $dt = DateTime->from_epoch(epoch => $submit_epoch);
        $dt->set_time_zone( 'Europe/Paris' );
        my $submit_time = $dt->strftime('%Y/%m/%d %H:%M:%S');
        if (!$first_submit_epoch) {
            $first_submit_epoch = $submit_epoch;
        }

        #create the list of reviewers to print
        my $reviewerlist = "";
        foreach my $reviewer (keys %patch_reviewers) {
            if ($reviewerlist eq "") {
                $reviewerlist = $reviewer;
            } else {
                $reviewerlist .= ", $reviewer";
            }

            if (exists $reviewers{$reviewer}) {
                $reviewers{$reviewer}++;
            } else {
                $reviewers{$reviewer} = 1;
            }
        }
        if (! $reviewerlist) {
            $reviewerlist = "-"
        }

        if ($print_commit_list) {
            print "$submit_time, $owner, $author, $submitter, $inserted_lines, $deleted_lines, \"$subject\", \"$reviewerlist\"\n";
        } else {
            print "$number_of_commits\n";
        }
        $total_added += $inserted_lines;
        $total_removed += $deleted_lines;
        if (exists $owners{$owner}) {
            $owners{$owner}++;
        } else {
            $owners{$owner} = 1;
        }

        if (exists $authors{$author}{"num"}) {
            $authors{$author}{"num"}++;
            $author_added{$author} += $inserted_lines;
            $author_removed{$author} += $deleted_lines;
            $authors{$author}{"earliest_commit"}=$submit_time;
        } else {
            $authors{$author}{"num"} = 1;
            $authors{$author}{"latest_commit"}=$submit_time;
            $authors{$author}{"earliest_commit"}=$submit_time;
            $author_added{$author} = $inserted_lines;
            $author_removed{$author} = $deleted_lines;
        }
        if (! exists $authors{$author}{email} && $author_email) {
            $authors{$author}{email} = "$author_email";
        }
    }
    my $Days = ($first_submit_epoch - $submit_epoch) / 86400;
    if (($first_submit_epoch - $submit_epoch) % 86400) {
        $Days += 1;
    }

    print "- Total Commits: $number_of_commits\n";
    printf "- Average Commits per day: %.2f\n", $number_of_commits / $Days;
    print "- Total lines added: $total_added\n";
    print "- Total lines removed: $total_removed\n";
    print "- Total difference: " . ($total_added + $total_removed) . "\n\n";

    print "=== Authors - Number of commits ===\n";
    my $number_of_authors = 0;
    foreach my $author (sort { $authors{$b}{num} <=> $authors{$a}{num} } (keys %authors) ) {
	if (! exists $authors{$author}{"email"}) {
		$authors{$author}{"email"} = "-";
	}
        printf "%-25s %5d %-40s (%2.2f%%) {%s / %s}\n",$author, $authors{$author}{"num"}, $authors{$author}{"email"}, $authors{$author}{"num"} / $number_of_commits * 100, $authors{$author}{"latest_commit"}, $authors{$author}{"earliest_commit"};
        $number_of_authors++;
    }
    print "Total Authors: $number_of_authors\n\n";

    print "=== Authors - Lines added ===\n";
    foreach my $author (sort { $author_added{$b} <=> $author_added{$a} } (keys %author_added) ) {
        if ($author_added{$author}) {
            printf "%-25s %5d (%2.3f%%)\n",$author, $author_added{$author}, $author_added{$author} / $total_added * 100;
        }
    }
    print "\n";

    print "=== Authors - Lines removed ===\n";
    foreach my $author (sort { $author_removed{$a} <=> $author_removed{$b} } (keys %author_removed) ) {
        if ($author_removed{$author}) {
            printf "%-25s %5d (%2.3f%%)\n",$author,$author_removed{$author} * -1, $author_removed{$author} / $total_removed * 100;
        }
    }
    print "\n";

    print "=== Reviewers - Number of patches reviewed ===\n";
    my $number_of_reviewers = 0;
    foreach my $reviewer (sort { $reviewers{$b} <=> $reviewers{$a} } (keys %reviewers) ) {
        printf "%-25s %5d (%2.3f%%)\n",$reviewer, $reviewers{$reviewer}, $reviewers{$reviewer} / $number_of_commits * 100;
        $number_of_reviewers++;
    }
    print "Total Reviewers: $number_of_reviewers\n\n";

    print "=== Submitters - Number of patches submitted ===\n";
    foreach my $submitter (sort { $submitters{$b} <=> $submitters{$a} } (keys %submitters) ) {
        printf "%-25s %5d (%2.3f%%)\n",$submitter, $submitters{$submitter}, $submitters{$submitter} / $number_of_commits * 100;
    }
    print "Total Submitters: $number_of_submitters\n\n";

    print "Commits, Ave, Added, Removed, Diff, Authors, Reviewers, Submitters\n";
    printf "$number_of_commits, %.2f, $total_added, $total_removed, " . ($total_added + $total_removed) . ", $number_of_authors, $number_of_reviewers, $number_of_submitters\n", $number_of_commits / $Days;
}

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sub check_versions {
    `git cat-file -e $old_version^{commit} 2>/dev/null`;
    if (${^CHILD_ERROR_NATIVE}){
        print "Error: Old version ($old_version) does not exist.\n";
        exit 1;
    }

    `git cat-file -e $new_version^{commit} 2>/dev/null`;
    if (${^CHILD_ERROR_NATIVE}){
        print "Error: New version ($new_version) does not exist.\n";
        exit 1;
    }
}

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sub get_user {
    my $url=`git config -l | grep remote.origin.url`;

    if ($url =~ /.*url=ssh:\/\/(\w+@[a-zA-Z][a-zA-Z0-9\.]+:\d+)/)
    {
        $URL_WITH_USER = $1;
    } else {
        print "Error: Could not get a ssh url with a username from gitconfig.\n";
        print "       use the -u option to set a url.\n";
        exit 1;
    }
}

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sub get_commits {
    my @commits = @_;
    my $submit_time = "";
    if (defined $SKIP_GERRIT_CHECK) {
        return;
    }
    my $ssh = Net::OpenSSH->new("$URL_WITH_USER", );
    $ssh->error and die "Couldn't establish SSH connection to $URL_WITH_USER:". $ssh->error;

    print "Using URL: ssh://$URL_WITH_USER\n";

    if (! -d $infodir) {
        mkpath($infodir)
    }

    for my $commit_id (@commits) {
        $commit_id =~ s/^\s+|\s+$//g;
        $submit_time = "";
            my $gerrit_review;

        # Quit if we've reeached the last coreboot commit supporting these queries
        if ($commit_id =~ /^7309709/) {
            last;
        }

        if (-f "$infodir/$commit_id") {
            $gerrit_review = 1;
        } else {
            $gerrit_review = `git log $commit_id^..$commit_id | grep '\\sReviewed-on:\\s'`;
        }

        if ($gerrit_review && $commit_id && (! -f "$infodir/$commit_id") ) {
            print "Downloading $commit_id";
            my @info = $ssh->capture("gerrit query --format=JSON --comments --files --current-patch-set --all-approvals --submit-records --dependencies commit:$commit_id");
            $ssh->error and die "remote ls command failed: " . $ssh->error;

            my $commit_info = JSON::Util->decode($info[0]);
            my $rowcount = $commit_info->{'rowCount'};
            if (defined $rowcount && ($rowcount eq "0")) {
                print " - no gerrit commit for that id.\n";
                open( my $HANDLE, ">", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
                print $HANDLE "No gerrit commit";
                close $HANDLE;
                next;
            }
            my $approvals = $commit_info->{'currentPatchSet'}{'approvals'};

            for my $approval (@$approvals) {
                if ($approval->{'type'} eq "SUBM") {
                    $submit_time = $approval->{'grantedOn'}
                }
            }
            my $dt="";
            if ($submit_time) {
                $dt = DateTime->from_epoch(epoch => $submit_time);
            } else {
                print " - no submit time for that id.\n";
                open( my $HANDLE, ">", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
                print $HANDLE "No submit time";
                close $HANDLE;

                next;
            }

            open( my $HANDLE, ">", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
            print $HANDLE $info[0];
            close $HANDLE;

            $dt->set_time_zone( 'Europe/Paris' );
            print " - submit time: " . $dt->strftime('%Y/%m/%d %H:%M:%S') . "\n";
        } elsif ($commit_id && (! -f "$infodir/$commit_id")) {
            print "No gerrit commit for $commit_id\n";
            open( my $HANDLE, ">", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
            print $HANDLE "No gerrit commit";
            close $HANDLE;
        }
    }
    print "\n";
}

#-------------------------------------------------------------------------------
# check_arguments parse the command line arguments
#-------------------------------------------------------------------------------
sub check_arguments {
    my $show_usage = 0;
    GetOptions(
        'help|?'         => sub { usage() },
        'url|u=s'        => \$URL_WITH_USER,
        'skip|s'         => \$SKIP_GERRIT_CHECK,
    );
    # strip ssh:// from url if passed in.
    if (defined $URL_WITH_USER) {
        $URL_WITH_USER =~ s|ssh://||;
    }
    if (@ARGV) {
        ($old_version, $new_version) = @ARGV;
    } else {
        usage();
    }
}

#-------------------------------------------------------------------------------
# usage - Print the arguments for the user
#-------------------------------------------------------------------------------
sub usage {
    print "gerrit_stats <options> [Old version] [New version]\n";
    print "Old version should be a tag (4.1), a branch (origin/4.1), or a commit id\n";
    print "New version can be 'HEAD' a branch (origin/master) a tag (4.2), or a commit id\n";
    print " Options:\n";
    print "    u | url [url]           url with username.\n";
    print "Example: \"$0 -u Gaumless\@review.coreboot.org:29418 origin/4.1 4.2\"\n";
    exit(0);
}

1;
