#include <boost/graph/graph_traits.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
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
using namespace boost::numeric::ublas;
using namespace boost;
using namespace std;


 typedef property<edge_weight_t, int> EdgeWeightProperty;
 typedef boost::adjacency_list<listS, vecS, directedS, no_property,
 EdgeWeightProperty > mygraph;
 int main() 
{ 
    mygraph g;
     add_edge (0, 1, 8, g);
     add_edge (0, 3, 18, g);
     add_edge (1, 2, 20, g);
     add_edge (2, 3, 2, g);
     add_edge (3, 1, 1, g);
     add_edge (1, 3, 7, g);
     cout<<out_degree(0, g)<<endl;

property_map<mygraph, edge_weight_t>::type
      weight = get(edge_weight, g);
    cout << "weight[(u,v)] = " << get(weight, pair<int, int>(0, 1)) << endl;
  
 } 
