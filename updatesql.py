#http://www.slideshare.net/japerk/nltk-in-20-minutes

from nltk import word_tokenize, sent_tokenize, corpus
from nltk.stem.wordnet import WordNetLemmatizer
import curses.ascii as ascii
import csv
from multiprocessing import Pool
import multiprocessing
import sys
import MySQLdb
import os
import re

root = "/home/jagat/nyt"
stopWords = corpus.stopwords.words("english")
db = MySQLdb.connect(host="localhost", # your host, usually localhost
                     user="jagat", # your username
                      passwd="d", # your password
                      db="nyt") # name of the data base
#db.autocommit(True)                      
c = db.cursor()
lmtzr = WordNetLemmatizer()

#for fl in [ root + "/" + x.strip() for x in open(root + "/fileListShort.txt").readlines()]:
def handleFile(fl): 
    print fl
    wordf = None
    
    if os.path.exists(fl):
        wordf = open(fl, "rU")
        words = [r for r in csv.reader(wordf, delimiter="\t") if len(r) == 5]
        
        wordtuple = []
        for (articleId, entity, sequence, word, proximity) in words:
          if re.match("^[A-Za-z]", word):
            lemma = lmtzr.lemmatize(word)
            c.executemany("""INSERT INTO words
              (article_id, entity, sequence, word, lemma, proximity) 
              VALUES (%s, %s, %s, %s, %s, %s)""",
              words)
    '''        
        for record in words:
            if len(record) != 5:
              print "ALERT: ",fl,record,len(record)
              sys.stdout.flush()
              continue
    
            articleId = record[0]
            entity = record[1]
            sequence = record[2]
            entityToSearch = record[3]
            proximity = record[4]
    '''
    if entf != None:
      entf.close()
    if wordf != None:
      wordf.close()
    #print words
    #print sentences
    
if __name__ == "__main__":
  #if len(sys.argv) == 1:
  #  print "No arg"
  #  sys.exit(0)

  #fl = sys.argv[1]
  #handleFile(fl)
  #idx = int(sys.argv[1])
  files = [root + "/" + fl.strip() for fl in open(root + "/fileList.word.txt")]
  #start = idx*100000
  #end = min((idx+1)*100000, 1855658)
  p = Pool(multiprocessing.cpu_count())
  p.map(handleFile, files)
  p.close()
  db.commit()
  db.close()

