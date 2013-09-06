num=`cat nyt.out | wc -l`
echo $num
while [ $num -ne 1855658 ] ; do  
  echo "Waiting"
  sleep 10 
  num=`cat nyt.out | wc -l`
  echo $num
done   
echo "Executing"
python ~/eclipse_workspace/NYTWorkTokenize/src/CloseWords.py
echo "Done"
