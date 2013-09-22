year=2006

echo "Creating tables"
while [ $year -ge 1987 ] ; do
    echo table"$year"
#    echo "drop table if exists  word_aggregate_$year" | mysql -p'44Jz2T' jagat
#    echo "create table word_aggregate_$year select month, entity, lemma, count(*) as count from words_$year group by month, entity, lemma order by month, entity, lemma" | mysql -p'44Jz2T' jagat
#    for col in month entity lemma count ; do
#        echo $col
#        echo "create index word_aggregate_""$year""_$col on word_aggregate_$year($col)" | mysql -p'44Jz2T' jagat
#    done
#    echo "insert into word_aggregate select * from word_aggregate_$year" | mysql -p'44Jz2T' jagat

    year=`echo "$year - 1" | bc`
done
for col in month entity lemma count ; do
    echo $col
    echo "create index word_aggregate_$col on word_aggregate($col)" | mysql -p'44Jz2T' jagat
done
