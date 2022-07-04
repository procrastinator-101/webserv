#!/usr/bin/perl
# The first line should be where perl resides on the local system.

$company = "company-name\@cs.pitt.edu";
$webmaster = "webmaster-name\@cs.pitt.edu";
# You can have a list of e-mail addresses in the memberlist.
$memberlist = "member-name\@cs.pitt.edu";

 ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)
                                                  = localtime(time);
      $mon = $mon + 1;

if ($ENV{'REQUEST_METHOD'} eq 'POST') {

# Get the input

    read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

# Split the name-value pairs

    @pairs = split(/&/, $buffer);

# Load the FORM variables

    foreach $pair (@pairs) {
        ($name, $value) = split(/=/, $pair);
        $value =~ tr/+/ /;
        $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;

        $FORM{$name} = $value;
    }

# Open the email message to send the data to webmaster
				
    if($FORM{comm1} ne "") {
    open (MESSAGE, "| /usr/bin/mail  $webmaster ");
# The mail utility is in /usr/bin/mail so you need to change that if different.

# Format email header information

    print MESSAGE "To: $company\n";
    if ($FORM{email} ne "") {
        print MESSAGE "Reply-To: $FORM{email}\n";
    }

# Write the actual email message

    print MESSAGE "Subject: Message from $ENV{'REMOTE_HOST'}\n\n";
    print MESSAGE "Name: $FORM{name}\n";
    print MESSAGE "Address: $FORM{address}\n";
    print MESSAGE "Telephone: $FORM{tel}\n";
    print MESSAGE "Fax:         $FORM{fax}\n";
    print MESSAGE "Email:   $FORM{email}\n";
    print MESSAGE "Message to Host:     $FORM{comm1}\n"; 
    close (MESSAGE);
    }

# Open the email message to send the data to a list of members
				
    if($FORM{comm2} ne "") {
    open (MESSAGE, "| /usr/bin/mail $memberlist ");

# Format email header information

    print MESSAGE "To: $company\n";
    if ($FORM{email} ne "") {
        print MESSAGE "Reply-To: $FORM{email}\n";
    }

# Write the actual email message

    print MESSAGE "Subject: Message from $ENV{'REMOTE_HOST'}\n\n";
    print MESSAGE "Name: $FORM{name}\n";
    print MESSAGE "Address: $FORM{address}\n";
    print MESSAGE "Telephone: $FORM{tel}\n";
    print MESSAGE "Fax:         $FORM{fax}\n";
    print MESSAGE "Email:   $FORM{email}\n";
    print MESSAGE "Message to All:     $FORM{comm2}\n";
    close (MESSAGE);
# Also post this message
# You need to change the full_path_name.
    open (MESSAGE, "> /full_path_name/t1");
    print MESSAGE "<p>\n";
    print MESSAGE "<b> Message from $ENV{'REMOTE_HOST'}</b><br>";
    print MESSAGE "<b> Name</b>: $FORM{name}<br>";
    print MESSAGE "<b> Email</b>:   $FORM{email}<br>";
    print MESSAGE "<b> Time/Date</b>:$hour:$min:$sec / $mon-$mday-$year,<br>";
    print MESSAGE "<b>Posted Message</b>:     $FORM{comm2}<br>";
    close (MESSAGE);
# The bulletin bu.html and two temporary files t1 and t2, are in local directory.
# We use local files t1 and t2 so that a new message appears at the top of bulletin.
    $ans=`cat /full_path_name/t1 /full_path_name/bu.html > /full_path_name/t2`;
    $ans=`cp /full_path_name/t2 /full_path_name/bu.html`;
    }

#    Post message to the bulletin board only
				
    if( $FORM{comm3} ne "") {
    open (MESSAGE, "> /full_path_name/t1");
    print MESSAGE "<p>\n";
    print MESSAGE "<b> Message from $ENV{'REMOTE_HOST'}</b><br>";
    print MESSAGE "<b> Name</b>: $FORM{name}<br>";
    print MESSAGE "<b> Email</b>:   $FORM{email}<br>";
    print MESSAGE "<b> Time/Date</b>:$hour:$min:$sec / $mon-$mday-$year,<br>";
    print MESSAGE "<b>Posted Message</b>:     $FORM{comm3}<br>";
    close (MESSAGE);
    $ans=`cat /full_path_name/t1 /full_path_name/bu.html > /full_path_name/t2`;
    $ans=`cp /full_path_name/t2 /full_path_name/bu.html`;
    }

# Thank the user and acknowledge the order
    &thank_you;

} 

else { 	
# Format an error message for the user

    print "Content-type: text/html\r\n\r\n";
    print "<HTML>\n";
    print "<HEAD>\n";
    print "<TITLE>Message Form Error</TITLE>\n";
    print "</HEAD>\n";
    print "<BODY>\n";
    print "<H1>Message Form Error</H1>\n";
    print "<HR>\n";
    print "<P>\n";
    print "Form input was not processed.  Please mail your ";
    print "remarks to $company\n";
    print "</BODY>\n";
    print "</HTML>\n";
}
sub thank_you {

    print "Content-type: text/html\r\n\r\n";
    print "<HTML>\n";
    print "<HEAD>\n";
    print "<TITLE>Thank You</TITLE>\n";
    print "</HEAD>\n";
    print "<BODY>\n";
    print "<H1>Thank You</H1>\n";
    print "<HR>\n";
    print "<P>\n";
    print "Your message have been emailed to host, or emailed to other members, or posted on the bulletin board.\n";
    print "</BODY>\n";
    print "</HTML>\n";
}