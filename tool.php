<?php
echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"
\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">
<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">
<head>
<title>DSFinder JAR Analysis Tool</title>
<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />
<style type=\"text/css\">
#header { padding: 35px 30px 10px 30px; background-color:#202080;}
#header h1 { font-variant: small-caps; }
#sidebar { float: left; width: 15%; padding: 1% 1% 0 1%; margin: 1%; color:#5ed6ff; background-color:#202080; }
#main {float: right; width: 80%; padding: 0 0 0 0; margin: 0; }
a:link {color:#8aede7; background-color:inherit !important;}
a:visited {color:#33FFFF; background-color:inherit !important;}
body { background:#06005c; color:white; } 
</style>
</head><body>
<div id=\"header\"><h1>Results</h1></div>";

if ($_FILES['inputjar']['error'] == UPLOAD_ERR_OK) {
  echo "<div id=\"sidebar\">";
  echo "<p> <a href=\"staging.ds-finder.log\">Summary Results</a> </p>";
  echo "<p> <a href=\"staging.ds-finder.log-full\">Complete Logs</a> </p>";
  echo "<p> <a href=\"tool.html\"> Back </a> </p>";
  echo "</div>";
  echo "<div id=\"main\">";
  $dir = '/home/albiz/dev/staging/';
  $package = $_POST['package_name'];
  $file = $dir . basename($_FILES['inputjar']['name']);
  $tmpfile = $_FILES['inputjar']['tmp_name'];

  if (!move_uploaded_file($_FILES['inputjar']['tmp_name'], $file)) {
    echo "<pre>FAILED to move :", $tmpfile, ": to ", $file, "</pre>";
    exit(1);
  } else {
    echo "<pre>moved :", $tmpfile, ": to ", $file, "</pre>";
  }

  echo "<pre>";
  echo "Received ", basename($file), "\n";
  
  system("bash -- process.sh $file $package", $rc);
  //system('whoami', $rc);
  echo $rc;

  echo "</pre>";
  echo "</div>";
 // unzip the jar
 // locate entry point
 // determine packages
 // run benchmarks


} else {
  echo "<pre>:",$_POST['_submit_check'],":</pre>";
}

echo "</body></html>";
?>
