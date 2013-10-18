
#include "projectheader.h"



float* PageRankNibbleDir( const DirGraph& g, int N, int v, double alpha, double epsilon )
{
	//1 - initialize the p and r 1xN vectors
	float* p = new float[N+1];
	float* r = new float[N+1];
	float* r_copy = new float[N+1];
	//initialize to 0
	for (int i=0; i<N+1; i++) {
		p[i] = 0; 
		r[i] = 0;
		r_copy[i] = 0;
    }
	
	r[v] = 1;
	r_copy[v] = 1;
	
	int front = v;
	
	
	graph_traits<DirGraph>::adjacency_iterator ai;
    graph_traits<DirGraph>::adjacency_iterator a_end;
	//double lazy_p = (1-alpha);
	
	int iter = 0;
	double totaldiff = 0;
	int move;
	
	while( true ) {
	
		iter++;
		
		move = 0;
		while (out_degree(front,g)==0 || r[front]/out_degree(front,g) < epsilon){
		
			// keep moving the pointer			
			front = (front+1) % (N+1);
			move++;
			//cout << "#moves= " << move << " front: " << front << endl;
			if(move == N+1){ // if moved over all r
				front = -1;
				break;
			}
		}
		if(front==-1){
			// passed over -- compare with old
				


			totaldiff = 0;
			// update r
			double summ = 0;
			for(int i=0; i<N+1; i++){
				r[i] = r[i] + p[i];
				p[i] = 0;
				summ+=r[i];
				totaldiff += abs(r_copy[i] - r[i]);
			}
			
			//cout << totaldiff << " " << summ << endl; getchar();
			if(totaldiff < 0.000001)
				break;
			
			
			// else --copy r to r_copy and repeat
			for(int i=0; i<N+1; i++){
				r_copy[i] = r[i];
			}			
		
			front =0;
			continue;
		}
		//	break;
		//cout << "Front is " << front << " with out-degree " << out_degree(front,g) << endl; getchar();
		
		int d_front = out_degree(front,g);
		
		
		
		// find neighbors of front
		tie(ai, a_end) = adjacent_vertices(front, g);
		
		//cout << "processing neighbors..." << endl; 
		int num_adj_vertices = 0;
		bool found;
		
		graph_traits<DirGraph>::edge_descriptor e;
		double probsum = 0;
		double prob, diff;
		for (; ai != a_end; ++ai) {
			num_adj_vertices++;
			
			tie(e, found) = edge(front, *ai, g);
			if(!found)
				continue;
			prob = get(edge_weight_t(), g, e);
			probsum+=prob;

			diff = prob*r[front];
			
			//cout << "Prob:" << prob << " r[front]: " << r[front] << " Diff: " << diff << endl; 
			
			p[*ai] = p[*ai] + diff;
			
		}
		r[front] = 0;
		//cout << "Prob sum=" << probsum << endl; 
		
		if(d_front != num_adj_vertices){
			cout << "trouble in directed graph..." << endl;
			cout << d_front << " !=? " << num_adj_vertices << endl; 
			
		}	
		
		//cout << "Out-degree: " << d_front << " Num-adj-vertices: " << num_adj_vertices << endl;getchar();	
		
	}
	
	cout << "Convergence took " << iter << " number of iterations." << endl << endl;
	delete[] p;
	delete[] r_copy;
	return r;


}




void PageRankNibble2( float* p, const Graph& g, int N, int v, double alpha, double epsilon )
{

	clock_t init, final;
	if(DEBUG){
	init=clock();
	}
	// here we do not use a list any more --whose size can get huge, 
	// we just warp-around-scan the r vector

	//cout << "approx PPV entered..." << endl; 

	// N: number of nodes in the graph
	// v: the query node
	// alpha: restart probability
	// epsilon: error/residual
	
	//1 - initialize the p and r 1xN vectors
	//float* p = new float[N+1];
	float* r = new float[N+1];
	
	//cout << "initialization done!" << endl;getchar();

	//initialize to 0
	for (int i=0; i<N+1; i++) {
		p[i] = 0; r[i] = 0;
    }
	
	r[v] = 1;
	
	int front = v; //initial pointer position
	

	graph_traits<Graph>::adjacency_iterator ai;
    graph_traits<Graph>::adjacency_iterator a_end;
	double lazy_p = (1-alpha)/2;
	
	int iter = 0;
	int move; //move pointer
	while( true ) {
	
		iter++;
		
			
		
		move = 0;
		while (out_degree(front,g)==0 || r[front]/out_degree(front,g) < epsilon){
		
			// keep moving the pointer			
			front = (front+1) % (N+1);
			//cout << move << " " << front << " " << out_degree(front,g) << endl;
			move++;
			//cout << "#moves= " << move << " front: " << front << endl;
			if(move == N+1){ // if moved over all r
				front = -1;
				break;
			}
		}
		if(front == -1)
			break;
		

		int d_front = out_degree(front,g);
		
		
		float r_front = r[front];
		p[front] = p[front] + alpha * r_front;
		r[front] = lazy_p * r_front;
		
		
		// find neighbors of front
		tie(ai, a_end) = adjacent_vertices(front, g);
		
		//cout << "processing neighbors..." << endl; 
		
		for (; ai != a_end; ++ai) {
			r[*ai] = r[*ai] + (lazy_p/d_front)*r_front;
		}
		
		//cout << iter << " DONE........." << endl;
	}
	if(DEBUG){
	final=clock()-init;
	cout << "PageRank computation took " << (double)final / ((double)CLOCKS_PER_SEC) << " seconds" << endl;
	}
	//cout << "Convergence took " << iter << " number of iterations." << endl;
	
	delete[] r;
	


}




void swap(float* v, int* index, int i, int j)
{
float f;
f = v[i];
v[i] = v[j];
v[j] = f;

int t;
t = index[i];
index[i] = index[j];
index[j] = t;
}

//Function to determine the partitions
// partitions the array and returns the middle subscript
int partition(float* array, int* index, int top, int bottom)
{
     float x = array[top];
	 
	 int i = top-1;
     int j = bottom + 1;
     do
     {
           do     
           {
				j--;
           }while (x >array[j]);

          do  
         {
                i++;
          } while (x <array[i]);

          if (i < j)
         { 
                 swap(array, index, i,j);
				 
         }
     }while (i < j);    
     return j;           // returns middle subscript 
}

void quickSort(float* array, int* index, int top, int bottom, int N)
{
	// top = subscript of beginning of array
    // bottom = subscript of end of array
     int middle;
     if (top < bottom)
    {
          middle = partition(array, index, top, bottom);
		
		  quickSort(array, index, top, middle, N);   // sort first section
          quickSort(array, index, middle+1, bottom, N);    // sort second section
     }
     return;
}


int MyLinearSearch(int a[], int first, int last, int key) {
   // function:
   //   Searches a[first]..a[last] for key.  
   // returns: index of the matching element if it finds key, 
   //         otherwise  -1.
   // parameters:
   //   a           in  array of (possibly unsorted) values.
   //   first, last in  lower and upper subscript bounds
   //   key         in  value to search for.
   // returns:
   //   index of key, or -1 if key is not in the array.
   
   for (int i=first; i<=last; i++) {
       if (key == a[i]) {
          return i;
       }
   }
   return -1;    // failed to find key
}

ivector findTopKNeighbors(const dvector& ppv, int N, int R, int topk, const Graph& g, int localR, ivector& topk_rankcount){
	int* index = new int[N];
	float *p = new float[N];
	// initialize index to all sequential and
	// degree-normalize the p vector
	
	for (int i=0; i<N; i++) {
		index[i] = i+1;
		p[i] = ppv(i);
		/* can return degree-normalized top-k if we want
		int d = out_degree(i,g);
		if(d>0)
			p[i] = (float)(p[i]/d);
		else
			p[i] = 0;
		*/
	}
	
	quickSort(p, index, 0, N-1, N);
	
	
	ivector topnodes(N);
	
	int count = 0;
	int distinctcount = 0;

	float prevp = -1;
	int samenum = 0;
	for(int i=0; i<N; i++){
		// if index[i] is already a neighbor of R, continue
		// isNeighbor(g, localR, findLocalID(g, index[i])) || 
		if(index[i] == R)
			continue;
		
		if(p[i] == 0){
			topk_rankcount(distinctcount) = samenum;
			break;			
		}
		
		if(p[i] == prevp || count == 0){
			samenum++;
			topnodes(count) = index[i];
			count++;
			prevp = p[i]; //for count == 0
		}
		else{
			topk_rankcount(distinctcount) = samenum;
			distinctcount++;
			if(distinctcount == topk)
				break;
			samenum = 1;			
			prevp = p[i];
			
			topnodes(count) = index[i];
			count++;
		}		
		
	}
	
	ivector topknodes(count);
	for(int i=0; i<count; i++)
		topknodes(i) = topnodes(i);
	
	return topknodes;

}

// find lowest conductance cluster among sweeps
map<int,int> findMinConductanceClusterS(const Graph& g, float p[], int N, int id, int& size, double& conductance){

	
	int* index = new int[N+1];
	// initialize index to all sequential and
	// degree-normalize the p vector
	index[0] = 0;
	for (int i=1; i<N+1; i++) {
		index[i] = i;
		int d = out_degree(i,g);
		if(d>0)
			p[i] = (double)(p[i]/d);
		else
			p[i] = 0;
	}


	quickSort(p, index, 0, N, N);
	//printVec<float>(p, N+1); getchar();
	
	
	//////////// ---------------------------------- /////////////
	int topk = 0; // CONSIDER top-k closest neighbors in the same S as seed node
	for (int i=0; i<N+1; i++) {		
		if(p[i] < pCutThreshold)
		{
			topk = i-1;
			break;
		}
    }
	if(DEBUG){
	cout << "Top-k is " << topk+1 << endl; 
	}
	//getchar();
	/////////////////////////////////////////////////////////////
	
	
	//printVec<float>(p, 20); getchar();
	//printVec<int>(index, 20); getchar();

	double* conductances = new double[topk+1];
	conductances[0] = 1; // if only 1 node included in S, vol(S) = delta(S)
	
	
	double deltaS = out_degree(index[0],g); 
	double volS = deltaS;
	//cout << "initial deltaS " << deltaS << " for node " << index[1] << endl;
	
	
	graph_traits<Graph>::adjacency_iterator ai;
	graph_traits<Graph>::adjacency_iterator ai_end;
	int found = 0;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// ------------------ compute conductance for top k --------------------------- ///////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	for (int i=1; i<=topk; i++) { //was i=1
		// for the new node index[2], find how many edges go to in-S nodes (E1) and how many to out-S nodes (E2)
		tie(ai, ai_end) = adjacent_vertices(index[i], g);
		for (; ai != ai_end; ++ai){
			//if(*ai == index[i])
			//	cout << "self-loop!" << endl;
			found = MyLinearSearch(index, 0, i, *ai);
			if(found==-1) //not a neighbor in-S
				deltaS++;
			else
				deltaS--;	
			//cout << "neighbor is " << *ai << " deltaS: " << deltaS << " found: " << found << endl;
		}
		volS = volS + out_degree(index[i],g);
		
		conductances[i] = (double)(deltaS/volS);
		//cout << "conductance when first " << i+1 << " nodes are considered: " << conductances[i] << endl;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/* write sweep-order conductance - p value - globalID ---
	ofstream myfile;
	string f = "beforecut/hub_";
	string l = ".txt";
	cout << to_string<int>(id) << endl;
	f += to_string<int>(id);
	f += l;
	char *a=new char[f.size()+1];
	a[f.size()]=0;
	memcpy(a,f.c_str(),f.size());
	myfile.open ( a );
	for (int i=0; i<topk+1; i++)
	{
		myfile << conductances[i] << " " << p[i] << " " << index[i] <<'\n';
	}
	myfile.close();
	*/

	//printVec<double>(conductances, N); getchar();
	
	//cut at the minimum conductance around the node index[0]
	int firstNminC = minIndex<double>(conductances,topk+1); //old -->func. minIndex assures minimum but not zero!--
	if(DEBUG){
	cout << "Cut at first-" << firstNminC+1 << " elements with conductance " << conductances[firstNminC] << endl; 
	}
	//getchar();
	
	
	map<int,int> x;		
	
	size = firstNminC+1;
	conductance = conductances[firstNminC];
	
	//cout << index[N] << endl; getchar();
	
	
	for (int i=0; i<=firstNminC; i++) {
		int temp = index[i];
		temp = g[temp].globalID;
		//cout << "#" <<i+1 << " id: " << temp << endl;
		//x[index[i]] = i; 
		x.insert(make_pair(temp, i+1)); // key is the node id !
		
	}	
	
	delete[] index;
	delete[] conductances;
	return x;

}



// find small conductance (with at most 0.1 conductance diff with min) cluster among sweeps
map<int,int> findSmallConductanceLargeClusterS(const Graph& g, float p[], int N, int id, int& size, double& conductance){

	
	int* index = new int[N+1];
	// initialize index to all sequential and
	// degree-normalize the p vector
	index[0] = 0;
	for (int i=1; i<N+1; i++) {
		index[i] = i;
		int d = out_degree(i,g);
		if(d>0)
			p[i] = (double)(p[i]/d);
		else
			p[i] = 0;
	}


	quickSort(p, index, 0, N, N);
	//printVec<float>(p, N+1); getchar();
	
	
	//////////// ---------------------------------- /////////////
	int topk = 0; // CONSIDER top-k closest neighbors in the same S as seed node
	for (int i=0; i<N+1; i++) {		
		if(p[i] < pCutThreshold)
		{
			topk = i-1;
			break;
		}
    }
	cout << "Top-k is " << topk+1 << endl; 
	//getchar();
	/////////////////////////////////////////////////////////////
	
	
	//printVec<float>(p, 20); getchar();
	//printVec<int>(index, 20); getchar();

	double* conductances = new double[topk+1];
	conductances[0] = 1; // if only 1 node included in S, vol(S) = delta(S)
	
	double deltaS = out_degree(index[0],g); 
	double volS = deltaS;
	//cout << "initial deltaS " << deltaS << " for node " << index[1] << endl;
	
	
	graph_traits<Graph>::adjacency_iterator ai;
	graph_traits<Graph>::adjacency_iterator ai_end;
	int found = 0;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// ------------------ compute conductance for top k --------------------------- ///////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	for (int i=1; i<=topk; i++) {
		// for the new node index[2], find how many edges go to in-S nodes (E1) and how many to out-S nodes (E2)
		tie(ai, ai_end) = adjacent_vertices(index[i], g);
		for (; ai != ai_end; ++ai){
			//if(*ai == index[i])
			//	cout << "self-loop!" << endl;
			found = MyLinearSearch(index, 0, i, *ai);
			if(found==-1) //not a neighbor in-S
				deltaS++;
			else
				deltaS--;	
			//cout << "neighbor is " << *ai << " deltaS: " << deltaS << " found: " << found << endl;
		}
		volS = volS + out_degree(index[i],g);
		
		conductances[i] = (double)(deltaS/volS);
		//cout << "conductance when first " << i+1 << " nodes are considered: " << conductances[i] << endl;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////////

	//printVec<double>(conductances, N); getchar();
	
	//find the MINIMUM conductance around the node index[0]
	int firstNminC = minIndex<double>(conductances,topk+1); //func. minIndex assures minimum but not zero!
	cout << "There are " << firstNminC+1 << " elements with MIN conductance " << conductances[firstNminC] << endl; 
	
	
	//cut at SMALL (~MINIMUM --second minimum or |MINIMUM-SMALL| < 0.1) conductance around the node index[0] with LARGER cluster
	int firstNsmallC = smallIndex<double>(conductances,topk+1); //func. minIndex assures minimum but not zero!
	cout << "There are " << firstNsmallC+1 << " elements with SMALL conductance " << conductances[firstNsmallC] << endl; 
	
	
	map<int,int> x;		
	
	size = firstNsmallC+1;
	conductance = conductances[firstNsmallC];	
	//cout << index[N] << endl; getchar();
	
	for (int i=0; i<=firstNsmallC; i++) {
		int temp = index[i];
		//cout << "#" <<i+1 << " id: " << temp << endl;
		//x[index[i]] = i; 
		x.insert(make_pair(temp, i+1)); // key is the node id !
		
	}	
	return x;



}




// find minimum sparsity cluster among sweeps
map<int,int> findMinSparsityClusterS(const Graph& g, float p[], int N, int id, int& size, double& conductance){

	
	int* index = new int[N+1];
	// initialize index to all sequential and
	// degree-normalize the p vector
	index[0] = 0;
	for (int i=1; i<N+1; i++) {
		index[i] = i;
		int d = out_degree(i,g);
		if(d>0)
			p[i] = (double)(p[i]/d);
		else
			p[i] = 0;
	}


	quickSort(p, index, 0, N, N);
	//printVec<float>(p, N+1); getchar();
	
	
	//////////// ---------------------------------- /////////////
	int topk = 0; // CONSIDER top-k closest neighbors in the same S as seed node
	for (int i=0; i<N+1; i++) {		
		if(p[i] < pCutThreshold)
		{
			topk = i-1;
			break;
		}
    }
	cout << "Top-k is " << topk+1 << endl; 
	//getchar();
	/////////////////////////////////////////////////////////////
	
	
	//printVec<float>(p, 20); getchar();
	//printVec<int>(index, 20); getchar();

	double* sparsities = new double[topk+1];
	sparsities[0] = 1; // if only 1 node included in S, vol(S) = delta(S) --here, delta(S) is the number of out-bound nodes!
	
	double deltaS = out_degree(index[0],g); 
	double volS = deltaS;
	
	// create an unordered set (hash table), and add all the initial outbound-nodes
	set<int> nset;

	graph_traits<Graph>::adjacency_iterator ai;
    graph_traits<Graph>::adjacency_iterator ai_end;
	
	tie(ai, ai_end) = adjacent_vertices(index[0], g);
	for (; ai != ai_end; ++ai)
        nset.insert(*ai);		
	
	//cout << "initial deltaS " << deltaS << " for node " << index[1] << endl;
	
	
	int found;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// ------------------ compute conductance for top k --------------------------- ///////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	for (int i=1; i<=topk; i++) {
		// for the new node index[1], first find if it was in nset, if so remove it
		set<int>::iterator pos = nset.find(index[i]);
		if(pos != nset.end()){ // it is there --remove, because in-S now
			nset.erase(pos);
		}
		
		// next add all neighbors BUT the ones in S-already! --that is, add only forward-link neighbors
		tie(ai, ai_end) = adjacent_vertices(index[i], g);
		for (; ai != ai_end; ++ai){
			found = MyLinearSearch(index, 0, i, *ai);
			if(found==-1) {//not a neighbor in-S
				// add to nset, if not there (set class takes care of that)
				nset.insert(*ai);
			}
			//else
				// do nothing	
			//cout << "neighbor is " << *ai << " deltaS: " << deltaS << " found: " << found << endl;
		}
		volS = volS + out_degree(index[i],g);
		
		sparsities[i] = (double)(nset.size()/volS);
		//cout << "sparsity when first " << i+1 << " nodes are considered: " << sparsities[i] << endl;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/* write sweep-order conductance - p value - globalID ---
	ofstream myfile;
	string f = "beforecut/hub_";
	string l = ".txt";
	cout << to_string<int>(id) << endl;
	f += to_string<int>(id);
	f += l;
	char *a=new char[f.size()+1];
	a[f.size()]=0;
	memcpy(a,f.c_str(),f.size());
	myfile.open ( a );
	for (int i=0; i<topk+1; i++)
	{
		myfile << sparsities[i] << " " << p[i] << " " << index[i] <<'\n';
	}
	myfile.close();
	*/

	//printVec<double>(sparsities, N); getchar();
	
	//cut at the minimum conductance around the node index[0]
	int firstNminC = minIndex<double>(sparsities,topk+1); //func. minIndex assures minimum but not zero!
	cout << "Cut at first-" << firstNminC+1 << " elements with sparsity " << sparsities[firstNminC] << endl; 
	//getchar();
	
	
		
	
	size = firstNminC+1;
	conductance = sparsities[firstNminC];
	
	//cout << index[N] << endl; getchar();
	
	map<int,int> x;	
	for (int i=0; i<=firstNminC; i++) {
		int temp = index[i];
		//cout << "#" <<i+1 << " id: " << temp << endl;
		//x[index[i]] = i; 
		x.insert(make_pair(temp, i+1)); // key is the node id !
		
	}	
	return x;

}








void mergeClusters(map<int,int> *hub_p, int k, int& finalClusterCount){

	map<int,int> x, y;
	map <int, int> :: const_iterator iter;
	
	finalClusterCount = k;
	
	bool *isMerged = new bool[k];
	for (int i=0; i<k; i++) {
		isMerged[i] = false;
	}
	
	
	
	int initSize, curSize, overlapSize;
	for (int i=0; i<k; i++){
	
		if(isMerged[i])
			continue;
		
		
		x = hub_p[i];
		
		int itnum = 0;
		do{
			itnum++;
			
			
			if(DEBUG){
			if(itnum > 1)
				cout << "Round-" << itnum << " for cluster " << i+1 << endl;
			}	
		
			// find overlap with rest of the clusters
			for (int j=0; j<k; j++){
			
				initSize = (int) x.size();
				curSize = initSize;
				
				if(isMerged[j] || i==j)
					continue;
					
					
				
					
				// if valid cluster: 
				///////////////   ----- find amount of overlap -----  /////////////////
				y = hub_p[j];
				overlapSize = 0;
				for(map<int,int>::const_iterator it = x.begin(); it != x.end(); ++it)
				{
					iter = y.find(it->first);
					if ( iter != y.end() ){ // overlap node!!
						overlapSize++;
					}
				}
				//////////////////////////////////////////////////////////////////////
				double cover_ratio = (double) overlapSize / min((int) x.size(), (int) y.size());
				
				
				if( cover_ratio > OVERLAPF){ //if overlap > overlapf, merge!
				
					if(DEBUG){
					cout << "Cover-ratio (" << i+1 << "," << j+1 << ") == " << cover_ratio << endl;
					}
					// merge y INTO x cluster
					for(map<int,int>::const_iterator it = y.begin(); it != y.end(); ++it)
					{
						iter = x.find(it->first);
						if ( iter == x.end() ){ // not in X --put in
							curSize++; 
							x[it->first] = curSize; 
						}
					}
					if(DEBUG){
					cout << "Old size of x: " << initSize << " and Current size: " << curSize << endl;
					}
					
					isMerged[j] = true;
					finalClusterCount--;
					
					
				}		
			}	
			
		} while (curSize != initSize);
		
		hub_p[i] = x; // when no more merge, update cluster
		
	}
	if(DEBUG){
	cout << endl << "Final cluster count is " << finalClusterCount << endl;
	}
	
	for (int i=0; i<k; i++){
		if( !isMerged[i] ){
			// find a place before if exists, else leave it there
			for (int j=0; j<i; j++){
				if(isMerged[j]){
					hub_p[j] = hub_p[i];
					isMerged[j] = false;
					isMerged[i] = true; // free existing space
					break;
				}
			}		
		}	
	}
	
	delete[] isMerged;


}

/*
void mergeClustersReloaded(Graph& g, map<int,int> *hub_p, int k, int& finalClusterCount){

	map<int,int> x, y;
	map <int, int> :: const_iterator iter;
	
	finalClusterCount = k;
	
	bool *isMerged = new bool[k];
	for (int i=0; i<k; i++) {
		isMerged[i] = false;
	}
	
	
	
	int initSize, curSize, overlapSize;
	for (int i=0; i<k; i++){
	
		if(isMerged[i])
			continue;
		
		
		x = hub_p[i];
		
		int itnum = 0;
		do{
			itnum++;
			
			
			if(DEBUG){
			if(itnum > 1)
				cout << "Round-" << itnum << " for cluster " << i+1 << endl;
			}	
		
			// find overlap with rest of the clusters
			///////////////   ----- find amount of overlap -----  /////////////////
			overlapSize = 0;
			for(map<int,int>::const_iterator it = x.begin(); it != x.end(); ++it)
			{
				if ( g[it->first].belong2cluster > 1 ){ // overlap node!!
					overlapSize++;
				}
			}
			double cover_ratio = (double) overlapSize / (int) x.size();
			
			if( cover_ratio > OVERLAPF){ //if overlap > overlapf, merge! ---new: merge with (and only with) the first-encounter overlapping cluster 
			
			initSize = (int) x.size();
			curSize = initSize;
			for (int j=0; j<k; j++){
							
				
				if(isMerged[j] || i==j)
					continue;
							
				
					
				// if valid cluster: 
				///////////////   ----- find IF any overlap -----  /////////////////
				y = hub_p[j];
				overlapSize = 0;
				for(map<int,int>::const_iterator it = x.begin(); it != x.end(); ++it)
				{
					iter = y.find(it->first);
					if ( iter != y.end() ){ // overlap node!!
						overlapSize++;
						break;
					}
				}
				//////////////////////////////////////////////////////////////////////
				if(overlapSize == 0) // can be 0 or 1
					continue;
				
				//else merge!
				
				// merge y INTO x cluster
				for(map<int,int>::const_iterator it = y.begin(); it != y.end(); ++it)
				{
					iter = x.find(it->first);
					if ( iter == x.end() ){ // not in X --put in
						curSize++; 
						x[it->first] = curSize; 
					}
					else{ //overlap node, reduce count due to merge
						(g[it->first].belong2cluster)--;
					}
				}
				if(DEBUG){
				cout << "Old size of x: " << initSize << " and Current size: " << curSize << endl;
				}
				
				isMerged[j] = true;
				finalClusterCount--;
					
				break;	
						
			}
			} // end if cover>threshold		
			else{
				break;
			}
			
		} while (1);
		
		hub_p[i] = x; // when no more merge, update cluster
		
	}
	if(DEBUG){
	cout << endl << "Final cluster count is " << finalClusterCount << endl;
	}
	
	for (int i=0; i<k; i++){
		if( !isMerged[i] ){
			// find the first place empty from the beginning if exists, else leave it there
			for (int j=0; j<i; j++){
				if(isMerged[j]){
					hub_p[j] = hub_p[i];
					isMerged[j] = false;
					isMerged[i] = true; // free existing space
					break;
				}
			}		
		}	
	}
	
	delete[] isMerged;


}

*/

















