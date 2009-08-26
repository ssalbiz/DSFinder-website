#Plugin Name: Highlight
#Plugin URI: http://www.andre-simon.de
#Description: Plugin which uses the highlight utility to generate coloured source code
#Author: André  Simon
#Version: 1.2
#Author URI: http://www.andre-simon.de/

package MT::Plugin::Highlight;

use strict;
use IPC::Open3;
use MT;
use MT::Entry;
use MT::Plugin;
use MT::Template::Context;
use MT::WeblogPublisher;

my $plugin = new MT::Plugin();
$plugin->name("MT Highlight");
$plugin->description('Source code highlighter using highlight (http://www.andre-simon.de)');
$plugin->doc_link('http://wiki.andre-simon.de/');
$plugin->author_name('Andre Simon');
$plugin->author_link('http://www.andre-simon.de/');
$plugin->plugin_link('http://wiki.andre-simon.de/');
$plugin->version('1.2');

MT->add_callback("BuildPage", 1, $plugin, \&highlight_callback);
MT->add_plugin($plugin);

sub highlight_callback
{
   my ($cb, %args) = @_;
   use Data::Dumper;
   my $html = ${$args{'Content'}};
   $html =~ s/<highlight([^>]*?)lang="([^"]+?)"([^>]*?)>(.*?)<\/highlight>/highlight_code($2,$4)/ges;
   ${$args{'Content'}} = $html;
   return 1;
}

sub highlight_code
{
   my ($lang, $source) = @_;

   $lang =~ s/[^a-zA-Z]//g;   #delete special chars in user input
   $source =~ s/\<br \/\>//g; #get rid of <br> Tags inserted by MT
   $source =~ s/\<\/?p\>//g;  # MT inserts <p> Tags if <, > are present in input code

   local(*HIS_IN, *HIS_OUT, *HIS_ERR);

   my @hl_args = ('-f', "-S$lang");
   push (@hl_args, '--inline-css'); # use inline css definitions
   push (@hl_args, '-skwrite'); # coloring theme
   push (@hl_args, '-l'); # linenumbers
   push (@hl_args, '-j2'); # linenumber length
   push (@hl_args, '-z'); # linenumber zeroes
   push (@hl_args, '-Fgnu'); # reformat
   push (@hl_args, '-t4'); # replace tabs
   my $childpid = IPC::Open3::open3(*HIS_IN, *HIS_OUT, *HIS_ERR, 'highlight', @hl_args);
   print HIS_IN $source;
   close(HIS_IN);            # Give end of file to kid.
   my @outlines = <HIS_OUT>; # Read till EOF.

   close HIS_OUT;
   close HIS_ERR;
   waitpid($childpid, 0);

   my $htext = join "", @outlines;

   return "<pre>".$htext."</pre>";
}



