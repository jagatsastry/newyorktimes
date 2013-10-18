from numpy import *
import sys

G = genfromtxt(sys.argv[1], delimiter="\t")

thresholdPer = int(sys.argv[2])

scores = list(sorted(G[:, 2]))
score = scores[len(scores) * thresholdPer / 100]

filtG = G[G[:, 2] >= score, 0:2]
filename = sys.argv[1] + '_threshold_' + str(score) + '_.graph'
print filename
savetxt(filename, filtG, fmt='%d', delimiter='\t')
