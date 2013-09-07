#http://www.slideshare.net/japerk/nltk-in-20-minutes

from nltk import word_tokenize, sent_tokenize, corpus
import curses.ascii as ascii
import csv
from multiprocessing import Pool
import multiprocessing
import sys
import MySQLdb
import os

root = "/home/jagat/nyt"
stopWords = corpus.stopwords.words("english")
db = MySQLdb.connect(host="localhost", # your host, usually localhost
                     user="jagat", # your username
                      passwd="d", # your password
                      db="nyt") # name of the data base
#db.autocommit(True)                      
c = db.cursor()


#for fl in [ root + "/" + x.strip() for x in open(root + "/fileListShort.txt").readlines()]:
def handleFile(fl): 
    print fl
    storeEnt(fl)
    storeWord(fl)

def storeEnt(fl): 
    entf = None
    if os.path.exists(fl + ".ent"):
        entf = open(fl + ".ent", "rU")
        entities = [tuple(r)  for r in csv.reader(entf, delimiter="\t") if len(r) == 5]
        c.executemany("""INSERT INTO entities
          ( article_id, pub_date, entity_type, entity, entity_to_search ) 
          VALUES (%s, %s, %s, %s, %s)""",
          entities)
    if entf != None:
      entf.close()

def storeWord(fl):
    wordf = None
    if os.path.exists(fl + ".word"):
        wordf = open(fl + ".word")
        words = [tuple(r) for r in csv.reader(wordf, delimiter="\t")]
        c.executemany("""INSERT INTO words
          (article_id, entity, sequence, word, lemma, pos_str, pos, proximity) 
          VALUES (%s, %s, %s, %s, %s, %s, %s, %s)""",
          words)
        db.commit()
        print "Stored",len(words),"records into SQL from",fl + ".word"
    if wordf != None:
      wordf.close()
    
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
  p = Pool(multiprocessing.cpu_count())
  p.map(handleFile, files)
  p.close()
  db.commit()
  db.close()

