<?php
echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"
\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">
<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">
<head>
<title>DSFinder JAR Analysis Tool</title>
<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />
</head><body>";
if ($_FILES['inputjar']['error'] == UPLOAD_ERR_OK) {
  echo "<p> <a href=\"staging.ds-finder.log\">Summary Results</a> </p>";
  echo "<p> <a href=\"staging.ds-finder.log-full\">Complete Logs</a> </p>";
  echo "<p> <a href=\"tool.html\"> Back </a> </p>";
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
  echo "<p> <a href=\"staging.ds-finder.log\">Summary Results</a> </p>";
  echo "<p> <a href=\"staging.ds-finder.log-full\">Complete Logs</a> </p>";
  echo "<p> <a href=\"tool.html\"> Back </a> </p>";
 // unzip the jar
 // locate entry point
 // determine packages
 // run benchmarks


} else {
  echo "<pre>:",$_POST['_submit_check'],":</pre>";
}

echo "</body></html>";
?>
