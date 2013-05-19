#!/usr/bin/awk -f
# quantifyGenotypes.awk
# description: quantify output from alfy
# author: Bernhard Haubold, haubold@evolbio.mpg.de
# date: February 13, 2009
BEGIN{
  first = 1;
  cmd = "sort -k 2 -r -n";
}{
  if(/>/){
    if(first){
      first = 0;
      print;
    }
    else{
      for(a in s)
	print a "\t" s[a]/total | cmd;
      close(cmd);
      print;
      for(a in s)
	delete s[a];
      total = 0;
    }
  }else{
    len = $2 - $1 + 1;
    total += len;
    # key construction for >= 1 genotypes
    key = "";
    for(i=4;i<=NF;i++)
      key = key $i;
    s[key] += len;
  }
}END{
  for(a in s)
    printf("%s\t%2.1f\n", a, s[a]/total*100) | cmd;
 }
