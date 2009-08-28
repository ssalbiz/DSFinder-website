<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<title>DSFinder Web Tool</title>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<link rel="stylesheet" href="styles.css" type="text/css" />

</head>
<body onload="alterRows('other')">


<script type="text/javascript">
// http://www.bewebmaster.com/222.php

function alterRows(id) {
 if (document.getElementsByTagName) {
  var table = document.getElementById(id);
  var rows = table.getElementsByTagName('tr');
  for (var i = 0; i < rows.length; i++) {
   if (i % 2 == 0) rows[i].className="c1"; else rows[i].className="c2";
  }
 }
}
</script>

<div id="header">
<h1>Other Results</h1>
</div>

<div id="sidebar">
<?php include('menu.php') ?>
</div>

<div id="main">
<h2>External Array Accesses</h2>
<table id="arrayexp">
<tr> <th> Benchmark </th> <th> External Array Accesses </th>  </tr>
  <tr> <td> aglets </td> <td class="right">   0 </td> </tr>
  <tr> <td> antlr-gunit </td> <td class="right">   0 </td> </tr>
  <tr> <td> aoi </td> <td class="right">  74 </td> </tr>
  <tr> <td> argoUML </td> <td class="right">   0 </td> </tr>
  <tr> <td> asm </td> <td class="right">   4 </td> </tr>
  <tr> <td> axion </td> <td class="right">   0 </td> </tr>
  <tr> <td> azureus </td> <td class="right">   8 </td> </tr>
  <tr> <td> bcel </td> <td class="right">   0 </td> </tr>
  <tr> <td> Bean </td> <td class="right">   0 </td> </tr>
  <tr> <td> bloat </td> <td class="right">   0 </td> </tr>
  <tr> <td> cglib </td> <td class="right">   0 </td> </tr>
  <tr> <td> colt </td> <td class="right">   3 </td> </tr>
  <tr> <td> columba </td> <td class="right">   0 </td> </tr>
  <tr> <td> commons-cli </td> <td class="right">   0 </td> </tr>
  <tr> <td> commons-collections </td> <td class="right">   0 </td> </tr>
  <tr> <td> commons-lang </td> <td class="right">   0 </td> </tr>
  <tr> <td> commons-logging </td> <td class="right">   0 </td> </tr>
  <tr> <td> DCM </td> <td class="right">   0 </td> </tr>
  <tr> <td> derby </td> <td class="right">   0 </td> </tr>
  <tr> <td> dom4j </td> <td class="right">   0 </td> </tr>
  <tr> <td> drjava </td> <td class="right">   2 </td> </tr>
  <tr> <td> fit </td> <td class="right">   0 </td> </tr>
  <tr> <td> fop </td> <td class="right">   4 </td> </tr>
  <tr> <td> galleon </td> <td class="right">   0 </td> </tr>
  <tr> <td> gantt </td> <td class="right">   0 </td> </tr>
  <tr> <td> hibernate </td> <td class="right">   0 </td> </tr>
  <tr> <td> hsqldb </td> <td class="right">  42 </td> </tr>
  <tr> <td> ireport </td> <td class="right">   0 </td> </tr>
  <tr> <td> jag </td> <td class="right">   0 </td> </tr>
  <tr> <td> jasper </td> <td class="right">   0 </td> </tr>
  <tr> <td> javacc </td> <td class="right">   4 </td> </tr>
  <tr> <td> jaxen </td> <td class="right">   0 </td> </tr>
  <tr> <td> jchem </td> <td class="right">   0 </td> </tr>
  <tr> <td> jcm </td> <td class="right">   0 </td> </tr>
  <tr> <td> jedit </td> <td class="right">   6 </td> </tr>
  <tr> <td> jeppers </td> <td class="right">   0 </td> </tr>
  <tr> <td> jetty </td> <td class="right">   2 </td> </tr>
  <tr> <td> jext </td> <td class="right">   0 </td> </tr>
  <tr> <td> jfreechart </td> <td class="right">   0 </td> </tr>
  <tr> <td> jgraph </td> <td class="right">   0 </td> </tr>
  <tr> <td> jmeter </td> <td class="right">   0 </td> </tr>
  <tr> <td> jre </td> <td class="right">  56 </td> </tr>
  <tr> <td> jung </td> <td class="right">   0 </td> </tr>
  <tr> <td> junit </td> <td class="right">   0 </td> </tr>
  <tr> <td> jython </td> <td class="right">  18 </td> </tr>
  <tr> <td> LawOfDemeter </td> <td class="right">   0 </td> </tr>
  <tr> <td> log4j </td> <td class="right">   0 </td> </tr>
  <tr> <td> lucene </td> <td class="right">  50 </td> </tr>
  <tr> <td> megamek </td> <td class="right">  29 </td> </tr>
  <tr> <td> NullCheck </td> <td class="right">   0 </td> </tr>
  <tr> <td> pmd </td> <td class="right">   0 </td> </tr>
  <tr> <td> poi </td> <td class="right">   0 </td> </tr>
  <tr> <td> ProdLine </td> <td class="right">   0 </td> </tr>
  <tr> <td> proxool </td> <td class="right">   0 </td> </tr>
  <tr> <td> regexp </td> <td class="right">   0 </td> </tr>
  <tr> <td> sable </td> <td class="right">  19 </td> </tr>
  <tr> <td> sandmark </td> <td class="right">   0 </td> </tr>
  <tr> <td> scala </td> <td class="right">   0 </td> </tr>
  <tr> <td> StarJ-Pool </td> <td class="right">   0 </td> </tr>
  <tr> <td> Tetris </td> <td class="right">   0 </td> </tr>
  <tr> <td> tomcat </td> <td class="right">   0 </td> </tr>
  <tr> <td> xerces </td> <td class="right">   0 </td> </tr>
  <tr> <td> xstream </td> <td class="right">   0 </td> </tr>
   </table>

</div>
</body>
</html>
