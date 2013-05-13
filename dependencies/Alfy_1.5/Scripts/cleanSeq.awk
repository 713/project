#!/usr/bin/awk -f
# quantifyGenotypes.awk
# description: change acgt to ACGT and remove all other 
#   characters from the sequence (but not from the header)
# author: Bernhard Haubold, haubold@evolbio.mpg.de
# date: February 8, 2011
BEGIN{
}{
  if(/>/){
    print;
  }else{
    seq = toupper($0);
    gsub(/[^ACGT]/,"",seq);
    print seq;
  }
}
