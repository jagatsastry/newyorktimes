#http://www.slideshare.net/japerk/nltk-in-20-minutes

from nltk import word_tokenize, sent_tokenize, corpus
import curses.ascii as ascii
import csv
from multiprocessing import Pool
from nltk.tag.stanford import POSTagger
import multiprocessing
import sys
from nltk.stem.wordnet import WordNetLemmatizer
from nltk.corpus.reader.wordnet import NOUN, VERB, ADJ, ADV
from corenlp import StanfordCoreNLP
from corenlp.ttypes import *
from thrift import Thrift
from thrift.transport import TSocket, TTransport
from thrift.protocol import TBinaryProtocol
from StoreSql import storeWord, db
import re
import os
import traceback

root = "/home/jagat/nyt"
stopWords = corpus.stopwords.words("english")
posTagger = POSTagger('/home/jagat/stanford-postagger-full-2013-06-20/models/english-bidirectional-distsim.tagger', '/home/jagat/stanford-postagger-full-2013-06-20/stanford-postagger-3.2.0.jar')
lemmatizer = WordNetLemmatizer()
wPosMap = {'JJ' : ADJ, 'JJS': ADJ, 
           'RB': ADV, 'RBR': ADV, 'RBS': ADV,
           'NN': NOUN, 'NNS': NOUN, 'NNP': NOUN, 'NNPS': NOUN,
           'VB': VERB, 'VBD': VERB, 'VBG': VERB, 'VBN': VERB, 'VBP': VERB, 'VBZ': VERB}
port = 9999
if len(sys.argv) > 1:
  port = int(sys.argv[1])

def getClient():
  socket = TSocket.TSocket('localhost', port)
  socket.setTimeout(10000)
  transport = TTransport.TBufferedTransport(socket)
  protocol = TBinaryProtocol.TBinaryProtocol(transport)
  client = StanfordCoreNLP.Client(protocol)
  transport.open()
  return client

clientPool = [getClient() for i in range(0,16)]

def getLemma(word, pos):
  return lemmatizer.lemmatize(word, wPosMap[pos])

def randomClient():
  from random import choice
  return choice(clientPool)

def getPosStr(pos):
  pos_str = {ADJ: 'adjective', NOUN: 'noun', ADV: 'adverb', VERB: 'verb'}[wPosMap[pos]]
  if pos == 'NNP' or pos == 'NNPS':
    pos_str = 'propernoun'

  return pos_str

def getPosTagsLemma(sentence):
  return [(tagtoken.token, tagtoken.tag, 
          getLemma(tagtoken.token, tagtoken.tag), getPosStr(tagtoken.tag)) for 
          tagtoken in randomClient().tag_text(sentence)[0] if 
          (tagtoken.tag in wPosMap and 
            tagtoken.token not in stopWords and 
            re.match("^[A-Za-z0-9_-]*$", tagtoken.token))]

#for fl in [ root + "/" + x.strip() for x in open(root + "/fileListShort.txt").readlines()]:

def handleFile(fl):
  try:
    handleFileForWord(fl)
  except Exception, e:
    print "Error: ",fl
    traceback.print_exc()

    if os.path.exists(fl + ".word"):
      print "Deleting " + (fl + ".word")
      os.remove(fl + ".word")
    import time
    time.sleep(1)
    print "Retrying for",fl
    handleFileForWord(fl)


def handleFileForWord(fl): 
    if os.path.exists(fl + ".word"):
      print "Skipping " + fl
      return
    print fl

    entities = [r for r in csv.reader(open(fl + ".ent", "rU"), delimiter="\t")]
    body = " ".join([ x.strip() for x in open(fl + ".body", "rU").readlines()]).lower()
    sentences = sent_tokenize(body)

     #articleId, entity, sequence, word, lemma, pos_str, pos, proximity

    fw = None 
    form = "{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\n" 
    printed = False

    for record in entities:
        if len(record) != 5:
          print "ALERT: ",fl,record,len(fl)
          sys.stdout.flush()
          return

        articleId = record[0]
        entity = record[3]
        entityToSearch = record[4].lower()
        seq = 0

        def splitAndClean(x):
          spl = x.split(", ")
          if len(spl) > 1:
              vals = [spl[0], spl[1].split(" (")[0]]
              vals.extend(vals[1].split(" "))
              return vals
          else:
              return x.split(" ")

        filt = splitAndClean(entityToSearch)
        filt.extend(splitAndClean(entity.lower()))
        filt = set(reversed(filt))

        for sentence in sentences:
            if entityToSearch in word_tokenize(sentence):
                seq += 1
                proximity = 0

                words = getPosTagsLemma(sentence)
                for (word, pos, lemma, pos_str) in filter(lambda wrd: wrd[0] not in filt, words):
                    if fw is None:
                        fw = open(fl + ".word", "w")
                    proximity += 1 #Fake proximity. To avoid duplicate records
                    fw.write(form.format(articleId, entity, seq, word, lemma, pos_str, pos, proximity))
                    if(printed == False):
                        print fl+ ".word"
                        printed = True

    if(fw is not None):
        fw.flush()
        fw.close()                

    storeWord(fl)
    #print words
    #print sentences
    
if __name__ == "__main__":
  #if len(sys.argv) == 1:
  #  print "No arg"
  #  sys.exit(0)

  #fl = sys.argv[1]
  #handleFile(fl)
  #idx = int(sys.argv[1])
  files = [root + "/" + fl.strip() for fl in open(root + "/fileList.txt")]
  #start = idx*100000
  #end = min((idx+1)*100000, 1855658)
  #[handleFile(fl) for fl in files]
  #return
  p = Pool(multiprocessing.cpu_count()/2)
  p.map(handleFile, files)
  p.close()
  print "Committing "
  db.commit()
  db.close()
  print "Done"
