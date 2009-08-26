
function display_report(benchmark) {
  benchmark = document.getElementById(benchmark);
  if (benchmark!=null && benchmark!="") {
     benchmark.className=(benchmark.className == 'hidden') ? 'unhidden' : 'hidden';
  }
}
