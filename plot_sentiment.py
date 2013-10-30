import csv
import sys

from numpy import *
from matplotlib import *
from pylab import *
import pylab
from common_mpl_props import *


SMOOTHEN = True
print sys.argv
if len(sys.argv) > 3:
    SMOOTHEN = False

title_1 = 'Sentiment of \'' + sys.argv[2] + '\' by month'
if SMOOTHEN:
    title_1 += ' (smoothened)'
title(title_1, {'fontsize':'22'})
xlabel('Month', {'fontsize':'22'})
ylabel('Sentiment', {'fontsize':'22'})
ylim(-1.0, 1.0)
data = array([ (row[0].strip().split(".iter")[0], float(row[1].strip())) for row in csv.reader(open(sys.argv[1]), delimiter='\t') ])

n = len(data)
sent = data[:, 1]
if SMOOTHEN:
    sent = [float(x) for x in sent]
    smoothSum = add(add(sent,  sent[1:] + [0]),  [0] +  sent[:-1])
    smoothDiv = [2] + ( [3] * (len(sent) - 2)) + [2]
    sent = divide(smoothSum, smoothDiv)
num_ticks = (len(data) + 1)/min(10, len(data) + 1)
xticks(range(1, len(data) + 1)[::num_ticks], data[:, 0][::num_ticks], rotation=20)
pylab.plot(range(1, len(data) + 1), sent, 'd-', color='b')

show()
