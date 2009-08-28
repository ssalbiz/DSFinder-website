<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<title>DSFinder Web Tool</title>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<link ref="stylesheet" href="styles.css" type="text/css" >

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
<table id="other">
<tr> <th> Benchmark </th> <th> External Array Accesses </th>  </tr>
  <tr> <td> aglets </td> <td align="right">   0 </td> </tr>
  <tr> <td> antlr-gunit </td> <td align="right">   0 </td> </tr>
  <tr> <td> aoi </td> <td align="right">  74 </td> </tr>
  <tr> <td> argoUML </td> <td align="right">   0 </td> </tr>
  <tr> <td> asm </td> <td align="right">   4 </td> </tr>
  <tr> <td> axion </td> <td align="right">   0 </td> </tr>
  <tr> <td> azureus </td> <td align="right">   8 </td> </tr>
  <tr> <td> bcel </td> <td align="right">   0 </td> </tr>
  <tr> <td> Bean </td> <td align="right">   0 </td> </tr>
  <tr> <td> bloat </td> <td align="right">   0 </td> </tr>
  <tr> <td> cglib </td> <td align="right">   0 </td> </tr>
  <tr> <td> colt </td> <td align="right">   3 </td> </tr>
  <tr> <td> columba </td> <td align="right">   0 </td> </tr>
  <tr> <td> commons-cli </td> <td align="right">   0 </td> </tr>
  <tr> <td> commons-collections </td> <td align="right">   0 </td> </tr>
  <tr> <td> commons-lang </td> <td align="right">   0 </td> </tr>
  <tr> <td> commons-logging </td> <td align="right">   0 </td> </tr>
  <tr> <td> DCM </td> <td align="right">   0 </td> </tr>
  <tr> <td> derby </td> <td align="right">   0 </td> </tr>
  <tr> <td> dom4j </td> <td align="right">   0 </td> </tr>
  <tr> <td> drjava </td> <td align="right">   2 </td> </tr>
  <tr> <td> fit </td> <td align="right">   0 </td> </tr>
  <tr> <td> fop </td> <td align="right">   4 </td> </tr>
  <tr> <td> galleon </td> <td align="right">   0 </td> </tr>
  <tr> <td> gantt </td> <td align="right">   0 </td> </tr>
  <tr> <td> hibernate </td> <td align="right">   0 </td> </tr>
  <tr> <td> hsqldb </td> <td align="right">  42 </td> </tr>
  <tr> <td> ireport </td> <td align="right">   0 </td> </tr>
  <tr> <td> jag </td> <td align="right">   0 </td> </tr>
  <tr> <td> jasper </td> <td align="right">   0 </td> </tr>
  <tr> <td> javacc </td> <td align="right">   4 </td> </tr>
  <tr> <td> jaxen </td> <td align="right">   0 </td> </tr>
  <tr> <td> jchem </td> <td align="right">   0 </td> </tr>
  <tr> <td> jcm </td> <td align="right">   0 </td> </tr>
  <tr> <td> jedit </td> <td align="right">   6 </td> </tr>
  <tr> <td> jeppers </td> <td align="right">   0 </td> </tr>
  <tr> <td> jetty </td> <td align="right">   2 </td> </tr>
  <tr> <td> jext </td> <td align="right">   0 </td> </tr>
  <tr> <td> jfreechart </td> <td align="right">   0 </td> </tr>
  <tr> <td> jgraph </td> <td align="right">   0 </td> </tr>
  <tr> <td> jmeter </td> <td align="right">   0 </td> </tr>
  <tr> <td> jre </td> <td align="right">  56 </td> </tr>
  <tr> <td> jung </td> <td align="right">   0 </td> </tr>
  <tr> <td> junit </td> <td align="right">   0 </td> </tr>
  <tr> <td> jython </td> <td align="right">  18 </td> </tr>
  <tr> <td> LawOfDemeter </td> <td align="right">   0 </td> </tr>
  <tr> <td> log4j </td> <td align="right">   0 </td> </tr>
  <tr> <td> lucene </td> <td align="right">  50 </td> </tr>
  <tr> <td> megamek </td> <td align="right">  29 </td> </tr>
  <tr> <td> NullCheck </td> <td align="right">   0 </td> </tr>
  <tr> <td> pmd </td> <td align="right">   0 </td> </tr>
  <tr> <td> poi </td> <td align="right">   0 </td> </tr>
  <tr> <td> ProdLine </td> <td align="right">   0 </td> </tr>
  <tr> <td> proxool </td> <td align="right">   0 </td> </tr>
  <tr> <td> regexp </td> <td align="right">   0 </td> </tr>
  <tr> <td> sable </td> <td align="right">  19 </td> </tr>
  <tr> <td> sandmark </td> <td align="right">   0 </td> </tr>
  <tr> <td> scala </td> <td align="right">   0 </td> </tr>
  <tr> <td> StarJ-Pool </td> <td align="right">   0 </td> </tr>
  <tr> <td> Tetris </td> <td align="right">   0 </td> </tr>
  <tr> <td> tomcat </td> <td align="right">   0 </td> </tr>
  <tr> <td> xerces </td> <td align="right">   0 </td> </tr>
  <tr> <td> xstream </td> <td align="right">   0 </td> </tr>
   </table>

</div>
</body>
</html>