#Usage python neighbor_vote <month> [<num_iter> default 1]
#E.g. python neighbor_vote.py 2001_11
#     python neighbor_vote.py 2001_11 2
from numpy import *
import sys
import csv

PRINT_ALL_ITER_STAT = True #Print statistics for every iteration into respective file. If false, it prints only for the last iteration

n1 = 0.839733744493 
p1 = 0.160266255507

PRINT_HEADER = False
current_iter = 0

fl = sys.argv[1]
NUM_ITER = 1
if len(sys.argv) > 2:
    NUM_ITER = int(sys.argv[2])

print "Month: ",fl," Num iter: ",NUM_ITER

words = [x[1] for x in csv.reader(open('graphs/wordmaps/' + fl + '.wordmap'), delimiter='\t') ]
wordMap = {}
for (idx, word) in enumerate(words):
    wordMap[word] = idx

seedWords = [False] * len(words)

wordCalculated = [False] * len(words)
wordProbs = zeros(len(words))

for [word, polarity] in  csv.reader(open('mpqa.csv'), delimiter=','): #csv.reader(open('seedwords.txt'), delimiter='\t'):
    if word in wordMap:
        prob = -1
        if polarity == 'p':
            prob = 1
        wordProbs[wordMap[word]] = prob
        wordCalculated[wordMap[word]] = True
        seedWords[wordMap[word]] = True

entities = [x[1] for x in csv.reader(open('graphs/entitymaps/' + fl + '.entmap'), delimiter='\t') ]
entCalculated = [False] * len(entities)

mt = genfromtxt('graphs/graphs/' + fl + '.graph', delimiter='\t')

while current_iter < NUM_ITER:
    print " %d " % current_iter,
    entProbsNorm = zeros(len(entities))
    entProbs = zeros(len(entities)) #Has to be recomputed

    negCtsE = zeros(len(entities))
    posCtsE = zeros(len(entities))
    neutralCtsE = zeros(len(entities))
    notFoundCtsE = zeros(len(entities))

    for [ent, word, weight] in mt:
        ent = int(ent - 1)
        word = int(word - 1)

        p = wordProbs[word]

        if wordCalculated[word] == False:
            p = 0.5

        multiplier = 1.0
        if p < 0.5:
            multiplier = n1
        elif p > 0.5:
            multiplier = p1

        weight = weight * multiplier

        if wordCalculated[word] == False:
            notFoundCtsE[ent] += weight
        elif p < 0.5:
            negCtsE[ent] += weight
        elif p > 0.5:
            posCtsE[ent] += weight
        else:
            neutralCtsE[ent] += weight

        if seedWords[word] == False:
            continue

        

        entProbs[ent] += (p * weight)
        entProbsNorm[ent] += (weight)
        entCalculated[ent] = True

    for (idx, norm) in enumerate(entProbsNorm):
        if entCalculated[idx]:
            entProbs[idx] /= entProbsNorm[idx]

        #entFw.write(entities[idx] + '\t' + str(prob) + '\t' + str(int(negCtsE[idx])) + '\t' + str(int(posCtsE[idx])) + '\t' + str(int(neutralCtsE[idx])) + '\t' +  str(int(notFoundCtsE[idx])) + '\n')

    negCtsW = zeros(len(words))
    posCtsW = zeros(len(words))
    neutralCtsW = zeros(len(words))
    notFoundCtsW = zeros(len(words))

    wordProbs = zeros(len(words))
    wordProbsNorm = zeros(len(words))

    for [ent, word, weight] in mt:
        ent = int(ent - 1)
        word = int(word - 1)

        if seedWords[word]:
            continue

        p = entProbs[ent]
        if entCalculated[ent] == False:
            notFoundCtsW[word] += weight
        elif p < 0.5:
            negCtsW[word] += weight
        elif p > 0.5:
            posCtsW[word] += weight
        else:
            neutralCtsW[word] += weight


        if entCalculated[ent] == False:
            p = 0.5

        multiplier = 1.0
        if p < 0.5:
            multiplier = n1
        elif p > 0.5:
            multiplier = p1

        wordProbs[word] += (p * weight * multiplier)
        wordProbsNorm[word] += (weight * multiplier)
        wordCalculated[word] = True

    for (idx, norm) in enumerate(wordProbsNorm):
        if seedWords[idx] == 0:
            wordProbs[idx] /= wordProbsNorm[idx]

    if PRINT_ALL_ITER_STAT or current_iter == NUM_ITER - 1:
        entFl = 'graphs/neighbor_vote_multiplier/' + fl + '.iter_' + str(current_iter) + '.ent';
        print " Entity statistics: ",entFl
        entFw = open(entFl, 'w')

        wordFl = 'graphs/neighbor_vote_multiplier/' + fl + '.iter_' + str(current_iter) + '.word';
        print " Word statistics: ",wordFl
        wordFw = open(wordFl, 'w')

        if PRINT_HEADER:
            op = '%30s\t%10s\t%3s\t%3s\t%3s\t%3s\t%5s\n' % ('Entity', 'Prob', 'Neg', 'Pos', 'Neu', 'Unk', '%Pos')
            entFw.write(op)
            op = '%30s\t%10s\t%3s\t%3s\t%3s\t%3s\t%5s\n' % ('Word', 'Prob', 'Neg', 'Pos', 'Neu', 'Unk', '%Pos')
            wordFw.write(op)

        for (idx, prob) in enumerate(entProbs):
            if entCalculated[idx]:
                op = '%30s\t%10f\t%3d\t%3d\t%3d\t%3d\t%2.3f\n' % (entities[idx], prob, negCtsE[idx], posCtsE[idx], neutralCtsE[idx], notFoundCtsE[idx], 100.0 * posCtsE[idx]/float(entProbsNorm[idx]))
                entFw.write(op)

        for (idx, prob) in enumerate(wordProbs):
            if seedWords[idx] and wordProbsNorm[idx] != 0:
                op = '%30s\t%10f\t%3d\t%3d\t%3d\t%3d\t%2.3f\n' % (words[idx], prob, negCtsW[idx], posCtsW[idx], neutralCtsW[idx], notFoundCtsW[idx], 100.0*posCtsW[idx]/float(wordProbsNorm[idx]))
                wordFw.write(op)

        entFw.close()
        wordFw.close()
    current_iter += 1
