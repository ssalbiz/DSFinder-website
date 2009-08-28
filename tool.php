<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<title>DSFinder Web Tool</title>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<link rel="stylesheet" href="styles.css" type="text/css" />
</head>
<body>

<div id="header">
<h1>DSFinder Web Tool</h1>
</div>

<div id="sidebar">
  <?php include('menu.php') ?>
  <hr /> 
  <p><a href="?log=staging.ds-finder.log">Summary log </a></p>
  <p><a href="?log=staging.ds-finder.log-full">Detailed log </a></p>
</div>

<div id="main">

<?php
 if (isset($_GET['log'])) {
   echo "<pre>";
   include ($_GET['log']);
   echo "</pre>";
   echo "</pre></div></body></html>";
   exit(0);
 }
 else if (isset($_POST['_submit_check'])) {
 if ($_FILES['inputjar']['error'] == UPLOAD_ERR_OK) {
   $dir = '/tmp/';
   $package = $_POST['package_name'];
   $file = $dir . basename($_FILES['inputjar']['name']);
   $tmpfile = $_FILES['inputjar']['tmp_name'];

   if (!move_uploaded_file($_FILES['inputjar']['tmp_name'], $file)) {
     echo "<pre>FAILED to move :", $tmpfile, ": to ", $file, "</pre>";
     exit(1);
   }

   echo "<pre>";
   echo "Received ", basename($file), "\n";
   
   system("bash -- process.sh $file $package", $rc);
   //system('whoami', $rc);
   echo $rc;

   echo "</pre></div></body></html>";
   exit(0);
  // unzip the jar
  // locate entry point
  // determine packages
  // run benchmarks
 }
}
?>

<form action="tool.php" class="compactform" method="post" enctype="multipart/form-data">
 <div class="oneper">
  <label for="package_name">Package prefix to analyze(e.g. java.util)
  <input type="text" name="package_name"  id="package_name" value="" size="40" />
  </label>
 </div>
 <div class="oneper">
  <label for="inputjar">JAR File
  <input type="file" name="inputjar" id="inputjar" size="40" />
  </label>
 </div>
  <input type="hidden" name="_submit_check" id="_submit_check" value="1" />
  <input type="submit" value="Submit" /><br />
</form>
</div>
</body>
</html>
