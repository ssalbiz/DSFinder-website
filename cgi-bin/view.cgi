#!/usr/bin/perl
use strict;
use CGI qw/:standard/;           # load standard CGI routines
use IPC::Open3;

my $query = new CGI;
my $class = $query->param('name');
my $name = $query->param('varname');
my $pkg_name = $query->param('pkg');
my $type = $query->param('type');

$class =~ s/\./\//g;
#inner classes will be declared in their parent class' .java files
$class =~ s/\$.*//;
$class = "$class.java";
#should read source directory prefix from file
my $benchmark_root="/home/albiz/dev/";
my %path_prefix= ("Bean", "$benchmark_root/soot/aspectjBenchmarkSuite/Bean/src/", 
               "DCM", "$benchmark_root/soot/aspectjBenchmarkSuite/DCM/src/", 
	       "LawOfDemeter", "$benchmark_root/soot/aspectjBenchmarkSuite/LawOfDemeter/src/", 
	       "NullCheck", "$benchmark_root/soot/aspectjBenchmarkSuite/NullCheck/src/", 
	       "ProdLine", "$benchmark_root/soot/aspectjBenchmarkSuite/ProdLine/src/", 
	       "StarJ-Pool", "$benchmark_root/soot/aspectjBenchmarkSuite/StarJ-Pool/src/", 
	       "Tetris", "$benchmark_root/soot/aspectjBenchmarkSuite/Tetris/src/", 
	       "aglets", "$benchmark_root/aglets/src/", 
	       "antlr-gunit", "$benchmark_root/antlr/antlr-3.1.3/gunit/src/main/java/", 
	       "aoi", "$benchmark_root/aoi/AoIsrc272/ArtOfIllusion/src/", 
	       "argoUML", "$benchmark_root/AUML/argoumlsrc/src/argouml-app/src/", 
	       "asm", "$benchmark_root/asm-3.2/src/", 
	       "axion", "$benchmark_root/axion/axion/src/", 
	       "azureus", "$benchmark_root/workspace/azureus2/", 
	       "bcel", "$benchmark_root/bcel-5.2/src/java/", 
	       "bloat", "$benchmark_root/BLOAT/src/", 
	       "cglib", "$benchmark_root/cglib/src/proxy/", 
	       "colt", "$benchmark_root/colt/src/", 
	       "columba", "$benchmark_root/columba/core-api/src/main/java/", 
	       "commons-cli", "$benchmark_root/commons-cli-1.2-src/src/java/", 
	       "commons-collections", "$benchmark_root/commons-collections-3.2.1-src/src/java/", 
	       "commons-lang", "$benchmark_root/commons-lang-2.4-src/src/java/", 
	       "commons-logging", "$benchmark_root/commons-logging-1.1.1-src/src/java/", 
	       "derby", "$benchmark_root/db-derby-10.5.1.1-src/java/engine/", 
	       "dom4j", "$benchmark_root/dom4j-1.6.1/src/java/", 
	       "drjava", "$benchmark_root/drjava/drjava/src/", 
	       "fit", "$benchmark_root/Fit/source/imp/java/src/", 
	       "fop", "$benchmark_root/FOP/fop-0.95/src/java/", 
	       "galleon", "$benchmark_root/galleon/galleon/src/", 
	       "gantt", "$benchmark_root/ganttproject-2.0.9/tmp/ganttproject-2.0/gantt/", 
	       "hibernate", "$benchmark_root/hibernate/src/", 
	       "hsqldb", "$benchmark_root/hsqldb/src/", 
	       "ireport", "$benchmark_root/iReport-3.0.0-src/src/", 
	       "jag", "$benchmark_root/jag-6.1/src/", 
	       "jasper", "$benchmark_root/jasper/jasperreports-3.5.1/src/", 
	       "javacc", "$benchmark_root/javaCC/javacc/src/", "jaxen", 
	       "$benchmark_root/jaxen-1.1.1/src/java/main/", "jchem", 
	       "$benchmark_root/cdk-1.0.x/src/", 
	       "jcm", "$benchmark_root/jcm/JCM/trunk/src/", 
	       "jedit", "$benchmark_root/jEdit/", 
	       "jeppers", "$benchmark_root/jeppers/src/", 
	       "jetty", "$benchmark_root/jetty-6.1.17/modules/jetty/src/main/java/", 
	       "jext", "$benchmark_root/jext/src/lib/", 
	       "jfreechart", "$benchmark_root/jfreechart-1.0.13/source/", 
	       "jgraph", "$benchmark_root/jgraphx/src/", 
	       "jmeter", "$benchmark_root/jakarta-jmeter-2.3.2/src/core/", 
	       "jre", "$benchmark_root/jre/", 
	       "jung", "$benchmark_root/jung-2.0/", 
	       "junit", "$benchmark_root/jUnit/src/main/java/", 
	       "jython", "$benchmark_root/jython/src/main/java/", 
	       "log4j", "$benchmark_root/apache-log4j-1.2.15/src/main/java/", 
	       "lucene", "$benchmark_root/lucene-2.4.1/src/java/", 
	       "megamek", "$benchmark_root/megamek/src/", 
	       "pmd", "$benchmark_root/pmd/pmd-4.2.5/src/", 
	       "poi", "$benchmark_root/poi-3.2-FINAL/src/java/", 
	       "proxool", "$benchmark_root/proxool-0.9.1/src/java/", 
	       "regexp", "$benchmark_root/regexp-1.5/src/java/", 
	       "sablecc", "$benchmark_root/sablecc/sablecc-3.2/src/", 
	       "sandmark", "$benchmark_root/sandmark/smark3/", 
	       "tomcat", "$benchmark_root/tomcat/java/", 
	       "xerces", "$benchmark_root/xerces-2_9_1/src/", 
	       "xstream", "$benchmark_root/xstream-1.3.1/xstream/src/java/");


if (not ($class =~ m/^[\w\/]+\.[j]ava$/)) { die ('bad name') }
my $fn = $path_prefix{$pkg_name}.$class;

#figure out the line number we need to highlight
#best guess: grep for type and name, hopefully it will be the field declaration, not a method parameter.
#Hopefully...

$type=~s/.*\.//;
$type=~s/.*\$//; #drop prefixes
$name=~s/\/.*//g; #drop everything after backslash in 'next/prev' entries
my $ln = `egrep -n "$type.*$name" $fn | grep -v "*" | egrep -v "implements|extends" | head -1`;
$ln =~ s/\:.*//;

print header;
print start_html($class); # start the HTML

local (*HIS_IN, *HIS_OUT, *HIS_ERR);
print "<a href=#l_$ln>Jump to field declaration</a>";
print "<pre>";
my $highlight = "highlight-2.6.8/src/highlight";
my @hl_args = qw(-f -Sjava --inline-css -skwrite -l -a -j2 -z -t4 --data-dir /var/www/localhost/htdocs/cgi-bin/);

system ($highlight, @hl_args, $fn) == 0 || print "File not Found\n";

print "</pre>";
print "
<script type='text/javascript'>
var anchors = document.getElementsByTagName('a');
anchors[".$ln."].nextSibling.setAttribute('style', 'background-color:yellow');
</script>
";

print end_html;                  # end the HTML
