#months=`ls graphs/test_graphs/ | sed "s/.graph//g"`
echo "Running algorithm"
cat all_months | xargs -P 16 -I % python neighbor_vote.py % 10


function generate_month_rep ()
{

    month=$1
    fl=graphs/neighbor_vote_reports/"$month".report
    iter=0
    printf $month

    echo > $fl
    while [ $iter -lt 10 ] ; do 
        printf " $iter "
        echo "Iter: $iter" >> $fl
        python pos_stats.py $month $iter >> $fl
        iter=`echo "$iter + 1" | bc`
    done
    echo
}

export -f generate_month_rep

echo "Generating reports"
for month in `cat all_months` ; do 
    generate_month_rep $month
done
