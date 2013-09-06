#http://www.slideshare.net/japerk/nltk-in-20-minutes

from nltk import word_tokenize, sent_tokenize, corpus
import curses.ascii as ascii
import csv
from multiprocessing import Pool
import multiprocessing
import sys

root = "/home/jagat/nyt"
stopWords = corpus.stopwords.words("english")

#for fl in [ root + "/" + x.strip() for x in open(root + "/fileListShort.txt").readlines()]:
def handleFile(fl): 
    print "Handling " + fl
    entities = [r for r in csv.reader(open(fl + ".ent", "rU"), delimiter="\t")]
    body = " ".join([ x.strip() for x in open(fl + ".body", "rU").readlines()])
    sentences = sent_tokenize(body)
    words = [[word for word in word_tokenize(sentence)\
              if ((len(word) == 1 and ascii.ispunct(word[0])) == False\
              and word.lower() not in stopWords)]\
             for sentence in sentences]
    fw = None
    form = "{0}\t{1}\t{2}\t{3}\t{4}\n" #articleId, entity, sequence, closeWord, proximity
    printed = False
    for record in entities:
        if len(record) != 5:
          print "ALERT: ",fl,record,len(fl)
          sys.stdout.flush()
          return

        articleId = record[0]
        entity = record[3]
        entityToSearch = record[4]
        seq = 0
        filt = entityToSearch.split(", ")
        if len(filt) > 1:
            filt = [filt[0], filt[1].split(" (")[0]]
        else:
            filt = entityToSearch.split(" ")
        filt = list(reversed(filt))
        for (sent, words) in zip(sentences, words):
            if entityToSearch in sent:
                seq += 1
                proximity = 0
                for word in filter(lambda wrd: wrd not in filt, words):
                    if fw is None:
                        fw = open(fl + ".word", "w")
                    proximity += 1 #Fake proximity. To avoid duplicate records
                    fw.write(form.format(articleId, entity, seq, word, proximity))
                    if(printed == False):
                        print "Writing to " + fl+ ".word"
                        printed = True
    if(fw is not None):
        fw.flush()
        fw.close()                
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
  p = Pool(multiprocessing.cpu_count())
  p.map(handleFile, files)
  p.close()
