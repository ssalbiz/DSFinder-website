<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<title>DSFinder Results</title>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<style type="text/css">
#header { padding: 35px 30px 10px 30px; background-color:#202080;}
#header h1 { font-variant: small-caps; }
#sidebar { float: left; width: 18%; padding: 1% 1% 0 1%; margin: 1%; background-color:#202080;}
#main {float: right; width: 78%; padding 0 0 0 0; margin: 0; }
.hidden { display: none; }
.unhidden { display: block; }
a:link {color:#8aede7; background-color:inherit !important;}
a:visited {color:#33FFFF; background-color:inherit !important;}
body { background:#06005c; color:white; } 
</style>
</head>

<body>
<script type="text/javascript" src="./util.js"></script>
<div id="header">
<h1>DSFinder Results</h1>
</div>


<?php

$dbcnx = @mysql_connect('localhost', 'dsfinder_user', 'password');
if (!$dbcnx) {
 echo "<p><a href=\"index.html\"> Home </a></p>";
 echo "<p> Unable to connect to database! </p>";
  exit();
}

if (! @mysql_select_db('dsfinder_new', $dbcnx) ) {
  echo "<p><a href=\"index.html\"> Home </a></p>";
  echo "<p> Unable to connect to database!</p>";
  exit();
}

$pkglist=array('Bean', 'DCM', 'LawOfDemeter', 'NullCheck', 'ProdLine', 'StarJ-Pool', 'Tetris', 'aglets', 'antlr-gunit', 'aoi', 'argoUML', 'asm', 'axion', 'azureus', 'bcel', 'bloat', 'cglib', 'colt', 'columba', 'commons-cli', 'commons-collections', 'commons-lang', 'commons-logging', 'derby', 'dom4j', 'drjava', 'fit', 'fop', 'galleon', 'gantt', 'hibernate', 'hsqldb', 'ireport', 'jag', 'jasper', 'javacc', 'jaxen', 'jchem', 'jcm', 'jedit', 'jeppers', 'jetty', 'jext', 'jfreechart', 'jgraph', 'jmeter', 'jre', 'jung', 'junit', 'jython', 'log4j', 'lucene', 'megamek', 'pmd', 'poi', 'proxool', 'regexp', 'sablecc', 'sandmark', 'tomcat', 'xerces', 'xstream');

echo "<div id=\"sidebar\">\n";
echo "<p><a href=\"index.html\"> Home </a></p>";
echo "<hr />";
foreach ($pkglist as $pkg) {
  print_links($pkg);
}
echo "</div>\n";

echo "<div id=\"main\">";
if (isset($_GET['pkg'])) {
  if (!in_array($_GET['pkg'], $pkglist)) {
    echo "<p> Invalid benchmark! </p>\n";
  } else {
    getlogs($_GET['pkg']);
  }
//  printf("<p><br />IN SUMMARY: <br />TOTAL EXACT FIELD MATCH STRUCTURES: %d OUT OF %d : %0.2f%s<br />", $total_exact_hits, $total_exact, ($total_exact_hits*100.0/$total_exact), "%");
//  printf("TOTAL NON_OBJECT STRUCTURES: %d OUT OF %d : %0.2f%s<br />", $total_nonobject_hits, $total_nonobject,  ($total_nonobject_hits*100.0/$total_nonobject), "%");
//  printf("TOTAL OBJECT STRUCTURES: %d OUT OF %d : %0.2f%s<br />", $total_object_hits, $total_object, ($total_object_hits*100.0/$total_object) ,"%");
  echo "<p>TOTAL PACKAGES: ", count($pkglist), "<br /></p>";
}
echo "</div>";
function print_links($pkg_name) {
  echo "<a href=\"?pkg=",$pkg_name,"\" >", $pkg_name, "</a><br />\n";
}

function getlogs($pkg_name) {
global $total_exact_hits;
global $total_nonobject_hits;
global $total_object_hits ;
global $total_exact ;
global $total_nonobject ;
global $total_object ;
  
  $sql = 'SELECT * FROM fields WHERE pkgname ="' . mysql_real_escape_string($pkg_name) . '" AND type=classname;' ;
  //echo $sql;
  $result = @mysql_query($sql);
  if (!$result) {
    echo "<p> no records found </p>\n";
    exit();
  }
  echo "<p><a href=\"#\" onclick=\"display_report('", $pkg_name, "')\">", Hide, "</a></p>\n";
  echo "<div id=\"", $pkg_name, "\" class=\"unhidden\">\n";
  echo "<a href=\"logs/$pkg_name.ds-finder.log\">",$pkg_name, " summary log</a><br />\n";
  echo "<a href=\"logs/$pkg_name.ds-finder.log-full\">",$pkg_name, " full logged output</a><br />\n";
  echo "<p>EXACT FIELD MATCHES:</p>\n";
  printResults($result, $total_exact, $total_exact_hits);
  $sql = 'SELECT * FROM fields WHERE pkgname="' . mysql_real_escape_string($pkg_name) . '" AND type!=classname AND type!="java.lang.Object";';
  $result = @mysql_query($sql);
  if (!$result) {
    echo "<p> no records found </p>\n";
    exit();
  }
  echo "<p>NON-OBJECT FIELDS:</p>\n";
  printResults($result, $total_nonobject, $total_nonobject_hits);
  $sql = 'SELECT * FROM fields WHERE pkgname="' . mysql_real_escape_string($pkg_name) . '" AND type="java.lang.Object";';
  $result = @mysql_query($sql);
  if (!$result) {
    echo "<p> no records found </p>\n";
    exit();
  }
  echo "<p>OBJECT FIELDS:</p>\n";
  printResults($result, $total_object, $total_object_hits);
  echo "<hr /><hr />\n";
  echo "</div>\n";
}

function printResults($result, &$total, &$total_hits) {
  $positives = 0; $negatives = 0;
  $pkg_name = 0;
  $local_total = 0;
  echo "<table border=\"0\" cellpadding=\"1\" cellspacing=\"0\" width=\"100%\">\n";
  echo '<tr><th>Class</th><th>Type</th><th>Name</th><th>DS</th><th>DS Type</th></tr>';
  while ($row = mysql_fetch_array($result)) {
    $checked = $row['datastructure'] ? "checked=\"checked\"":"";
    $bgcolor = "red";
    $pkg_name = $row['pkgname'];
    if ($checked) {
      if (strcmp($row['remarks'], 'List')==0) {
        $bgcolor = "green";
      } elseif (strcmp($row['remarks'], 'Tree')==0) {
        $bgcolor = "olive";
      } elseif (strcmp($row['remarks'], 'Graph') == 0) {
        $bgcolor = "aqua";
      } elseif (strcmp($row['remarks'], 'Hash') == 0) {
        $bgcolor = "teal";
      } else {
        $bgcolor = "lime";
      }
    }

    echo "<tr style=\"background-color:", $bgcolor , "\">\n";
    echo "<td><a href=\"/cgi-bin/view.cgi?name=",$row['classname'],";pkg=",$pkg_name,";type=",$row['type'],";varname=", $row['name'],"\">\n";
    //truncate?
    echo preg_replace("/([\w\.]+)\./", "" ,$row['classname']), "</a></td><td>*", preg_replace("/([\w\.]+)\./", "", $row['type']), "*</td><td>", $row['name'];
    echo "</td><td><input type=\"checkbox\" name=\"id" . $row['id'] . "\" value=\"1\" " . $checked . " /></td>\n";
    echo '<td>',$row['remarks'];
    //echo "<select name=\"remarks", $row['id'], "\">\n";
    //echo "<option value=\"List\" ", (strcmp($row['remarks'], 'List') == 0)?"selected=\"selected\"":"" ,">List</option>\n";
    //echo "<option value=\"Tree\" ", (strcmp($row['remarks'], 'Tree') == 0)?"selected=\"selected\"":"" ,">Tree</option>\n";
    //echo "<option value=\"Graph\" ", (strcmp($row['remarks'], 'Graph') == 0)?"selected=\"selected\"":"" ,">Graph</option>\n";
    //echo "<option value=\"Hash\" ", (strcmp($row['remarks'], 'Hash') == 0)?"selected=\"selected\"":"" ,">Hash</option>\n";
    //echo "<option value=\"None\" ", (strcmp($row['remarks'], 'None') == 0 || strcmp($bgcolor, 'red') == 0)?"selected=\"selected\"":"" ,">None</option>\n";
    //echo "</select>\n";
    echo "</td></tr>\n";
    $positives+= $checked?1:0;
    $negatives+= $checked?0:1;
    $local_total++;
  }
  $total+= $local_total;
  if ($local_total <= 0)
    echo '<tr><td>No Matches</td></tr>';
  echo "</table>\n";
  if ($positives || $negatives) {
    $total_hits += $positives;
    echo "\n<p>Total of: ", $positives, " positives, and ", $negatives, " negatives in ", $pkg_name, " <br />\n";
    echo "percentage positive: ", ($positives*100.0)/($positives+$negatives*1.0), "%</p>\n";
  }

}

?>


</body>
</html> 
