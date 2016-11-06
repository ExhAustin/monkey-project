#include <cstdlib>
#include <iostream>
#include <utility> // std::pair

using namespace std;

// 2-D grid structure
class Grid{
public:
	int width, height;
	pair<int,int> inlet, outlet;
	int score;

	Grid(){};
	void init(int,int,pair<int,int>,pair<int,int>);
	void init(const Grid&);
	char get(int x, int y) const{/*if(x<0||x>=height||y<0||y>=width) cout<<"FUCK"<<endl;*/ return tiles[x][y];};
	void set(int x, int y, char c){/*if(x<0||x>=height||y<0||y>=width) cout<<"FUCK"<<endl;*/ tiles[x][y] = c;};
	bool operator<(const Grid&);

protected:
	vector< vector<char> > tiles;

private:
	Grid(const Grid& G){init(G);};	// copy constructor
	Grid& operator=(const Grid&);	// assignment operator
};

// Path is just another grid with a specified front node(leaf), parent, and liquid type(color)
class Path: public Grid{
public:
	char color;
	int parent;
	pair<int,int> leaf;

	Path(){};
	void init(const Grid&);
	void init(const Path&);

private:
	Path(const Path& P){init(P);};	// copy constructor
	Path& operator=(const Path&);	// assignment operator
};

//----------------Grid member functions
// Initialize
void Grid::init(int w, int h, pair<int,int> i, pair<int,int> o){
	width = w;
	height = h;
	inlet = i;
	outlet = o;

	score = 0;

	tiles.resize(height);
	for(int row=0; row<height; row++){
		tiles[row].resize(width);
	}
}

// Initialize with assignment
void Grid::init(const Grid& G){
	width = G.width;
	height = G.height;
	inlet = G.inlet;
	outlet = G.outlet;

	score = G.score;

	tiles.resize(height);
	for(int i=0; i<height; i++){
		tiles[i].resize(width);
		for(int j=0; j<width; j++){
			set(i,j, G.get(i,j));
		}
	}
}

// Assignment operator
Grid& Grid::operator=(const Grid& G){
	width = G.width;
	height = G.height;
	inlet = G.inlet;
	outlet = G.outlet;

	score = G.score;

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			set(i,j, G.get(i,j));
		}
	}
}

// Score comparison
bool Grid::operator<(const Grid& G){
	return score < G.score;
}

//----------------Path member functions
// Initialize with assignment (from Grid)
void Path::init(const Grid& P){
	width = P.width;
	height = P.height;
	inlet = P.inlet;
	outlet = P.outlet;

	score = P.score;

	tiles.resize(height);
	for(int i=0; i<height; i++){
		tiles[i].resize(width);
		for(int j=0; j<width; j++){
			set(i,j, 'u');
		}
	}

	leaf = inlet;
}

// Initialize with assignment
void Path::init(const Path& P){
	width = P.width;
	height = P.height;
	inlet = P.inlet;
	outlet = P.outlet;

	score = P.score;

	tiles.resize(height);
	for(int i=0; i<height; i++){
		tiles[i].resize(width);
		for(int j=0; j<width; j++){
			set(i,j, P.get(i,j));
		}
	}

	leaf = P.leaf;
	color = P.color;
}

// Assignment operator
Path& Path::operator=(const Path& P){
	width = P.width;
	height = P.height;
	inlet = P.inlet;
	outlet = P.outlet;

	score = P.score;

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			set(i,j, P.get(i,j));
		}
	}

	leaf = P.leaf;
	color = P.color;
}
