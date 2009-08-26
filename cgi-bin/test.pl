#!/usr/bin/perl -w
use CGI;

print "Content-type: text/html\n\n";

my $highlight="highlight-2.6.8/src/highlight"; 
my @options=qw( -f -Sjava --inline-css -skwrite -l -a -j2 -z -t4 /home/albiz/dev/BLOAT/src/EDU/purdue/cs/bloat/cfg/FlowGraph.java --data-dir /var/www/localhost/htdocs/cgi-bin/); 
system($highlight, @options);
