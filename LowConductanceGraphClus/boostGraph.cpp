
#include "projectheader.h"



double findConductance(const Graph& g, map<int,int> nodes, int& outS, int& totalVol){

	int deltaS = 0; 
	int volS = 0;
	
	graph_traits<Graph>::adjacency_iterator ai;
	graph_traits<Graph>::adjacency_iterator ai_end;
	
	map <int, int> :: const_iterator iter;
	
	for(map<int,int>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{		
		// for the new node, find how many edges go to in-S nodes (E1) and how many to out-S nodes (E2)
		tie(ai, ai_end) = adjacent_vertices(it->first, g);
		for (; ai != ai_end; ++ai){
			// if neighbor is not in "nodes", increase deltaS (outgoing edge!)
			iter = nodes.find(*ai);
			if ( iter == nodes.end() ){
				deltaS++;
			}
		}
		volS = volS + out_degree(it->first,g);	
    }
	outS = deltaS;
	totalVol = volS;
	//cout << " (" << deltaS << "/" << volS << ")" << endl;
	return ((double)deltaS/(double)volS);

}

double findSparsity(const Graph& g, map<int,int> nodes, int& outS, int& totalVol){

	int volS = 0;
	
	graph_traits<Graph>::adjacency_iterator ai;
	graph_traits<Graph>::adjacency_iterator ai_end;
	
	set<int> nset;
	
	map <int, int> :: const_iterator iter;
	
	for(map<int,int>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{		
		// for the new node, find how many edges go to in-S nodes (E1) and how many to out-S nodes (E2)
		tie(ai, ai_end) = adjacent_vertices(it->first, g);
		for (; ai != ai_end; ++ai){
			// if neighbor is not in "nodes", add to nset (out-bound neighbor!)
			iter = nodes.find(*ai);
			if ( iter == nodes.end() ){
				nset.insert(*ai);
			}
		}
		volS = volS + out_degree(it->first,g);	
    }
	outS = nset.size();
	totalVol = volS;
	//cout << " (" << deltaS << "/" << volS << ")" << endl;
	return ((double)outS/(double)volS);

}

void printadjacentnodes(const Graph& g, int v){
	// print the adjacent vertices to vertex v
	graph_traits<Graph>::adjacency_iterator ai;
    graph_traits<Graph>::adjacency_iterator ai_end;
	
	tie(ai, ai_end) = adjacent_vertices(v, g);
	
	//graph_traits<Graph>::degree_size_type d_v;
	int d_v = out_degree(v,g); 
	
	cout << "Vertex " << v << " has " <<  d_v << " neighbors\n";
    std::cout << "adjacent vertices " << v << ": ";
    for (; ai != ai_end; ++ai)
        std::cout << *ai <<  " ";		
	std::cout << std::endl;

}

int findLocalID(const Graph& g, int globalID){
	// print vertices of the graph
	int N = num_vertices(g);
	for (int i=0; i<N; i++){		
		if(g[i].globalID == globalID)
			return i;
	}
	return -1;
}

void printglobalnodes(const Graph& g){
	// print vertices of the graph
	int N = num_vertices(g);
	for (int i=0; i<N; i++){
		cout << g[i].globalID << " ";
	}cout <<endl;

}

void printnodes(const Graph& g){
	// print vertices of the graph
	graph_traits<Graph>::vertex_iterator iter_t;
	graph_traits<Graph>::vertex_iterator iter_t2;
	for (tie(iter_t,iter_t2) = vertices(g); iter_t != iter_t2; ++iter_t){
		cout << *iter_t << " ";
	}cout <<endl;

}



void expandCluster(map<int, int>& vertices, Graph& g){
	
	
	
	map <int, int> :: const_iterator neigh, isinrest, lastelement;
	graph_traits<Graph>::adjacency_iterator ai, a_end, bi, b_end;
    
	int Ssize = (int) vertices.size();
	int Gsize = Ssize;
	
	map<int,int> copy_list = vertices; // modify "vertices" with degree-1 neighbor pull-in
	// copy_list holds the vertices in subgraph G discovered so far!
	
	int marked_neighs;
	for(map<int,int>::const_iterator it = vertices.begin(); it != vertices.end(); ++it){
		
		//cout << "vertex in S " << it->first << endl;
		
		
		tie(ai, a_end) = adjacent_vertices(it->first, g);
		
		for (; ai != a_end; ++ai) {
			//cout << "neighbor " << *ai << endl;
			
			// find if neighbor is in G
			neigh = copy_list.find(*ai);
			if(neigh == copy_list.end()){ // not in G
				Gsize++;
				copy_list[*ai] = Gsize;
				
				
				///*---------------- pull-in section -------------------------
				// if not-belong to any cluster --i.e. in rest, if not continue
				if(g[*ai].belong2cluster > 0)
					continue;
					
				// if all neighbors belong to a cluster --PULL IN to S!!! == add to vertices
				// key in vertices is globalID, value is orderID
				marked_neighs = 0;
				tie(bi, b_end) = adjacent_vertices(*ai, g);
				for (; bi != b_end; ++bi) {
					if(g[*bi].belong2cluster>0)
						marked_neighs++;
				}
				if ((signed)out_degree(*ai,g) == marked_neighs){ //--pull in!
				 
					Ssize++;
					vertices[*ai] = Ssize;
					
					if(DEBUG){
						cout << "Pulling in a node with degree " << out_degree(*ai, g) << " New size: " << Ssize << endl;
					}
					
					// MARK IT, do NOT BOTHER if it belongs to another cluster --probably not!
					g[*ai].belong2cluster++;
					
				}
				//------------------------------------------------------------------*/
			}
	
			if(Ssize > 10000)
				return;
			//getchar();
		}
		
	}
}

void getSubgraph2(Graph& sg, const map<int, int>& vertices, const Graph& g, bool includeOutbound){
	
	
	
	map <int, int> :: const_iterator neigh, isinrest, lastelement;
	graph_traits<Graph>::adjacency_iterator ai, a_end, bi, b_end;
    
	int Ssize = (int) vertices.size();
	int Gsize = Ssize; //subgraph-size
	map<int,int> copy_list = vertices; // modify "vertices" with degree-1 neighbor pull-in
	// copy_list holds the vertices in subgraph G discovered so far!
	
	int d;
	for(map<int,int>::const_iterator it = vertices.begin(); it != vertices.end(); ++it){
		
		//cout << "vertex in S " << it->first << endl;
		//d = out_degree(it->first,g);

		// find all the neighbors of v and add them to the sg
		tie(ai, a_end) = adjacent_vertices(it->first, g);		
		for (; ai != a_end; ++ai) {
			//cout << "neighbor " << *ai << endl;
			
			// find if neighbor is in G
			neigh = copy_list.find(*ai);
			if(neigh == copy_list.end() ){ // not in G
				if(includeOutbound){
					Gsize++;
					copy_list[*ai] = Gsize;
					//cout << "NOT in S! adding edge " << it->second << "," << Gsize <<endl;				
					add_edge(it->second, Gsize, sg);
					sg[it->second].globalID = it->second; 
					sg[Gsize].globalID = *ai;
				}
				/*
				else{ // there cannot be any vertex in REST with all neighbors out of REST --those are pulled-in!!!
					graph_traits<Graph>::vertex_descriptor u;
					u = add_vertex(sg);
					sg[u].globalID = it->first; // just add the vertex			
				}
				*/
			}
			else{ // in G --just add the edge OR not in G but outbound=false
				//cout << "in S! adding edge " << it->second << "," << neigh->second <<endl;
				if(it->second < neigh->second){
					add_edge(it->second, neigh->second, sg);
					sg[it->second].globalID = it->second; 
					sg[neigh->second].globalID = neigh->second;
				}
			}
			
			//cout << num_vertices(sg)-1 << " nodes in sg so far" << endl;
			//getchar();
		}
		
	}
	// add singletons
	graph_traits<Graph>::vertex_descriptor u;
	for(map<int,int>::const_iterator it = vertices.begin(); it != vertices.end(); ++it){
		d = out_degree(it->first,g);
		if(d==0){
			cout << "zero degree" << endl;
			try{ // if id is in between it is already added
				sg[it->second].globalID = it->second;
			}
			catch( char *str ){ // if not just add to the end of the id range
				u = add_vertex(sg);
				sg[u].globalID = it->second;
			}
		}
	}
	//printnodes(sg);
	if(Ssize>0)
		sg[0].globalID = 0;
	//cout << "Returning from subgraph.. " << endl;
	

}



void getSubgraph(Graph& sg, const map<int, int>& vertices, const Graph& g, bool includeOutbound){
	
	
	
	map <int, int> :: const_iterator neigh, isinrest, lastelement;
	graph_traits<Graph>::adjacency_iterator ai, a_end, bi, b_end;
    
	int Ssize = (int) vertices.size();
	int Gsize = Ssize; //subgraph-size
	map<int,int> copy_list = vertices; // modify "vertices" with degree-1 neighbor pull-in
	// copy_list holds the vertices in subgraph G discovered so far!
	
	int d;
	for(map<int,int>::const_iterator it = vertices.begin(); it != vertices.end(); ++it){
		
		//cout << "vertex in S " << it->first << endl;
		//d = out_degree(it->first,g);

		// find all the neighbors of v and add them to the sg
		tie(ai, a_end) = adjacent_vertices(it->first, g);		
		for (; ai != a_end; ++ai) {
			//cout << "neighbor " << *ai << endl;
			
			// find if neighbor is in G
			neigh = copy_list.find(*ai);
			if(neigh == copy_list.end() ){ // not in G
				if(includeOutbound){
					Gsize++;
					copy_list[*ai] = Gsize;
					//cout << "NOT in S! adding edge " << it->second << "," << Gsize <<endl;				
					add_edge(it->second, Gsize, sg);
					sg[it->second].globalID = it->first; 
					sg[Gsize].globalID = *ai;
				}
				/*
				else{ // there cannot be any vertex in REST with all neighbors out of REST --those are pulled-in!!!
					graph_traits<Graph>::vertex_descriptor u;
					u = add_vertex(sg);
					sg[u].globalID = it->first; // just add the vertex			
				}
				*/
			}
			else{ // in G --just add the edge OR not in G but outbound=false
				//cout << "in S! adding edge " << it->second << "," << neigh->second <<endl;
				if(it->second < neigh->second){
					add_edge(it->second, neigh->second, sg);
					sg[it->second].globalID = it->first; 
					sg[neigh->second].globalID = neigh->first;
				}
			}
			
			//cout << num_vertices(sg)-1 << " nodes in sg so far" << endl;
			//getchar();
		}
		
	}
	// add singletons
	graph_traits<Graph>::vertex_descriptor u;
	for(map<int,int>::const_iterator it = vertices.begin(); it != vertices.end(); ++it){
		d = out_degree(it->first,g);
		if(d==0){
			cout << "zero degree" << endl;
			try{ // if id is in between it is already added
				sg[it->second].globalID = it->first;
			}
			catch( char *str ){ // if not just add to the end of the id range
				u = add_vertex(sg);
				sg[u].globalID = it->first;
			}
		}
	}
	//printnodes(sg);
	if(Ssize>0)
		sg[0].globalID = 0;
	//cout << "Returning from subgraph.. " << endl;
	

}

int weighted_out_degree(int vertex, Graph& g) {
  return g[vertex].weightedDegree;
}

Graph createGraph(const char* filename, int readType)
{

	Graph g;
	
	FILE *input= fopen(filename, "r+");
    map<pair<int,int>, int> edges;

	map<pair<int,int>, int>::iterator it;
	int E = 0;
	
	if (input==NULL) {
		perror ("Error opening file");
    exit(1);
  }
	else{	
		int source, dest, w, time;
		
	  float weight = 0;
			
		while(feof(input)== 0){	
			E++;

      fscanf(input,"%d%d%f\n", &source, &dest, &weight);
      w = (int)(weight * 1000);
		  add_edge(source, dest,  g);	
                  edges[pair<int,int>(source,dest)] = w;
		}
	}
	
	fclose(input);
	cout << endl << "There are " <<  E << " edges in g" << endl;
		
	int N = num_vertices(g); //adds vertex 0 for some reason!?
	cout << endl << "There are " << N-1 << " vertices in g\n" << endl;

    for(it = edges.begin(); it != edges.end(); ++it){
       g[(it->first).first].weightedDegree += it->second;
       g[(it->first).second].weightedDegree += it->second;;
    }
	
	for (int i=0; i<N; i++){
		g[i].globalID = i;
		g[i].belong2cluster = 0;
    cout<<i<<" : "<<g[i].weightedDegree<<endl;
	}
	return g;
}


/*
int main()
{
	Graph g = createGraph();
	
	// print the adjacent vertices to vertex 5
	std::cout << "adjacent vertices 5: ";
    graph_traits<Graph>::adjacency_iterator ai;
    graph_traits<Graph>::adjacency_iterator ai_end;
    for (tie(ai, ai_end) = adjacent_vertices(5, g); ai != ai_end; ++ai)
        std::cout << *ai <<  " ";		
	std::cout << std::endl;
	////////////////////////////////////
	
	
	return 0;
}
*/

