BEGIN{
  open = 0;
  prev = "other";
}{
  if($1 !~ />/){
    annot = $4; 
    if(annot !~ prev){
      if(NR > 2)
	print start,end,prev;
      start = $1;
      prev = annot;
    }
    end = $2;
  }
}END{
  print start,end,annot;
 }