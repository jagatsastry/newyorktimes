// project header
// includes function prototypes

#include <iostream> 
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <deque>
#include <fstream>
#include <cmath>
#include <time.h>
#include <set>
#include <sstream>
#include <float.h>
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <iomanip>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/subgraph.hpp> 
#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

//#include <tr1/unordered_set>
//#include <boost/unordered_set.hpp>



using namespace boost::numeric::ublas;
using namespace boost;	
using namespace std;



struct vertex_info { 
    int globalID; 
	int belong2cluster;
  int weightedDegree;
};

struct edge_info { 
    double weight; 
};


typedef adjacency_list<listS, vecS, undirectedS, vertex_info>  Graph;
typedef adjacency_list<listS, vecS, directedS, vertex_info, property<edge_weight_t, double> > DirGraph;
typedef boost::numeric::ublas::vector<double> dvector;
typedef boost::numeric::ublas::vector<int> ivector;
//typedef std::tr1::unordered_set<int> uset;	
		

// parameters
const double alpha = 0.15;
//const double epsilon = 0.000001;
const double pCutThreshold = 0.0000005;
const double OVERLAPF = 0.50;
const bool DEBUG = true;
const bool SUBSET = true;
//const bool TWOHOPS = true;
const int TOPK = 50;
const int TIMES = 100;
//const string DIRECTORY = "dblp_500K_store+500Kedges/";
//const string DIRECTORY = "livej_store/";
//const string DIRECTORY = "web_notredame_store_reloaded/";
//const string DIRECTORY = "gnutella04_store_reloaded/";
const string DIRECTORY = "clusters/";
//const int MMAX  = 1000;
const int MINSZ = 50;
const int MAXSZ = 250;
// subset algorithms



	
// graph algorithms
Graph createGraph(const char* filename, int readType);
void getSubgraph(Graph& sg, const map<int, int>& vertices, const Graph& g, bool includeOutbound);
void getSubgraph2(Graph& sg, const map<int, int>& vertices, const Graph& g, bool includeOutbound);
void expandCluster(map<int, int>& vertices, Graph& g);
void printnodes(const Graph& g);
void printglobalnodes(const Graph& g);
void printadjacentnodes(const Graph& g, int v);
int findLocalID(const Graph& g, int globalID);
double findConductance(const Graph& g, map<int,int> nodes, int& outS, int& totalVol);
double findSparsity(const Graph& g, map<int,int> nodes, int& outS, int& totalVol);
dvector bruteForcePPV(const Graph& g, int s);
ivector findTopKNeighbors(const dvector& ppv, int N, int R, int topk, const Graph& g, int localR, ivector& topk_rankcount);
int rwr( matrix<double>& M, dvector& v, dvector& vcurr, int N, double alpha);
int MyLinearSearch(int a[], int first, int last, int key);



// our algorithms
double** computeC(const Graph& g, int N);
double** computeE(const Graph& g, int N);
double** computeCgivenR(const Graph& g, int N, int R);
double** computeEgivenR(const Graph& g, int N, int R);



// auxiliary algorithms
void PageRankNibble2( float* p, const Graph& g, int N, int v, double alpha, double epsilon );
float* PageRankNibble( const Graph& g, int N, int v, double alpha, double epsilon ); //THIS DOES RANDOM WALK
float* PageRankNibbleDir( const DirGraph& g, int N, int v, double alpha, double epsilon ); // THIS REGULAR -WITHOUT RESTART!
map<int,int> findMinConductanceClusterS(const Graph& g, float* p, int N, int id, int& size, double& conductance);
map<int,int> findMinSparsityClusterS(const Graph& g, float* p, int N, int id, int& size, double& conductance);
map<int,int> findSmallConductanceLargeClusterS(const Graph& g, float p[], int N, int id, int& size, double& conductance);
void mergeClusters(map<int,int> *hub_p, int k, int& finalClusterCount);
void mergeClustersReloaded(Graph& g, map<int,int> *hub_p, int k, int& finalClusterCount);



template<class T>
bool isNeighbor(const Graph& g, T v, T u){
	// return true if u is a neighbor of v
	
	//get iterator over the neighbors of v
	graph_traits<Graph>::adjacency_iterator ai;
	graph_traits<Graph>::adjacency_iterator ai_end;
	
	tie(ai, ai_end) = adjacent_vertices(v, g);
	for (; ai != ai_end; ++ai){
		if((signed)*ai==u)
			return true;
	}
	return false;
}


template<class T>
bool InvertMatrix(const matrix<T>& input, matrix<T>& inverse)
{
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<T> A(input);

	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0)
		return false;

	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<T> (A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

	return true;
}



template <class T>
void printVec(T a[], int N) {
   
   for (int i=0; i<N; i++) {
		cout << a[i] << " ";
   }
   cout << endl;
   
}


template <class T>
int minIndex(T table[], int size)
{
	double mnmm = table[0];
	int index = 0;
	for(int i=9;i<size;i++)
    {
		if(table[i]<mnmm) // && table[i]>0 --exclude for disconnected components
		  {
			  mnmm=table[i];
			  index=i;
		  }
    }
	return index;
}



template <class T>
int smallIndex(T table[], int size)
{
	double mnmm = table[0];
	int index=0;
	for(int i=1;i<size;i++)
    {
		if(table[i]<mnmm && table[i]>0)
		  {
			  mnmm=table[i];
			  index = i;
		  }
    }

	//mnmm = 1;
	for(int i=size-1;i>index;i--)
    {
		if(abs(table[i]-mnmm)<0.1 && table[i]>0)
		//if(table[i]<mnmm && table[i]>0)
		  {
			  mnmm=table[i];
			  index=i;
			  break;
		  }
    }
	
	
	
	return index;
}





template <class T>
string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}








