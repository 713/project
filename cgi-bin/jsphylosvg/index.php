<?php

$myFile = "aligned.fasta_align_phylip_phyml_tree.txt";
$fh = fopen($myFile, 'r');
$theData = fread($fh);
fclose($fh);

echo <<<EOT
<html>
<head>
<script type="text/javascript" src="./raphael-min.js" ></script> 
<script type="text/javascript" src="./jsphylosvg-min.js"></script> 

<script type="text/javascript">
window.onload = function(){
var dataObject = { newick: '$theData' };";
var dataObject = { newick: '7482847,Carp:0.20619571,(Frog:0.28811231,(Chicken:0.66409644,(Human:0.68873953,((Mouse:0.14036386,Rat:0.12557381)0.989000:0.16334473,(Whale:0.16400887,(Cow:0.17075212,Seal:0.25100152)0.864000:0.03988935)0.860000:0.05735875)0.888000:0.12629816)0.978000:0.21996847)0.667000:0.07186549)0.986000:0.20915770);' };
//<!--
//  var dataObject = { newick: '(((Espresso:2,(Milk Foam:2,Espresso Macchiato:5,((Steamed Milk:2,Cappuccino:2,(Whipped Cream:1,Chocolate Syrup:1,Cafe Mocha:3):5):5,Flat White:2):5):5):1,Coffee arabica:0.1,(Columbian:1.5,((Medium Roast:1,Viennese Roast:3,American Roast:5,Instant Coffee:9):2,Heavy Roast:0.1,French Roast:0.2,European Roast:1):5,Brazilian:0.1):1):1,Americano:10,Water:1);' };
//  
//-->
  phylocanvas = new Smits.PhyloCanvas(
      dataObject,
      'svgCanvas', 
      500, 500
      );
};
</script>
</head>
<body>
<div id="svgCanvas"> </div>
</body>
</html>			
EOT;
?>
