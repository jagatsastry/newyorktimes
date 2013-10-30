dir="graphs/neighbor_vote_multiplier/"
#dir="graphs/neighbor_vote_files/neighbor_vote/"
#dir="graphs/neighbor_vote_rescaled_andknown/"

entity=$1
keyword=$2
if [ "" = "$entity" -o "" = "$keyword" ] ; then
 echo "Usage: bash <filename> <entity> <keyword>"
 exit 1
fi


sent_dir=$dir/sentiments
if [ ! -e $sent_dir ] ; then
    mkdir $sent_dir
fi

file=$sent_dir/$keyword.sent
if [ "$3" != "" ] ; then
    file="$file".sent
else
    file="$file"_smooth.sent
fi

#if [ ! -e $file ] ; then
    grep "$entity" -i $dir/*iter_0*.ent   |  sed "s#$dir##g" | sed "s#^/##g" | sort -k 1 > $file
#fi

python plot_sentiment.py $file "$entity" $3
