from numpy import *
import sys
import csv

NUM_ITER = 1
current_iter = 0

fl = sys.argv[1]

words = [x[1] for x in csv.reader(open('graphs/wordmaps/' + fl + '.wordmap'), delimiter='\t') ]
wordMap = {}
for (idx, word) in enumerate(words):
    wordMap[word] = idx

seedWords = zeros(len(words))

wordCalculated = zeros(len(words))
wordProbs = zeros(len(words))
wordProbsNorm = zeros(len(words))

for [prob, word] in csv.reader(open('seedwords.txt'), delimiter='\t'):
    if word in wordMap:
        wordProbs[wordMap[word]] = float(prob)
        wordCalculated[wordMap[word]] = 1
        seedWords[wordMap[word]] = 1

entities = [x[1] for x in csv.reader(open('graphs/entitymaps/' + fl + '.entmap'), delimiter='\t') ]
entProbs = zeros(len(entities))
entProbsNorm = zeros(len(entities))
entCalculated = zeros(len(entities))

mt = genfromtxt('graphs/graphs/' + fl + '.graph', delimiter='\t')


entFw = open('graphs/neighbor_vote/' + fl + '.iter_' + str(current_iter) + '.ent', 'w')
op = '%30s\t%10s\t%3s\t%3s\t%3s\t%3s\n' % ('Entity', 'Prob', 'Neg', 'Pos', 'Neu', 'Unk')
entFw.write(op)

wordFw = open('graphs/neighbor_vote/' + fl + '.iter_' + str(current_iter) + '.word', 'w')
op = '%30s\t%10s\t%3s\t%3s\t%3s\t%3s\n' % ('Word', 'Prob', 'Neg', 'Pos', 'Neu', 'Unk')
wordFw.write(op)

negCtsE = zeros(len(entities))
posCtsE = zeros(len(entities))
neutralCtsE = zeros(len(entities))
notFoundCtsE = zeros(len(entities))

for [ent, word, weight] in mt:
    ent = ent - 1
    word = word - 1
    p = wordProbs[word]

    if wordCalculated[word] == 0:
        notFoundCtsE[ent] += weight
    elif p < 0.5:
        negCtsE[ent] += weight
    elif p > 0.5:
        posCtsE[ent] += weight
    else:
        neutralCtsE[ent] += weight

    if wordCalculated[word] == 0:
        p = 0.5

    entProbs[ent] += (p * weight)
    entProbsNorm[ent] += weight
    entCalculated[ent] = 1

for (idx, norm) in enumerate(entProbsNorm):
    entProbs[idx] /= entProbsNorm[idx]

for (idx, prob) in enumerate(entProbs):
    op = '%30s\t%10f\t%3d\t%3d\t%3d\t%3d\n' % (entities[idx], prob, negCtsE[idx], posCtsE[idx], neutralCtsE[idx], notFoundCtsE[idx])
    entFw.write(op)
    #entFw.write(entities[idx] + '\t' + str(prob) + '\t' + str(int(negCtsE[idx])) + '\t' + str(int(posCtsE[idx])) + '\t' + str(int(neutralCtsE[idx])) + '\t' +  str(int(notFoundCtsE[idx])) + '\n')

negCtsW = zeros(len(words))
posCtsW = zeros(len(words))
neutralCtsW = zeros(len(words))
notFoundCtsW = zeros(len(words))

for [ent, word, weight] in mt:
    ent = ent - 1
    word = word - 1
    p = entProbs[ent]

    if entCalculated[ent] == 0:
        notFoundCtsW[word] += weight
    elif p < 0.5:
        negCtsW[word] += weight
    elif p > 0.5:
        posCtsW[word] += weight
    else:
        neutralCtsW[word] += weight

    if seedWords[word] == 1:
        continue

    if entCalculated[ent] == 0:
        p = 0.5

    wordProbs[word] += (p * weight)
    wordProbsNorm[word] += weight
    wordCalculated[word] = 1

for (idx, norm) in enumerate(wordProbsNorm):
    if seedWords[idx] == 0:
        wordProbs[idx] /= wordProbsNorm[idx]

for (idx, prob) in enumerate(wordProbs):
    op = '%30s\t%10f\t%3d\t%3d\t%3d\t%3d\n' % (words[idx], prob, negCtsW[idx], posCtsW[idx], neutralCtsW[idx], notFoundCtsW[idx])
    wordFw.write(op)

entFw.close()
wordFw.close()
