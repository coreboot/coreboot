#!/usr/bin/env perl
#
# SPDX-License-Identifier: GPL-2.0-only

package gerrit_stats;

# To install any needed modules install the cpanm app, and use it to install the required modules:
#  sudo cpan App::cpanminus
#  sudo /usr/local/bin/cpanm JSON::Util Net::OpenSSH DateTime Devel::Size

# perltidy -l=200 -bt=2 -ce

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
my $infodir = "$ENV{'HOME'}/.local/commit_info/" . `git config -l | grep remote.origin.url | sed 's|.*@||' | sed 's|:.*||'`;
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

    my %submitters                = ();
    my %authors                   = ();
    my %owners                    = ();
    my %reviewers                 = ();
    my %commenters                = ();
    my %author_added              = ();
    my %author_removed            = ();
    my $total_added               = 0;
    my $total_removed             = 0;
    my $number_of_commits         = 0;
    my $number_of_submitters      = 0;
    my $submit_epoch              = "";
    my $first_submit_epoch        = "";
    my $patches_over_100_lines    = 0;
    my $total_lines_large_patches = 0;
    my %email_addresses           = (
        'Kacper Stojek' => 'kacper.stojek@3mdeb.com',
        'Damien Zammit' => 'damien@zamaudio.com',
        'Pavel Sayekat' => 'pavelsayekat@gmail.com',
        'Lance Zhao'    => 'lance.zhao@gmail.com',
    );

    my %aliases = (
        ' Felix Singer'                        => 'Felix Singer',
        'Abhay kumar'                          => 'Abhay Kumar',
        'AlexandruX Gagniuc'                   => 'Alexandru Gagniuc',
        'Anish K. Patel'                       => 'Anish K Patel',
        'Bao Zheng'                            => 'Zheng Bao',
        'Bernhard M. Wiedemann'                => 'Bernhard M. Wiedermann',
        'BjÃ¶rn Busse'                         => 'Bjarn Busse',
        'BryantOu'                             => 'Bryant Ou',
        'Chen Wisley'                          => 'Wisley Chen',
        'Cheng-Yi Chiang'                      => 'Jimmy Cheng-Yi Chiang',
        'Chris Ching (using chromium account)' => 'Chris Ching,',
        'ChromeOS Developer'                   => 'Dave Parker',
        'Cristi M'                             => 'Cristian Magherusan-Stanciu',
        'Cristian M?gheru?an-Stanciu'          => 'Cristian Magherusan-Stanciu',
        'Cristian MÄgheruÈan-Stanciu'          => 'Cristian Magherusan-Stanciu',
        'Cristian MÄgheruÈan-Stanciu'          => 'Cristian Magherusan-Stanciu',
        'DAWEI CHIEN'                          => 'Dawei Chien',
        'efdesign98'                           => 'Frank Vibrans',
        'Eugene D. Myers'                      => 'Eugene Myers',
        'Frank Vibrans III'                    => 'Frank Vibrans',
        'frank vibrans'                        => 'Frank Vibrans',
        'Frank.Vibrans'                        => 'Frank Vibrans',
        'FrankChu'                             => 'Frank Chu',
        'garmin chang'                         => 'Garmin Chang',
        'Garmin.Chang'                         => 'Garmin Chang',
        'hannahwilliams2'                      => 'Hannah Williams',
        'HAOUAS Elyes'                         => 'Elyes Haouas',
        'Harshapriya N'                        => 'Harsha Priya',
        'Hsuan-ting Chen'                      => 'Hsuan Ting Chen',
        'Iru Cai (vimacs)'                     => 'Iru Cai',
        'Jérémy Compostella'                   => 'Jeremy Compostella',
        'Jérémy Compostella'                   => 'Jeremy Compostella',
        'JG Poxu'                              => 'Po Xu',
        'JonathonHall-Purism'                  => 'Jonathon Hall',
        'Karthikeyan Ramasubramanian'          => 'Karthik Ramasubramanian',
        'Kerry She'                            => 'Kerry Sheh',
        'kewei.xu'                             => 'kewei xu',
        'Kumar, Gomathi'                       => 'Gomathi Kumar',
        'KyÃ¶sti MÃ¤lkki'                      => 'Kyösti Mälkki',
        'Kyösti Mälkki'                        => 'Kyösti Mälkki',
        'Marcello Sylvester Bauer'             => 'Marcello Sylvester Bauer',
        'Martin L Roth'                        => 'Martin Roth',
        'Martin Roth - Personal'               => 'Martin Roth',
        'Matt Ziegelbaum'                      => 'Matthew Ziegelbaum',
        'mengqi.zhang'                         => 'Mengqi Zhang',
        'mrnuke'                               => 'Alexandru Gagniuc',
        'Nina-CM Wu'                           => 'Nina Wu',
        'ot_zhenguo.li'                        => 'Zhenguo Li',
        'Patrick Georgi patrick.georgi'        => 'Patrick Georgi',
        'Patrick Georgi patrick'               => 'Patrick Georgi',
        'Pavlushka'                            => 'Pavel Sayekat',
        'Ravi Kumar Bokka'                     => 'Ravi Kumar',
        'Ravi kumar'                           => 'Ravi Kumar',
        'Ravi Sarawadi'                        => 'Ravishankar Sarawadi',
        'ravindr1'                             => 'Ravindra',
        'Ravindra N'                           => 'Ravindra',
        'Ricardo Ribalda Delgado'              => 'Ricardo Ribalda',
        'ron minnich'                          => 'Ron Minnich',
        'Ronald G. Minnich'                    => 'Ron Minnich',
        'samrab'                               => 'Sudheer Amrabadi',
        'SANTHOSH JANARDHANA HASSAN'           => 'Santhosh Janardhana Hassan',
        'semihalf-czapiga-jakub'               => 'Jakub Czapiga',
        'Seunghwan Kim'                        => 'SH Kim',
        'Sooi, Li Cheng'                       => 'Li Cheng Sooi',
        'Stefan Reinauerstepan'                => 'Stefan Reinauer',
        'stepan'                               => 'Stefan Reinauer',
        'Swift Geek (Sebastian Grzywna)'       => 'Sebastian "Swift Geek" Grzywna',
        'Sylvain "ythier" Hitier'              => 'Sylvain Hitier',
        'Thomas GstÃ¤dtner'                    => 'Thomas Gstaedtner',
        'UwePoeche'                            => 'Uwe Poeche',
        'UwePoeche'                            => 'Uwe Poeche',
        'Varshit Pandya'                       => 'Varshit B Pandya',
        'Wayne3 Wang'                          => 'Wayne Wang',
        'Wayne3_Wang'                          => 'Wayne Wang',
        'Xi Chen'                              => 'Xixi Chen',
        'Yu-Hsuan Hsu'                         => 'Yu-hsuan Hsu',
        'zbao'                                 => 'Zheng Bao',
        'Zheng Bao zheng.bao'                  => 'Zheng Bao',
        'zhiyong tao'                          => 'Zhiyong Tao',
        'Дмитрий Понаморев'                    => 'Dmitry Ponamorev',
    );

    if ( !$URL_WITH_USER ) {
        get_user();
    }

    print "Saving data to $infodir\n";

    # Make sure the versions exist
    check_versions();

    # Fetch patches if needed.  Get ids of first and last commits
    my @commits = `git log --pretty=%H "$old_version..$new_version" 2>/dev/null`;
    get_commits(@commits);
    my $last_commit_id  = $commits[0];
    my $first_commit_id = $commits[ @commits - 1 ];
    chomp $last_commit_id;
    chomp $first_commit_id;

    print "Statistics from commit $first_commit_id to commit $last_commit_id\n";
    print "Patch, Date, Owner, Author, Submitter, Inserted lines, Deleted lines, Subject, Reviewers, Commenters\n";

    # Loop through all commits
    for my $commit_id (@commits) {
        $commit_id =~ s/^\s+|\s+$//g;

        my $submitter        = "";
        my %patch_reviewers  = ();
        my %patch_commenters = ();
        my $info;
        my $owner;
        my $author;
        my $author_email;
        my $inserted_lines = 0;
        my $deleted_lines  = 0;
        my $subject;

        $number_of_commits++;
        print "\"$commit_id\", ";

        # Read the data file for the current commit
        if ( -f "$infodir/$commit_id" && -s "$infodir/$commit_id" > 20 ) {
            open( my $HANDLE, "<", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
            $info = <$HANDLE>;
            close $HANDLE;

            my $commit_info = JSON::Util->decode($info);

            # Get the easy data
            $owner = $commit_info->{'owner'}{'name'};
            if ( !$owner ) {
                $owner = $commit_info->{'owner'}{'username'};
            }
            if ( !$owner ) {
                $owner = "-";
            }
            if ( $owner && exists( $aliases{$owner} ) ) {
                $owner = $aliases{$owner};
            }

            $author = $commit_info->{'currentPatchSet'}{'author'}{'name'};
            if ( $author && exists( $aliases{$author} ) ) {
                $author = $aliases{$author};
            }
            if ( !$author ) {
                $author = $commit_info->{'currentPatchSet'}{'author'}{'username'};
            }
            $author_email   = $commit_info->{'currentPatchSet'}{'author'}{'email'};
            $inserted_lines = $commit_info->{'currentPatchSet'}{'sizeInsertions'};
            $deleted_lines  = $commit_info->{'currentPatchSet'}{'sizeDeletions'};
            $subject        = $commit_info->{'subject'};

            #get the patch's submitter
            my $approvals = $commit_info->{'currentPatchSet'}{'approvals'};
            for my $approval (@$approvals) {
                if ( $approval->{'type'} eq "SUBM" ) {
                    $submit_epoch = $approval->{'grantedOn'};
                    $submitter    = $approval->{'by'}{'name'};
                    if ( exists( $aliases{$submitter} ) ) {
                        $submitter = $aliases{$submitter};
                    }
                }
            }

            # Get all the commenters for all patch revisions
            my $comments = $commit_info->{'comments'};
            for my $comment (@$comments) {
                my $commenter;
                if ( $comment->{'reviewer'}{'username'} ) {
                    if ( $comment->{'reviewer'}{'username'} eq "jenkins" ) {
                        next;
                    }
                    if ( $comment->{'reviewer'}{'username'} eq "hardwaretestrobot" ) {
                        next;
                    }
                    if ( $comment->{'reviewer'}{'username'} eq "raptor-automated-test" ) {
                        next;
                    }
                }

                if ( $comment->{'reviewer'}{'name'} ) {
                    if ( $comment->{'reviewer'}{'name'} eq "Gerrit Code Review" ) {
                        next;
                    }
                }
                if ( $comment->{'message'} ) {
                    if ( $comment->{'message'} =~ "successfully cherry-picked" ) {
                        next;
                    }
                    if ( $comment->{'message'} =~ ": Code-Review" ) {
                        next;
                    }
                    if ( $comment->{'message'} =~ "Uploaded patch set" ) {
                        next;
                    }
                }

                if ( !$commenter ) {
                    $commenter = $comment->{'reviewer'}{'name'};
                    if ( $commenter && exists( $aliases{$commenter} ) ) {
                        $commenter = $aliases{$commenter};
                    }
                }
                if ( !$commenter ) {
                    $commenter = $comment->{'reviewer'}{'username'};
                    if ( $commenter && exists( $aliases{$commenter} ) ) {
                        $commenter = $aliases{$commenter};
                    }
                }
                if ( $commenter && $author && $commenter eq $author ) {
                    next;
                }
                if ($commenter) {
                    if ( $commenter && exists $patch_commenters{$commenter} ) {
                        $patch_commenters{$commenter}++;
                    } else {
                        $patch_commenters{$commenter} = 1;
                    }
                }
            }

            # Get all the reviewers for all patch revisions
            my $patchsets = $commit_info->{'patchSets'};
            for my $patch (@$patchsets) {
                if ( !$author ) {
                    $author = $patch->{'author'}{'name'};
                    if ( $author && exists( $aliases{$author} ) ) {
                        $author = $aliases{$author};
                    }
                }

                my $approvals = $patch->{'approvals'};
                for my $approval (@$approvals) {

                    if ( ( !$submitter ) && ( $approval->{'type'} eq "SUBM" ) ) {
                        $submit_epoch = $approval->{'grantedOn'};
                        $submitter    = $approval->{'by'}{'name'};
                        if ( $submitter && exists( $aliases{$submitter} ) ) {
                            $submitter = $aliases{$submitter};
                        }
                    }

                    if ( $approval->{'type'} eq "Code-Review" ) {
                        my $patch_reviewer = $approval->{'by'}{'name'};
                        if ($patch_reviewer) {
                            if ( exists $patch_reviewers{$patch_reviewer} ) {
                                $patch_reviewers{$patch_reviewer}++;
                            } else {
                                $patch_reviewers{$patch_reviewer} = 1;
                            }
                        }
                    }
                }
            }

        } else {

            # Get the info from git
            my $logline = `git log --pretty="%ct@@@%s@@@%an@@@%aE@@@%cn" $commit_id^..$commit_id --`;
            $logline =~ m/^(.*)@@@(.*)@@@(.*)@@@(.*)@@@(.*)\n/;
            ( $submit_epoch, $subject, $author, $author_email, $submitter ) = ( $1, $2, $3, $4, $5 );
            if ( exists( $aliases{$author} ) ) {
                $author = $aliases{$author};
            }
            $owner = $author;

            if ( $submitter && exists( $aliases{$submitter} ) ) {
                $submitter = $aliases{$submitter};
            }

            $logline = `git log --pretty= --shortstat $commit_id^..$commit_id --`;
            if ( $logline =~ m/\s+(\d+)\s+insertion/ ) {
                $inserted_lines = $1;
            }
            if ( $logline =~ m/\s+(\d+)\s+deletion/ ) {
                $deleted_lines = $1 * -1;
            }
            my @loglines = `git log $commit_id^..$commit_id -- | grep '\\sReviewed-by:'`;
            for my $line (@loglines) {
                if ( $line =~ m/.*:\s+(.*)\s</ ) {
                    my $patch_reviewer = $1;
                    if ( exists( $aliases{$patch_reviewer} ) ) {
                        $patch_reviewer = $aliases{$patch_reviewer};
                    }
                    if ($patch_reviewer) {
                        if ( exists $patch_reviewers{$patch_reviewer} ) {
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
        if ( $submit_epoch == 1428586219 ) {
            my $logline = `git log --pretty="%ct" $commit_id^..$commit_id --`;
            $logline =~ m/^(.*)\n/;
            $submit_epoch = $1;
        }

        # Add the count and owner to the submitter hash
        if ( $submitter && exists $submitters{$submitter} && exists $submitters{$submitter}{count} ) {
            $submitters{$submitter}{count}++;
        } else {
            $submitters{$submitter}{count} = 1;
            $number_of_submitters++;
            $submitters{$submitter}{"self"} = 0;
            $submitters{$submitter}{others} = 0;
            $submitters{$submitter}{name}   = $submitter;
        }

        if ( $submitter eq $author ) {
            $submitters{$submitter}{"self"}++;
        } else {
            $submitters{$submitter}{others}++;
        }

        # Create a readable date
        my $dt = DateTime->from_epoch( epoch => $submit_epoch );
        $dt->set_time_zone('Europe/Paris');
        my $submit_time = $dt->strftime('%Y/%m/%d');
        if ( !$first_submit_epoch ) {
            $first_submit_epoch = $submit_epoch;
        }

        # Create the list of commenters to print
        my $commenterlist = "";
        foreach my $commenter ( keys %patch_commenters ) {
            if ( $commenter && exists( $aliases{$commenter} ) ) {
                $commenter = $aliases{$commenter};
            }

            if ( $commenterlist eq "" ) {
                $commenterlist = $commenter;
            } else {
                $commenterlist .= ", $commenter";
            }

            if ( $commenter && exists $commenters{$commenter} ) {
                $commenters{$commenter}++;
            } else {
                $commenters{$commenter} = 1;
            }
        }
        if ( !$commenterlist ) {
            $commenterlist = "-";
        }

        # Create the list of reviewers to print
        my $reviewerlist = "";
        foreach my $reviewer ( keys %patch_reviewers ) {
            if ( exists( $aliases{$reviewer} ) ) {
                $reviewer = $aliases{$reviewer};
            }

            if ( $reviewerlist eq "" ) {
                $reviewerlist = $reviewer;
            } else {
                $reviewerlist .= ", $reviewer";
            }

            if ( $reviewer && exists $reviewers{$reviewer} ) {
                $reviewers{$reviewer}++;
            } else {
                $reviewers{$reviewer} = 1;
            }
        }
        if ( !$reviewerlist ) {
            $reviewerlist = "-";
        }

        if ($print_commit_list) {
            print "$submit_time, $owner, $author, $submitter, $inserted_lines, $deleted_lines, \"$subject\", \"$reviewerlist\" , \"$commenterlist\"\n";
        } else {
            print "$number_of_commits\n";
        }
        $total_added += $inserted_lines;
        if ( $inserted_lines - $deleted_lines > 100 ) {
            $patches_over_100_lines++;
            $total_lines_large_patches += $inserted_lines;
        }
        $total_removed += $deleted_lines;
        if ( exists $owners{$owner} ) {
            $owners{$owner}++;
        } else {
            $owners{$owner} = 1;
        }

        if ( $author && exists $authors{$author}{"num"} ) {
            $authors{$author}{"num"}++;
            $author_added{$author}   += $inserted_lines;
            $author_removed{$author} += $deleted_lines;
            $authors{$author}{"earliest_commit"} = $submit_time;
        } else {
            $authors{$author}{"num"}             = 1;
            $authors{$author}{"latest_commit"}   = $submit_time;
            $authors{$author}{"earliest_commit"} = $submit_time;
            $author_added{$author}               = $inserted_lines;
            $author_removed{$author}             = $deleted_lines;
        }
        if ( $author && ( !exists $authors{$author}{email} || $authors{$author}{email} eq "-" ) ) {
            if ($author_email) {
                $authors{$author}{email} = "$author_email";
            } elsif ( exists $email_addresses{$author} ) {
                $authors{$author}{email} = $email_addresses{$author};
            }
        }
    }
    my $Days = ( $first_submit_epoch - $submit_epoch ) / 86400;
    if ( ( $first_submit_epoch - $submit_epoch ) % 86400 ) {
        $Days += 1;
    }

    print "\n- Total Commits: $number_of_commits\n";
    printf "- Average Commits per day: %.2f\n", $number_of_commits / $Days;
    print "- Total lines added: $total_added\n";
    printf "- Average lines added per commit: %.2f\n", $total_added / $number_of_commits;
    print "- Number of patches adding more than 100 lines: $patches_over_100_lines\n";
    printf "- Average lines added per small commit: %.2f\n", ( $total_added - $total_lines_large_patches ) / ( $number_of_commits - $patches_over_100_lines );

    print "- Total lines removed: $total_removed\n";
    printf "- Average lines removed per commit: %.2f\n", $total_removed / $number_of_commits;
    print "- Total difference between added and removed: " . ( $total_added - $total_removed ) . "\n\n";

    print "=== Authors - Number of commits ===\n";
    printf "%-30s ,%5s ,%5s ,%6s ,%6s , %-52s ,%6s, %-19s , %s\n", "Author", "Ptchs", "Revws", "Cmnts", "Sbmts", "Email", "Prcnt", "Last commit", "Earliest_commit";

    my $number_of_authors = 0;
    foreach my $author ( sort { $authors{$b}{num} <=> $authors{$a}{num} } ( keys %authors ) ) {
        my $submissions = 0;
        if ( $author && exists $submitters{$author} ) {
            $submissions = $submitters{$author}{count};
        }
        my $review_count = 0;
        if ( $author && exists $reviewers{$author} ) {
            $review_count = $reviewers{$author};
        }

        my $comment_count = 0;
        if ( $author && exists $commenters{$author} ) {
            $comment_count = $commenters{$author};
        }

        if ( $author && !exists $authors{$author}{"email"} ) {
            $authors{$author}{"email"} = "-";
        }

        printf "%-30s ,%5d ,%5d ,%6d ,%6d , %-52s ,%5.2f%%, %s , %s\n", $author, $authors{$author}{"num"}, $review_count,
          $comment_count, $submissions, $authors{$author}{"email"}, $authors{$author}{"num"} / $number_of_commits * 100,
          $authors{$author}{"latest_commit"}, $authors{$author}{"earliest_commit"};
        $number_of_authors++;
    }
    print "Total Authors: $number_of_authors\n\n";

    print "=== Authors - Lines added ===\n";
    foreach my $author ( sort { $author_added{$b} <=> $author_added{$a} } ( keys %author_added ) ) {
        if ( $author_added{$author} ) {
            printf "%-30s, %10d, %2.3f%%\n", $author, $author_added{$author}, $author_added{$author} / $total_added * 100;
        }
    }
    print "\n";

    print "=== Authors - Lines removed ===\n";
    foreach my $author ( sort { $author_removed{$b} <=> $author_removed{$a} } ( keys %author_removed ) ) {
        if ( $author_removed{$author} ) {
            printf "%-30s, %10d, %6.3f%%\n", $author, $author_removed{$author} * -1, $author_removed{$author} / $total_removed * 100;
        }
    }
    print "\n";

    print "=== Reviewers - Number of patches reviewed ===\n";
    my $number_of_reviewers = 0;
    foreach my $reviewer ( sort { $reviewers{$b} <=> $reviewers{$a} } ( keys %reviewers ) ) {
        printf "%-30s, %6d, %6.3f%%\n", $reviewer, $reviewers{$reviewer}, $reviewers{$reviewer} / $number_of_commits * 100;
        $number_of_reviewers++;
    }
    print "Total Reviewers: $number_of_reviewers\n\n";

    print "=== Submitters - Number of patches submitted ===\n";
    printf "%-30s, %6s, %7s, %6s, %7s, %6s, %7s\n", "Name", "#", "total%", "Own", "own%", "Other", "other%";
    foreach my $submitter ( sort { $submitters{$b}{count} <=> $submitters{$a}{count} } ( keys %submitters ) ) {
        printf "%-30s, % 6d, %6.3f%%, %6d, %6.2f%%, %6d, %6.2f%%\n",
          $submitter,
          $submitters{$submitter}{count},
          $submitters{$submitter}{count} / $number_of_commits * 100,
          $submitters{$submitter}{"self"},
          $submitters{$submitter}{"self"} / $submitters{$submitter}{count} * 100,
          $submitters{$submitter}{others}, $submitters{$submitter}{others} / $submitters{$submitter}{count} * 100;
    }
    print "Total Submitters: $number_of_submitters\n\n";

    print "Commits, Ave, Added, Removed, Diff, Authors, Reviewers, Submitters\n";
    printf "$number_of_commits, %.2f, $total_added, $total_removed, " . ( $total_added + $total_removed ) . ", $number_of_authors, $number_of_reviewers, $number_of_submitters\n",
      $number_of_commits / $Days;
}

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sub check_versions {
    `git cat-file -e $old_version^{commit} 2>/dev/null`;
    if ( ${^CHILD_ERROR_NATIVE} ) {
        print "Error: Old version ($old_version) does not exist.\n";
        exit 1;
    }

    `git cat-file -e $new_version^{commit} 2>/dev/null`;
    if ( ${^CHILD_ERROR_NATIVE} ) {
        print "Error: New version ($new_version) does not exist.\n";
        exit 1;
    }
}

#-------------------------------------------------------------------------------
sub get_user {

    my $url = `git config -l | grep remote.origin.url`;

    if ( $url =~ /.*url=ssh:\/\/(\w+@[a-zA-Z][a-zA-Z0-9\.]+:\d+)(\/\w+)*/ ) {
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
    my @commits     = @_;
    my $submit_time = "";
    if ( defined $SKIP_GERRIT_CHECK && $SKIP_GERRIT_CHECK ) {
        return;
    }
    my $ssh = Net::OpenSSH->new( "$URL_WITH_USER", );
    $ssh->error and die "Couldn't establish SSH connection to $URL_WITH_USER:" . $ssh->error;

    print "Using URL: ssh://$URL_WITH_USER\n";

    if ( !-d $infodir ) {
        mkpath($infodir);
    }

    for my $commit_id (@commits) {
        $commit_id =~ s/^\s+|\s+$//g;
        $submit_time = "";
        my $gerrit_review;

        # Look for last coreboot commit
        if ( $commit_id eq "7309709742" ) {
            last;
        }

        if ( -f "$infodir/$commit_id" ) {
            $gerrit_review = 1;
        } else {
            $gerrit_review = `git log $commit_id^..$commit_id | grep '\\sReviewed-on:\\s'`;
        }

        if ( $gerrit_review && $commit_id && ( !-f "$infodir/$commit_id" ) ) {
            print "Downloading $commit_id";
            my @info = $ssh->capture("gerrit query --format=JSON --comments --files --current-patch-set --all-approvals --submit-records --dependencies commit:$commit_id");
            $ssh->error and die "remote ls command failed: " . $ssh->error;

            my $commit_info = JSON::Util->decode( $info[0] );
            my $rowcount    = $commit_info->{'rowCount'};
            if ( defined $rowcount && ( $rowcount eq "0" ) ) {
                print " - no gerrit commit for that id.\n";
                open( my $HANDLE, ">", "$infodir/$commit_id" ) or die "Error: could not open file '$infodir/$commit_id'\n";
                print $HANDLE "No gerrit commit";
                close $HANDLE;
                next;
            }
            my $approvals = $commit_info->{'currentPatchSet'}{'approvals'};

            for my $approval (@$approvals) {
                if ( $approval->{'type'} eq "SUBM" ) {
                    $submit_time = $approval->{'grantedOn'};
                }
            }
            my $dt = "";
            if ($submit_time) {
                $dt = DateTime->from_epoch( epoch => $submit_time );
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

            $dt->set_time_zone('Europe/Paris');
            print " - submit time: " . $dt->strftime('%Y/%m/%d %H:%M:%S') . "\n";
        } elsif ( $commit_id && ( !-f "$infodir/$commit_id" ) ) {
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
        'help|?'  => sub { usage() },
        'url|u=s' => \$URL_WITH_USER,
        'skip|s'  => \$SKIP_GERRIT_CHECK,
    );

    # strip ssh:// from url if passed in.
    if ( defined $URL_WITH_USER ) {
        $URL_WITH_USER =~ s|ssh://||;
    }
    if (@ARGV) {
        ( $old_version, $new_version ) = @ARGV;
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
