//-------------------------
// Grid -
//		0: liquid type 1
//		1: liquid type 2
//		x: don't care
//		T: don't touch
//
// Path - 
//		u: unexplored
//		e: explored
//-------------------------
#include <cstdlib>
#include <iostream>
#include <vector>
#include <queue>	// std::queue
#include <stack>	// std::stack
#include <algorithm> // std::sort

#include "gridpath_vec.h"

#define INFINITY 1e9

using namespace std;

vector<Path*>* optimalFill(const Grid*,int);
void pathDFS(vector<Path*>*,int,const Grid*,int);		// DFS path search
void solutionPrint(const Grid*, const vector<Path*>*);	// print optimal solution
int pathScore(const Path*, const Grid*);	// score for a path (higher == better)
void pathUpdate(const Path*, Grid*);		// updates a grid by tracing a path through it ('1' => 'x' or '0' => 'x')
bool pathEqual(const Path* P_ptr1, const Path* P_ptr2, const Grid* G_ptr);	// checks if two paths are equal in effect
void gridPrint(const Grid*);	// prints graphical representation of a grid
void pathPrint(const Path*);	// prints graphical representation of a path
Grid* gridPtrGen();		// allocates a Grid and returns a pointer to it
Path* pathPtrGen(); 	// allocates a Path and returns a pointer to it

struct compclass{
  bool operator() (Grid* G_ptr1, Grid* G_ptr2) { return G_ptr1->score < G_ptr2->score;}
} mycomp;

int main(){
	int height, width;
	pair<int,int> inlet, outlet;
	char** objective_arr;
	Grid objective;
	int K;
	vector<Path*>* solution_vec_ptr;

	// read input data
	cin >> height;
	cin >> width;
	cin >> inlet.first >> inlet.second;
	cin >> outlet.first >> outlet.second;

	objective_arr = new char*[height];
	for(int i=0; i<height; i++){
		objective_arr[i] = new char[width];

		for(int j=0; j<width; j++){
			cin >> objective_arr[i][j];
		}
	}

	cin >> K;

	// initialize objective grid from input data
	objective.init(height, width, inlet, outlet);
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			objective.set(i, j, objective_arr[i][j]);
		}
	}

	// run optimization
	solution_vec_ptr = optimalFill(&objective, K);

	// print solution
	solutionPrint(&objective, solution_vec_ptr);

	// return memory and terminate
	for(int i=0; i<height; i++){
		delete objective_arr[i];
	}
	delete objective_arr;

	for(int i=0; i<solution_vec_ptr->size(); i++){
		delete solution_vec_ptr->at(i);
	}
	delete solution_vec_ptr;

	return 0;
}

vector<Path*>* optimalFill(const Grid* objective_ptr, int K){
	int height, width;

	Grid *current_grid_ptr, *temp_grid_ptr;
	Path *temp_path_ptr;
	vector<Grid*> current_grid_vec, next_grid_vec;

	vector<Path*> path_pqueue;
	vector<vector<Path*>*> history_vec;

	vector<Path*>* solution_vec_ptr = new vector<Path*>;

	int n_injects = 0;
	int parent_num;
	char temp_char;
	bool completed;

	// initialize
	height = objective_ptr->height;
	width = objective_ptr->width;

	temp_grid_ptr = gridPtrGen();
	temp_grid_ptr->init(*objective_ptr);
	current_grid_vec.push_back(temp_grid_ptr);

	// iteration
	while(1){
		// process grids from previous iteration
		for(int g=0; g<current_grid_vec.size(); g++){
			current_grid_ptr = current_grid_vec[g];

			// search for paths reaching outlet, and place K highest scoring paths into path_pqueue
			cout << "path_pqueue.size() = " << path_pqueue.size() << endl;
			cout << "DFS..." << endl; //debug
			pathDFS(&path_pqueue, K, current_grid_ptr, g);
			cout << "DFS done." << endl; //debug
		}

		// increment number of injections
		n_injects += 1;

		// check if any paths are found
		if( path_pqueue.empty() ){
			cout << "Cannot find solution. Either the problem sucks or this program sucks. Or both." << endl;
			break;
		}

		// add resulting paths to history
		vector<Path*>* temp_path_vec_ptr = new vector<Path*>;

		sort(path_pqueue.begin(), path_pqueue.end(), mycomp);

		for(int i=0; i<path_pqueue.size(); i++){
			temp_path_vec_ptr->push_back(path_pqueue.at(i));
		}
		path_pqueue.clear();

		history_vec.push_back(temp_path_vec_ptr);

		// prepare next grid vector
		for(int p=0; p<temp_path_vec_ptr->size(); p++){
			parent_num = temp_path_vec_ptr->at(p)->parent;

			temp_grid_ptr = gridPtrGen();
			temp_grid_ptr->init( *current_grid_vec[parent_num] );
			pathUpdate(temp_path_vec_ptr->at(p), temp_grid_ptr);
			gridPrint(temp_grid_ptr); //debug

			next_grid_vec.push_back( temp_grid_ptr );
		}

		// clear currrent grid vector
		for(int g=0; g<current_grid_vec.size(); g++){
			delete current_grid_vec[g];
		}
		current_grid_vec.clear();

		// swap current and next grid vector
		current_grid_vec.swap(next_grid_vec);

		// check if completed
		for(int g=current_grid_vec.size()-1; g>=0; g--){
			completed = true;

			for(int i=0; i<height; i++){
				for(int j=0; j<height; j++){
					temp_char = current_grid_vec[g]->get(i,j);
					if(temp_char == '1' || temp_char == '0'){
						completed = false;
						break;
					}
				}

				if(completed == false){
					break;
				}
			}

			if(completed == true){
				parent_num = g;
				for(int i=history_vec.size()-1; i>=0; i--){
					temp_path_ptr = pathPtrGen();
					temp_path_ptr->init(*(history_vec[i]->at(parent_num)));
					solution_vec_ptr->push_back(temp_path_ptr);

					parent_num = history_vec[i]->at(parent_num)->parent;
				}

				break;
			}
		}

		if(completed == true){
			break;
		}
	} // end of entire iteration loop

	// clean vectors
	for(int g=0; g<current_grid_vec.size(); g++){
		delete current_grid_vec[g];
	}

	for(int i=0; i<history_vec.size(); i++){
		for(int p=0; p<history_vec.at(i)->size(); p++){
			delete history_vec[i]->at(p);
		}
		delete history_vec[i];
	}

	// return solution
	return solution_vec_ptr;
}

// DFS path search
//	(Begins with *current_grid_ptr, find every possible path, and place them into *path_pqueue_ptr)
//	(Only keeps the K best paths in *path_pqueue_ptr)
void pathDFS(vector<Path*>* path_pqueue_ptr, int K, const Grid* current_grid_ptr, int parent_num){
	Path *current_path_ptr, *temp_path_ptr;
	stack<Path*> dfs_path_stack;

	int height = current_grid_ptr->height;
	int width = current_grid_ptr->width;
	pair<int,int> inlet = current_grid_ptr->inlet;
	pair<int,int> outlet = current_grid_ptr->outlet;

	int i_next, j_next;
	int direction[4] = {1,0,-1,0};
	int path_score;
	int min_score, i_min;
	bool duplicate;
	char inlet_color, temp_char;
	char color[2];

	// choose liquid type
	inlet_color = current_grid_ptr->get(inlet.first, inlet.second);
	if( inlet_color == '0' ){
		color[0] = color[1] = '0';
	}else if(inlet_color == '1'){
		color[0] = color[1] = '1';
	}else if(inlet_color == 'x'){
		color[0] = '0';
		color[1] = '1';
	}else if(inlet_color == 'T'){
		return;
	}

	// inject liquid
	for(int c=0; c<2; c++){
		if(color[0] == color[1] && c == 1){ // break second loop if only one color is valid
			break;
		}

		// initial path
		temp_path_ptr = pathPtrGen();
		temp_path_ptr->init(*current_grid_ptr);
		temp_path_ptr->color = color[c];
		temp_path_ptr->set(inlet.first, inlet.second, 'e');

		dfs_path_stack.push(temp_path_ptr);

		// DFS
		while( !dfs_path_stack.empty() ){
			// pop element
			current_path_ptr = dfs_path_stack.top();
			dfs_path_stack.pop();
	
			// check if outlet is reached
			if(current_path_ptr->leaf == outlet){
				path_score = pathScore(current_path_ptr, current_grid_ptr);

				// not a success if nothing is achieved
				if(path_score <= 0){
					delete current_path_ptr;
					continue;
				}

				current_path_ptr->score = current_grid_ptr->score + path_score;
				current_path_ptr->parent = parent_num;

				// preprocess path_pqueue
				duplicate = false;
				min_score = INFINITY;

				for(int i=0; i<path_pqueue_ptr->size(); i++){
					temp_path_ptr = path_pqueue_ptr->at(i);

					// record min of pqueue
					if( temp_path_ptr->score < min_score ){
						min_score = temp_path_ptr->score;
						i_min = i;
					}

					// check for duplicates (paths with same overall effects)
					if(temp_path_ptr->parent == parent_num){
						if( pathEqual(temp_path_ptr, current_path_ptr, current_grid_ptr) ){
							duplicate = true;

							if( temp_path_ptr->score < current_path_ptr->score ){
								path_pqueue_ptr->at(i) = current_path_ptr;
								delete temp_path_ptr;
							}else{
								delete current_path_ptr;
							}

							break;
						}
					}
				}

				// push into path priority queue
				if( !duplicate ){
					if( path_pqueue_ptr->size() >= K ){
						if(current_path_ptr->score > path_pqueue_ptr->at(i_min)->score){
							delete path_pqueue_ptr->at(i_min);
							path_pqueue_ptr->at(i_min) = current_path_ptr;
						}else if(current_path_ptr->score < path_pqueue_ptr->at(i_min)->score){
							delete current_path_ptr;
						}else{
							//path_pqueue_ptr->push_back(current_path_ptr);
						}
					}else{
						path_pqueue_ptr->push_back(current_path_ptr);
					}
				}

				continue;
			}

			// explore
			for(int t=0; t<4; t++){
				i_next = current_path_ptr->leaf.first + direction[t];
				j_next = current_path_ptr->leaf.second + direction[(t+3)%4];
	
				// expand to next tile if valid
				if( i_next >= 0 && i_next < height && j_next >= 0 && j_next < width ){ // inside grid
					if(current_path_ptr->get(i_next,j_next) == 'u'){ // unexplored
						temp_char = current_grid_ptr->get(i_next, j_next);
						if( temp_char == color[c] || temp_char == 'x'){ // required liquid == path liquid || don't care
							temp_path_ptr = pathPtrGen();
							temp_path_ptr->init(*current_path_ptr);

							temp_path_ptr->set(i_next, j_next, 'e');
							temp_path_ptr->leaf.first = i_next;
							temp_path_ptr->leaf.second = j_next;

							dfs_path_stack.push(temp_path_ptr);
						}
					}
				}
			}

			// delete current path
			delete current_path_ptr;
		} // end of DFS while loop
	} // end of liquid color for loop
}

// print optimal solution
void solutionPrint(const Grid* objective_ptr, const vector<Path*>* solution_vec_ptr){
	if( solution_vec_ptr->empty() ){
		cout << "Cannot find solution. Either the problem sucks or this program sucks. Or both." << endl;
	}else{	// solution exists
		cout << "Optimal result:" << endl;
		
		cout << "number of injections =  " << solution_vec_ptr->size() << endl;

		cout << "desired configuration = " << endl;
		gridPrint(objective_ptr);

		cout << "injection paths = " << endl;
		for(int i=0; i<solution_vec_ptr->size(); i++){
			pathPrint(solution_vec_ptr->at(i));
		}
	}
}

// score for a path (higher == better)
int pathScore(const Path* P_ptr, const Grid* G_ptr){
	char temp;
	int gridsize = P_ptr->height * P_ptr->width;
	int score = 0;

	for(int i=0; i<P_ptr->height; i++){
		for(int j=0; j<P_ptr->width; j++){
			if(P_ptr->get(i,j) == 'e'){
				//== define score here ==
				temp = G_ptr->get(i,j);
				if(temp == 'x'){
					score -= 1;
				}else if(temp == P_ptr->color){
					score += gridsize;
				}else{
					score -= gridsize*gridsize;
				}
				//== ==
			}
		}
	}

	return score;
}

// updates a grid by tracing a path through it ('1' => 'x' or '0' => 'x')
void pathUpdate(const Path* P_ptr, Grid* G_ptr){
	for(int i=0; i<P_ptr->height; i++){
		for(int j=0; j<P_ptr->width; j++){
			if(P_ptr->get(i,j) == 'e'){
				G_ptr->set(i,j,'x');
			}
		}
	}

	G_ptr->score = P_ptr->score;
}

// checks if two paths are equal in effect
bool pathEqual(const Path* P_ptr1, const Path* P_ptr2, const Grid* G_ptr){
	bool equal = true;

	for(int i=0; i<G_ptr->height; i++){
		for(int j=0; j<G_ptr->width; j++){
			if( P_ptr1->get(i,j) != P_ptr2->get(i,j) && G_ptr->get(i,j) != 'x' ){
				equal = false;
			}
		}
	}

	return equal;
}

// prints graphical representation of a grid
void gridPrint(const Grid* A_ptr){
	for(int i=0; i<A_ptr->height; i++){
		for(int j=0; j<A_ptr->width; j++){
			cout << A_ptr->get(i,j) << " ";
		}
		cout << endl;
	}
}

// prints graphical representation of a path
void pathPrint(const Path* A_ptr){
	for(int i=0; i<A_ptr->height; i++){
		for(int j=0; j<A_ptr->width; j++){
			if(A_ptr->get(i,j) == 'e'){
				cout << A_ptr->color << " ";
			}else{
				cout << "." << " ";
			}
		}
		cout << endl;
	}

	cout << endl;
}

Grid* gridPtrGen(){
	Grid* G = new Grid;
	return G;
}

Path* pathPtrGen(){
	Path* P = new Path;
	return P;
}
