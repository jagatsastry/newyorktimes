#include "projectheader.h"
#include<cstdlib>

void storeMap( const map<int,int> & hub_pi, int ID, string filename );


int main(int argc, char* argv[])
{
	
	

	string filename(argv[1]); 
  cout<<filename<<endl;
//	cout << "Enter filename in dataset/: " << endl;	
//	cin>>filename;
	
	int k = atoi(argv[2]); // =199;
  cout<<k<<endl;
		
	int t =4;
	//cout << "Enter file type: " << endl;	
	//cin>>t;
	
  cout<<"Reading graph"<<endl;	
	Graph g = createGraph(filename.c_str(), t);
  cout<<"Done reading graph"<<endl;	
	
	//printnodes(g); getchar();
	int N = num_vertices(g)-1; 	
	//int E = num_edges(g);
	//cout << E << endl;
	
	
//	cout << "Enter number of seed nodes k: " << endl;		
//	cin>>k;
	
	string DIRECTORY("clusters");
	//cout << "Enter output folder name: " << endl;	
	//cin>>DIRECTORY;
	
	
	
	time_t initall;
	time(&initall);
	
	time_t init;
	time(&init);
	
		
	int total_nodes_covered = 0;
	
	
	
	int hub;
	//int *list = new int[k]; //already chosen distinct hubs whose clusters are dropped --k of them are allowed!
	map<int,int> *hub_p = new map<int,int>[k+2]; //+2 --+1 for GCC-rest, +1 for rest-rest
	
	
	// compute approximate PPV for all the chosen hubs
	int clusterSize;
	double conductance;
	int* clusterSizes = new int[k+2];
	double* conductances = new double[k+2];
	
	map<int, int> clstr;
	int i = 0;
	
	int numDroppedClusters = 0;

	
	// add all vertices to an unordered set
	set<int> vertices;
	int numsingletons = 0;
    for (int i=1; i<=N; i++){
		if(out_degree(i,g) == 0){
			numsingletons++;
		}
		vertices.insert(i);
	}
	
	cout << endl << "There are " << numsingletons << " singletons \n" << endl;
	
	set<int>::iterator theIterator;
	int hubind;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////// find clusters /////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while ( i < k ){
	
		// pick a random NODE 
		
		clock_t inithub, finalhub;
		if(DEBUG){
		inithub=clock();
		}
		
		//edgeiter = edges(g).first;
		srand((unsigned)time(NULL));
		hubind = rand() % vertices.size();
		
		theIterator = vertices.begin();
		for( int it=0; it<hubind; it++  ) {
			theIterator++;
		}			
		hub = *theIterator;
		
		
		if(DEBUG){
		finalhub=clock()-inithub;
		cout << "Finding a new hub took " << ((double)finalhub / ((double)CLOCKS_PER_SEC)) << " seconds" << endl;		
		cout << "Hub#" << i+1 << " is " << hub << " with out-degree " << out_degree(hub,g) << endl;
		}
		
		
		
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////// ----------------------------------- ////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		float* p = new float[N+1];
		PageRankNibble2( p, g, N, hub, alpha, 0.000001 );		
		
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////// ----------------------------------- ////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MINIMUM conductance cluster around hub -- last 3 parameters below are auxiliary
		clstr = findMinConductanceClusterS(g, p, N, i+1, clusterSize, conductance); // lets make this hold the returned set of nodes (cluster) with the 
		
		
		///////////////////////////ALSO TRIED//////////////////////////////////////////////////////
		// SMALL conductance around hub 
		// clstr = findSmallConductanceLargeClusterS(g, p, N, i+1, clusterSize, conductance);
		// minimum NODE-sparsity around hub
		// clstr = findMinSparsityClusterS(g, p, N, i+1, clusterSize, conductance);
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// see if cluster size is reasonably large
		if(clusterSize > MAXSZ || clusterSize < MINSZ){		// 
		
			//list[numDroppedClusters] = hub;
			try{
				vertices.erase(hub);
			}catch( char *str )    {
				//cout << "Caught some other exception: " << str << "\n";
			}
			
			if(DEBUG){
			cout << "Cluster size is " << clusterSize << " so DROP cluster!" << endl << endl;	
			}
		
		}
		else{		
			// See if non-overlapping part is large enough		
			int nonoverlapsize = 0;
			for(map<int,int>::const_iterator it = clstr.begin(); it != clstr.end(); ++it){
				if(!g[it->first].belong2cluster){
					nonoverlapsize++;				
				}
			}
			
			double o = (double)(clusterSize-nonoverlapsize)/clusterSize;
			if(o < OVERLAPF){ // if overlap < overlapf, accept!
				clusterSizes[i] = clusterSize;
				conductances[i] = conductance;
				hub_p[i] = clstr;
				
				for(map<int,int>::const_iterator it = clstr.begin(); it != clstr.end(); ++it){
					g[it->first].belong2cluster = true;
					
					try{
						vertices.erase(it->first);
					}catch( char *str )    {
						//cout << "Caught some other exception: " << str << "\n";
					}

				}
				
				i++;
				total_nodes_covered += nonoverlapsize;
				
				if(DEBUG){
				cout << "Overlap is " << o <<" (" << clusterSize-nonoverlapsize << ") " << " so ACCEPT cluster!" << endl;	
				cout << "Nodes covered so far " << total_nodes_covered << " / " << N << endl << endl;
				}
			}
			else{
				
				//list[numDroppedClusters] = hub;
				try{
					vertices.erase(hub);
				}catch( char *str )    {
					//cout << "Caught some other exception: " << str << "\n";
				}
				numDroppedClusters++;
				
				if(DEBUG){
				cout << "Overlap is " << o << " so DROP cluster " << numDroppedClusters << "!" << endl << endl;	
				}
			}

		}
		
		delete[] p;
		//     
		if( numDroppedClusters == k || (N-total_nodes_covered) <= (MAXSZ+numsingletons) || (int)vertices.size() == 0){ 
		//if too many clusters are dropped or #uncovered is small enough or no new vertices to select as a hub 
			k = i;
			break;
		}
		
	}
	//delete[] list;
	
	if(DEBUG){
	cout << "Found cluster count is " << k << endl << endl;
	cout << endl << endl << numDroppedClusters << " clusters are dropped due to large overlap!" << endl;
	}
	/////////////////////////////////////// clusters all-set! ///////////////////////////////////////////////////////
	
	
	
	
	//////////////////// OOOoooOoooOooo000o00oOOoOOo0oOo MERGE 00oo00000OOOOOOooOOOoOOOOooOoOO //////////////
	int finalClusterCount;
	mergeClusters(hub_p, k, finalClusterCount);
	k = finalClusterCount; // is the new k
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	cout << endl;
	int* outSs = new int[k+2];
	int* totalVols = new int[k+2];
	int outS, totalVol;
	double finalConductance;
	map<int,int> x, y;
	map <int, int> :: const_iterator iter;
	
	
		
	
	//////////////////////////////////////////////////////////////////////////////
	// -------------------------- find subgraphs ------------------------------ //
	//////////////////////////////////////////////////////////////////////////////	
	Graph *subgraphs = new Graph[k+2];	
	for (int j=0; j<k; j++){
		expandCluster(hub_p[j], g); //hub_p[j] now changed via pulling-in "degree 1" nodes
		Graph sg;
		getSubgraph(sg, hub_p[j], g, true);		
		subgraphs[j] = sg;		
	}	
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	
	//  ----------------------- after expansion -------------------------------
	if(DEBUG){
	cout << endl;
	}
	
	for (int i=0; i<k; i++)
	{
		x = hub_p[i];
		// compute and print conductance
		finalConductance = findConductance(g, x, outS, totalVol);
		//finalConductance = findSparsity(g, x, outS, totalVol);

		
		
		//if(DEBUG){
		cout << "Cluster-"<<i+1<<" size is " << (int) x.size() 
		<< " with conductance " <<  finalConductance
		<< " (" << outS << "/" << totalVol << ")" << endl;
		//}
		clusterSizes[i] = (int) x.size();
		conductances[i] = finalConductance;
		outSs[i] = outS;
		totalVols[i] = totalVol;
	}	
	//////////////////////////////////////////////////////////////////////	
	// how about unassigned vertices??? --> put them in a separate cluster!
	//////////////////////////////////////////////////////////////////////
	map<int,int> restinit;	
	int count = 0;
	for (int i=1; i<=N; i++) {
		if(!g[i].belong2cluster){
			count++;
			restinit[i] = count;
		}
	}	
	
	
	///////////////////////////////////////////////////////////////////////////////////
	// little experiment: compute connected components and sizes in rest
	// obtain rest-subgraph induced only on the rest nodes (no out-bound nodes)
	Graph sg;
	getSubgraph(sg, restinit, g, false);
	int numnodes = (signed)num_vertices(sg);	

	if(DEBUG){	
	cout << "Rest-INIT size is " << (int) restinit.size() << " and #nodes in graph is " << numnodes-1 << endl;
	}
	
	// call boost connected-components function
	std::vector<int> c(num_vertices(sg));
	int num = connected_components(sg, make_iterator_property_map(c.begin(), get(vertex_index, sg)));
	
	if(DEBUG){
	cout << endl << "Number of components in rest is " << num << endl;
	}
	
	int* csizes = new int[num];
	for(int i=0; i<num; i++)
		csizes[i] = 0;
	for(std::vector<int>::iterator it=c.begin(); it!=c.end(); ++it)
		csizes[*it]++;	
	
	int gccSize = 0, gccid = 0;
	if(DEBUG){
	cout << endl << "Component sizes are:" << endl;
	}
	for(int i=0; i<num; i++){
		if(DEBUG){
		cout << "Component-" << i+1 << ": " << csizes[i] << endl;
		}
		if(csizes[i] > gccSize){
			gccSize = csizes[i];
			gccid = i;
		}
	}
	delete [] csizes;
	
	
	if(DEBUG){
	cout << endl;
	}
	
	map<int,int> gccrest;	
	count=0;
	int vertex = 0;
	for(std::vector<int>::iterator it=c.begin(); it!=c.end(); ++it){
		if(*it == gccid){
			count++;
			gccrest[sg[vertex].globalID] = count;
			g[sg[vertex].globalID].belong2cluster = true;
		}
		vertex++;	
	}
	
	map<int,int> rest;	
	count = 0;
	for (int i=1; i<=N; i++) {
		if(!g[i].belong2cluster){
			count++;
			rest[i] = count;
		}
	}	
	
	
	
	/////////////////////////////////////////////////////////////////////////////////////
	
	finalConductance = findConductance(g, gccrest, outS, totalVol);
	
	//if(DEBUG){
	cout << "Cluster-"<<k+1<<" size is " << (int) gccrest.size() 
	<< " with conductance " <<  finalConductance
	<< " (" << outS << "/" << totalVol << ")" << endl;
	//}	
	clusterSizes[k] = (int) gccrest.size();
	conductances[k] = finalConductance;
	outSs[k] = outS;
	totalVols[k] = totalVol;
	
	Graph sg2;
	getSubgraph(sg2, gccrest, g, true);	// IMPORTANT
	subgraphs[k] = sg2; // IMPORTANT
	hub_p[k] = gccrest;
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	k=k+1; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	finalConductance = findConductance(g, rest, outS, totalVol);
	//finalConductance = findSparsity(g, rest, outS, totalVol);
	
	//if(DEBUG){
	cout << "Cluster-"<<k+1<<" size is " << (int) rest.size() 
	<< " with conductance " <<  finalConductance
	<< " (" << outS << "/" << totalVol << ")" << endl;
	//}
	clusterSizes[k] = (int) rest.size();
	conductances[k] = finalConductance;
	outSs[k] = outS;
	totalVols[k] = totalVol;
	
	Graph sg3;
	getSubgraph(sg3, rest, g, true);	// IMPORTANT
	subgraphs[k] = sg3; // IMPORTANT
	hub_p[k] = rest;
	/////////////////////////////////////////////////////////////////////////////////////
	
	
	
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	
	// load hub_p_k's
	
	stringstream ss;
	
	// STORE HUB_P MAPS
	ss.str("");
    ss << DIRECTORY << "/cluster";
	for (int c=0; c<k+1; c++) {	
		storeMap( hub_p[c], c+1, ss.str() );	
	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	
	
	
	
	
	
	return 0;
}
