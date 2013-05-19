BEGIN{
  color[numColor++] = "darkgray";
  color[numColor++] = "lightgray";
  color[numColor++] = "white";
  color[numColor++] = "white";
  color[numColor++] = "green";
  color[numColor++] = "yellow";
  color[numColor++] = "blue";
  color[numColor++] = "black, linestyle=dotted";
  color[numColor++] = "yellow";
  color[numColor++] = "green";
  color[numColor++] = "red";
  color[numColor++] = "black";
  color[numColor++] = "cyan";
  color[numColor++] = "blue";
  color[numColor++] = "magenta";

}
{
  if(!col[$3]){
    if(numColUsed < numColor)
      col[$3] = color[numColUsed++];
    else
      col[$3] = "white";
  }
  c = col[$3];

  coord1[numCoord] = $1;
  coord2[numCoord] = $2;
  colCor[numCoord++] = c;
}END{
  max = coord2[numCoord-1];
  f = 1/1000000;
  x = 3;
  print "\\begin{pspicture}(0,0)(15,1)";
  print "\\psset{xunit="x"cm}";
  printf "\\psaxes[Dx=0.2]{}(0,0)(%.4f,0)\n",max*f;
  print "\\psset{linewidth=15pt}";
  for(i=0;i<numCoord;i++){
    c1 = coord1[i]*f;
    c2 = coord2[i]*f;
    printf "\\psline[linecolor=%s](%.4f,1)(%.4f,1)\n",colCor[i],c1,c2;
  }
  print "\\end{pspicture}";
  for(a in col)
    print "%% " a, col[a];
 }
  