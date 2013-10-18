#include "projectheader.h"

void storeMap( const map<int,int> & hub_pi, int ID, string filename ){

stringstream ss;
ss << filename << "_" << ID;
ofstream file;			
file.open ( (ss.str()).c_str() );

// store as key \t value
for(map<int,int>::const_iterator it = hub_pi.begin(); it != hub_pi.end(); ++it)
{
	file << it->first << "\n"; //write only global ID
}
file.close();


}

void storeMap2( const map<int,int> & hub_pi, const map<int,int> & big, int ID, string filename ){

stringstream ss;
ss << filename << "_" << ID;
ofstream file;			
file.open ( (ss.str()).c_str() );

// store as key \t value
for(map<int,int>::const_iterator it = hub_pi.begin(); it != hub_pi.end(); ++it)
{
	// it->first is local id in big
	// global id is big[it->first]
	int localsg = it->first;
	map<int,int>::const_iterator iter = big.find(localsg);

	file << iter->second << "\t" << it->second << "\n";
}
file.close();



}


void loadMap( map<int,int> & hub_pi, int ID, string filename ){


stringstream ss;
ss << filename << "_" << ID;
ifstream file;			
file.open ( (ss.str()).c_str() );
if (!file) {
	cout << "There was a problem opening file " << endl;
	return;
}

// load key \t value
int key, value;
while (file >> key) {
	file >> value;
	hub_pi[key] = value;
}

file.close();



}

void loadMap2( map<int,int> & hub_pi, int ID, string filename ){


stringstream ss;
ss << filename << "_" << ID;
ifstream file;			
file.open ( (ss.str()).c_str() );
if (!file) {
	cout << "There was a problem opening file " << endl;
	return;
}

// load key \t value
int key, value;
while (file >> key) {
	file >> value;
	hub_pi[value] = key;
}

file.close();



}






