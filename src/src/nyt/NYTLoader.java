package nyt;

import com.nytlabs.corpus.NYTCorpusDocumentParser;
import com.nytlabs.corpus.NYTCorpusDocument;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

class EntWords {

  EntWords(String entityNameUsed, int sequence, String closeWord, int wordProximity) {
    this.entityNameUsed = entityNameUsed;
    this.sequence = sequence;
    this.closeWord = closeWord;
    this.wordProximity = wordProximity;
  }

  String entityNameUsed;
  int sequence;
  String closeWord;
  int wordProximity;
}

public class NYTLoader implements Runnable {
  private static String ROOT = "/home/jagat/nyt/";
  private File m_file;
  private static ExecutorService TPOOL = Executors.newFixedThreadPool(50);
  int articleId;// = doc.getGuid();
  String date;;// = doc.getPublicationYear() + "_" + doc.getPublicationMonth() + "_" + doc.getPublicationDayOfMonth();
  List<String> locations;// = doc.getLocations();
  List<String> people;// = doc.getPeople();
  List<String> orgs;// = doc.getOrganizations();
  String body;
//  List<String> sentences;
//  List<List<String>> sentenceTokens;

  public NYTLoader(File file) {
    m_file = file;
    NYTCorpusDocumentParser parser = new NYTCorpusDocumentParser();
    NYTCorpusDocument doc =
        parser.parseNYTCorpusDocumentFromFile(m_file, false);
    articleId = doc.getGuid();
    date = doc.getPublicationYear() + "_" + doc.getPublicationMonth() + "_" + doc.getPublicationDayOfMonth();
    locations = doc.getLocations();
    people = doc.getPeople();
    orgs = doc.getOrganizations();
    body = doc.getBody();
    if(body == null) body = "";
//    sentences = new ArrayList<String>//tokenizeIntoSentences(doc.getBody());
//    sentenceTokens = new ArrayList<List<String>>();
//    for(String sentence: sentences)
//      sentenceTokens.add(tokenizeIntoWords(sentences));
  }



  public void run()  {
    try {

      String entform = "%d\t%s\t%s\t%s\t%s\n"; //ArticleId, Date, EntityType, Entity, EntityToSearch

      FileWriter efw = new FileWriter(new File(m_file.getAbsolutePath() + ".ent"));
      FileWriter sfw = new FileWriter(new File(m_file.getAbsolutePath() + ".body"));
      sfw.write(body);

      for(String loc: locations) {
        loc = loc.replaceAll("[\r\n]", "");
        String entSan = sanitizeLocation(loc);
        String val = String.format(entform, articleId, date, "location", loc, entSan);
        efw.write(val);
      }

      for(String ppl: people) {
        ppl = ppl.replaceAll("[\r\n]", "");
        String pplSan = sanitizePeople(ppl);
        String val = String.format(entform, articleId, date, "people", ppl, pplSan);
        efw.write(val);
      }

      for(String org: orgs) {
        org = org.replaceAll("[\r\n]", "");
        String orgSan = sanitizeOrg(org);
        String val = String.format(entform, articleId, date, "organization", org, orgSan);
        efw.write(val);
      }
      efw.flush();
      sfw.flush();
      efw.close();
      sfw.close();
      System.out.println(m_file.getAbsolutePath());

    } catch(Throwable t) {
      t.printStackTrace();
      throw new RuntimeException(t.getMessage(), t);
    }
  }

  private String sanitizeLocation(final String loc) {
    return loc.split("\\s+\\(")[0].split(", ")[0];//BRONX (NYC) or ISRAEL, STATE OF
  }

  private String sanitizePeople(final String ppl) {
    if(ppl.contains(", "))
      return ppl.split(",\\s+")[0];
    else return ppl.split("\\s+")[0];
  }

  private String sanitizeOrg(final String org) {
    return org.split("\\s+\\(")[0];
  }

  public static void main(String[] args) throws Exception {
    String filename = "/fileListShort.txt";
    if(args.length > 0)
      filename = args[0];
    BufferedReader r = new BufferedReader(new FileReader(ROOT + "/" + filename));
    String line;
    while((line = r.readLine()) != null) {
      TPOOL.execute(new NYTLoader(new File(ROOT, line)));
    }
    TPOOL.shutdown();
    TPOOL.awaitTermination(Integer.MAX_VALUE, TimeUnit.MINUTES);
    r.close();
  }
}
