// CS 510 - Homework Assignment 1
// Sliding Block Puzzle
// By Greg Matthews : 10/11/17
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string.h>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<queue>
#include<stack>
#include<list>
#include<memory>
#include<time.h>
#include<cmath> 
using namespace std;
ifstream file; //File to open for reading puzzle

// Class Block is used as a object to store each block in the puzzle
class Block {
    public:
        Block(int id, vector<int> moves){
            block_id = id;
            moves_possible = moves;
        }

        bool check_move_valid(int move) { 
            if (find(moves_possible.begin(), moves_possible.end(), move) != moves_possible.end()) return true;
            else return false;
        }

        vector<int> get_moves() {
            return moves_possible;
        }
        
        int get_id() {
            return block_id;
        }

    private:
        int block_id; // Unique block ID for each block
        vector<int> moves_possible; // Lost of possible moves the block can take
};

// Class Move is used as an object to store a move; the direction and the block ID
class Move {
    public:
        Move(int pc, int mv) {
            block_id = pc;
            move = mv;
        }
        
        int get_id() {
            return block_id;
        }
        
        int get_move() {
            return move;
        }
    private: 
        int block_id;
        int move; // 1=up, 2=down, 3=left, 4=right
};

// Class State is used an a holder for the current state of the board, and its list of block pieces
class State{
    public: 
        string line;
        void display() {      // Function displays given SBP to screen
            int width = state[0]; int height = state[1];
            for (int i = 0; i < state.size(); i++) {
                if (i == 2 || (i-2) % width == 0) cout << endl;
                cout << state.at(i) << ", ";   // Display SBP to terminal  
            }
            cout << endl;
        }
        
        void load_puzzle(const char* dir) {   // Load puzzle from file
            file.open(dir, ifstream::in);
            if (!file){
                cout << "Unable to open the file." << endl;
                exit(1);
            }
            while (file >> line) {
                int i;
                stringstream ss(line);
                while (ss >> i) {   // Create vector of SBP values
                    state.push_back(i);
                    if (ss.peek() == ',') ss.ignore();
                }
            }

        }
        // Normalizes given state		
		void normalize() {
            int block_id = 3;
            for(int i=2; i < state.size(); i++) {
                if (state[i] == block_id) block_id++;
                else if (state[i] > block_id) {
                    swap_index(block_id, state[i]);
                    block_id++;
                }
            }
        }

        // Swap index of a block in a state with normalized value and vice versa
        void swap_index(int new_id, int old_id) {
            for(int i=2; i < state.size(); i++) {
                if (state[i] == old_id) state[i] = new_id;
                else if (state[i] == new_id) state[i] = old_id;
            }
        }
            
        vector<int> copy() {  // Copies original state and returns copied state
            vector<int> copied_state;
            for (int i = 0; i < state.size(); i++) {
                copied_state.push_back(state[i]);
            }
            return copied_state;
        }

        bool check_complete(vector<int> s) {     // Checks if SBP has been solved
            
            //for(int k=0; k<s.size();k++) cout << s[k] << endl;
            for(int i=0; i <s.size(); i++) {
                if(s[i] == -1) return false;
            }
            return true;
        }

        vector<int> get_state(){
            return state;
        }

        void set_state(vector<int> given_state) {
            state = given_state;
        }
    private:
        vector<int> state;
};
// Class responsible for all movement generation to state
class Move_Gen {  // 1=up, 2=down, 3=left, 4=right
    public:
        // Find valid block moves given current state and block number
        Block valid_block_moves(const vector<int>& state, int block_id) {
            found_moves.clear();
            valid_moves.clear();
            const vector<int> block_indices = find_block_indices(state, block_id);
            const vector<int> empty_indices = find_block_indices(state, 0);
            const vector<int> goal_indices = find_block_indices(state, -1);
            
            find_moves(block_indices, empty_indices, state[0]);
            
            // Special case for master brick, can move into goal cells
            if (block_id == 2) find_moves(block_indices, goal_indices, state[0]);
           
            // Checking that the given move sets for each block can all be moved in that direction
            for(int i=0; i < found_moves.size(); i++) {
                if ((found_moves[i] == 1 || found_moves[i] == 2) && block_indices.size() > 1) {
                    int count = 0;
                    for(int j=0; j < found_moves.size(); j++) {
                        if (found_moves[j] == found_moves[i]) count++;
                        // Add as valid move if size of block = # of same directions
                        if (count == block_indices.size()) valid_moves.push_back(found_moves[i]);
                    }
                }
                else valid_moves.push_back(found_moves[i]);
            }
            return Block (block_id, valid_moves);
        }
        
        // Find ALL valid block moves given current state and all blocks
        vector<Block> all_valid_block_moves(const vector<int>& state) {
            vector<Block> all_blocks;
            vector<int> block_ids;
            for (int i=2; i < state.size(); i++) {
                // Find all block numbers on the board and store into block_ids
                if ((find(block_ids.begin(), block_ids.end(), state[i]) != state.end()) && (state[i] >= 2)) {
                    block_ids.push_back(state[i]);
                }
            }
            // Find each blocks valid moves and store into Block class with its block_id
            for (int j=0; j < block_ids.size(); j++) {
                int block_id = block_ids[j];
                Block block = valid_block_moves(state, block_id);
                vector<int> moves = block.get_moves();
                if (!moves.empty()) all_blocks.push_back(block);
            }
            return all_blocks; 
        }

        // Performs the specified move onto the given state
        void applyMove(State& current_state, Move& move) {
            vector<int> new_state = current_state.get_state();
            vector<int> block_idx = find_block_indices(new_state, move.get_id()); 
            int board_width = new_state[0];
            int board_height = new_state[1];
            Block block_moves = valid_block_moves(new_state, move.get_id());
            int count_right = 0;
            int check_height = 0;

            if (board_height != check_height) {
                check_height = board_height;
                count_right = 0;
            }

            // Check if given move is valid, if so iterate through block indices and move them
            if(block_moves.check_move_valid(move.get_move())) {
                for(int i=0; i<block_idx.size(); i++){
                    // Move the block to the new position
                    if(move.get_move() == 1) new_state[block_idx[i] - board_width] = move.get_id();
                    else if(move.get_move() == 2) new_state[block_idx[i] + board_width] = move.get_id();
                    else if(move.get_move() == 3) new_state[block_idx[i] - 1] = move.get_id();
                    else if(move.get_move() == 4) {
                        count_right++;
                        new_state[block_idx[i] + 1] = move.get_id();
                    }
                    // Zero out original block position, being careful on larger block sizes
                    if (move.get_move() != 4 || count_right <= 1) new_state[block_idx[i]] = 0;

                }
            }
            // Update current state
            current_state.set_state(new_state);      
        }
            
        // Print move to screen
        void print_move(Move move) {
            cout << "(" << move.get_id() << ", ";
            switch (move.get_move()) {
                case 1: // up
                    cout << "up)"; break;
                case 2: // down
                    cout << "down)"; break;
                case 3: // left
                    cout << "left)"; break;
                case 4: // right
                    cout << "right)"; break;
            }
                cout << endl;
        }
        // Performs specified move onto a cloned state and returns it
        State applyMoveCloning(State& current_state, Move& move) {
            State new_state;
            new_state.set_state(current_state.copy());
            applyMove(new_state, move);
            return new_state;
        }

        // Find all available moves for all of the block indices
        void find_moves(const vector<int>& block_indices, const vector<int>& empty_indices, int width) {
            for (int i=0; i<block_indices.size(); i++) {
                for (int j=0; j<empty_indices.size(); j++) {
                    if (block_indices[i] == (empty_indices[j] + 1)) found_moves.push_back(3);
                    if ((block_indices[i] + 1) == empty_indices[j]) found_moves.push_back(4);
                    else if (block_indices[i] == (empty_indices[j] + width)) found_moves.push_back(1);
                    else if ((block_indices[i] + width) == empty_indices[j]) found_moves.push_back(2);
                }
            }
        }

        // Parse through current state and find index of where block_id is found
        const vector<int> find_block_indices(const vector<int>& state, int block_id){
            vector<int> block_indices;
            for (int i=2; i < state.size(); i++) {
                if (state[i] == block_id){
                    block_indices.push_back(i);
                }
            }
            return block_indices;
        }

    private:
        vector<int> valid_moves;
        vector<int> found_moves;
    };

// Swap index of a block in a state with normalized value and vice versa
void swap_index(int new_id, int old_id, vector<int> state) {
    for(int i=2; i < state.size(); i++) {
        if (state[i] == old_id) state[i] = new_id;
        else if (state[i] == new_id) state[i] = old_id;
    }
}

// Normalizes given state
vector<int> normalize(vector<int> s) {
    int block_id = 3;
    for(int i=2; i < s.size(); i++) {
        if (s[i] == block_id) block_id++;
        else if (s[i] > block_id) {
            swap_index(block_id, s[i], s);
            block_id++;
        }
    }
    return s;
}

// Compares 2 states and outputs true if they are identical
bool state_compare(vector<int> s1, vector<int> s2) {
    //s1 = normalize(s1);
    //s2 = normalize(s2);
    for (int i=0; i < s1.size(); i++) {
        if (s1[i] != s2[i]) return false;
    }
    return true;
}

// Generates a random move from the list of possible moves
Move gen_rand_move(vector<int> s) {
    Move_Gen mv_gen;
    vector<Block> blocks = mv_gen.all_valid_block_moves(s);
    vector<Move> moves;
    vector<int> moves_list;
    // Iterate through all valid block moves
    for (int i=0; i < blocks.size(); i++) {
        moves_list = blocks[i].get_moves();
        // For every move each block can make, construct vector of moves
        for (int j=0; j < moves_list.size(); j++) {
            Move move = Move(blocks[i].get_id(), moves_list[j]);
            moves.push_back(move);
        }
    }
    // Choose random move out of vector of all possible moves
    int choice = rand() % moves.size();
    return moves[choice];
}

// Function randomly slides SBP blocks until goal is reached
void random_walks(State& state, int N) {
    Move_Gen mv_gen;
    // Display the SBP board
    state.display();
    while(N) { 
        // Generate a random move and apply it to current state
        Move move = gen_rand_move(state.get_state());
        mv_gen.applyMove(state, move);
        cout << endl;
        mv_gen.print_move(move);
        cout << endl;
	state.normalize();
        state.display();
        // Check completion
        if (state.check_complete(state.get_state())) {
            cout << "Puzzle Solved." << endl;
            break;
        }
        N--;
    }
}

// Container for State on graph, including the states parent, cost, distance, heuristic, and move
class Vertex {
    public:
        Vertex(State& s) {
            state = s;
            dist = -1; // Represents infinite distance
        }
        int dist;// distance in graph
        int cost; //A star search cost
        int heuristic; //Manhattan distance
        vector<int> move; // contained of move that led to that vertex
        State state; // Current state 
        vector<Vertex> parent; // parent of current vertex
       
        // Computer Manhattan distance from master brick to goal
        void compute_manhattan_distance() {
            Move_Gen mv_gen;
            vector<int>distances;
            vector<int> block_2 = mv_gen.find_block_indices(state.get_state(), 2); 
            vector<int> block_neg1 = mv_gen.find_block_indices(state.get_state(), -1);
            heuristic = 0;
            int width = state.get_state()[0];
            for (int i=0; i<block_2.size(); i++){
                int height_2 = (block_2[i] - 2)/width;
                int width_2 = (block_2[i] - 2 - height_2*width);
                for (int j=0; j<block_neg1.size(); j++) {
                    int height_neg1 = (block_neg1[j] - 2)/width;
                    int width_neg1 = (block_neg1[j] - 2 - height_neg1*width);
                    distances.push_back(abs((height_neg1 - height_2) + abs(width_neg1 - width_2)));
                }
            }

            // Find lowest distance of several block manhattan distances
            if (!distances.empty()) {
                int lowest = distances[0];
                for (int k=0; k<distances.size(); k++){
                    if(distances[k] < lowest) lowest = distances[k];
                }
                heuristic = lowest;
            }
        }

};

// Checks if state has already been discovered
bool is_discovered(Vertex v, vector<Vertex> visited) {
    for (int i=0; i<visited.size(); i++) {
        if (state_compare(v.state.get_state(), visited[i].state.get_state())) {
                return true;
        }
    }
    return false;
}

// Prints the given path of move vectors
void print_path(vector<vector<int> > path) {
    stack<Move> S;
    Move_Gen mv_gen;
    for (int i=0; i<path.size(); i++) {
        Move mv = Move(path[i][0], path[i][1]);
        S.push(mv);
    }
    while(!S.empty()) {
        Move m = S.top();
        S.pop();
        if(!m.get_id() == 0 && !m.get_move() == 0) mv_gen.print_move(m);
    }
}

// Find the root path from goal back to start
vector<vector<int> > find_path(Vertex& start, Vertex& end, vector<vector<int> > path) {

    if (state_compare(start.state.get_state(), end.state.get_state()) || end.dist == 0) {
        path.push_back(end.move);
        return path;
    }
    path.push_back(end.move);
    return find_path(start, end.parent[0], path);
}

// Function uses BFS to traverse SBP using Queue (FIFO)
void BFS(State& state) {
    state.display();
    vector<Vertex> visited; //Container for all visited vertices
    int num_nodes = 0; int length = 0; 
    queue<Vertex> Q;

    Vertex start_v = Vertex(state);
    start_v.parent.push_back(start_v);
    start_v.dist = 0; // Root node has distance 0
    start_v.move.push_back(0); // Default block_id to 0
    start_v.move.push_back(0); // Default move to 0
    visited.push_back(start_v); // Add start vertex as visited
    Q.push(start_v); // Enqueue starting vertex

    clock_t time = clock(); //Start timer
    while (!Q.empty())
    {
        
        Move_Gen mv_gen;
        Vertex u = Q.front(); // Dequeue vertex
        Q.pop();
        num_nodes++;

        // Check if state has reached goal
        if (u.state.check_complete(u.state.get_state())) {
            time = clock() - time;
            
            // Print Path
            vector<vector<int> > move_path;
            move_path = find_path(start_v, u, move_path);
            print_path(move_path);
            length = u.dist;
            
            u.state.display();
            cout << num_nodes << " " << setprecision(3) << (float)time/CLOCKS_PER_SEC << " " << length << endl;
            break;
        }

        vector<Block> blocks = mv_gen.all_valid_block_moves(u.state.get_state());
        vector<Move> moves; //stores array of moves

        // Iterate through all valid block moves
        for (int i=0; i < blocks.size(); i++) {
            vector<int> moves_list = blocks[i].get_moves();
            // For every move each block can make, construct vector of moves
            for (int j=0; j < moves_list.size(); j++) {
                Move move = Move(blocks[i].get_id(), moves_list[j]);
                moves.push_back(move);
            }
        }

        // Create child vertices and enqueue them
        for (int k=0; k < moves.size(); k++){
            State s = mv_gen.applyMoveCloning(u.state, moves[k]);
            
            Vertex v = Vertex(s); // child
            s.normalize();
            Vertex v_norm = Vertex(s); // normalized vertex used for comparing to visited vertices

            v.parent.push_back(u); // make u parent of v
            v.dist  = u.dist + 1;
            v.move.push_back(moves[k].get_id()); //record move that led to that state
            v.move.push_back(moves[k].get_move()); //record move that led to that state

            // Check if v hasn't been visited and check for loops
            if (!is_discovered(v_norm, visited) && !state_compare(v_norm.state.get_state(), normalize(u.parent[0].state.get_state()))) { 
                Q.push(v); 
                visited.push_back(v_norm);
            }
        }
    }
}

// Function uses DFS to traverse SBP using Stack (LIFO)
void DFS(State& state) {
    state.display();
    vector<Vertex> visited; //Container for all visited vertices
    int num_nodes = 0; int length = 0; 
    stack<Vertex> S;

    Vertex start_v = Vertex(state);
    start_v.parent.push_back(start_v);
    start_v.dist = 0; // Root node has distance 0
    start_v.move.push_back(0); // Default block_id to 0
    start_v.move.push_back(0); // Default move to 0
    visited.push_back(start_v); // Add start vertex as visited
    S.push(start_v); // Enqueue starting vertex

    clock_t time = clock(); //Start timer
    while (!S.empty())
    {
        
        Move_Gen mv_gen;
        Vertex u = S.top(); // Pop vertex
        S.pop();
        num_nodes++;

        // Check if state has reached goal
        if (u.state.check_complete(u.state.get_state())) {
            time = clock() - time;
            
            // Print Path
            vector<vector<int> > move_path;
            move_path = find_path(start_v, u, move_path);
            print_path(move_path);
            length = u.dist;
            
            u.state.display();
            cout << num_nodes << " " << setprecision(3) << (float)time/CLOCKS_PER_SEC << " " << length << endl;
            break;
        }

        // Store array of block moves
        vector<Block> blocks = mv_gen.all_valid_block_moves(u.state.get_state());
        vector<Move> moves;

        // Iterate through all valid block moves
        for (int i=0; i < blocks.size(); i++) {
            vector<int> moves_list = blocks[i].get_moves();
            // For every move each block can make, construct vector of moves
            for (int j=0; j < moves_list.size(); j++) {
                Move move = Move(blocks[i].get_id(), moves_list[j]);
                moves.push_back(move);
            }
        }

        // Create child vertices and pop them onto stack
        for (int k=0; k < moves.size(); k++){
            State s = mv_gen.applyMoveCloning(u.state, moves[k]);
            
            Vertex v = Vertex(s); // Child
            s.normalize();
            Vertex v_norm = Vertex(s); // normalized vertex used for comparing to visited vertices

            v.parent.push_back(u); // Make u parent of v
            v.dist  = u.dist + 1; // Increment distance of v
            v.move.push_back(moves[k].get_id()); //record move that led to that state
            v.move.push_back(moves[k].get_move()); //record move that led to that state

            // Check if v hasn't been visited and check for loops
            if (!is_discovered(v_norm, visited) && !state_compare(v_norm.state.get_state(), normalize(u.parent[0].state.get_state()))) { 
                S.push(v); 
                visited.push_back(v_norm);
            }
        }
    }
}

// Comparator for priority queue to sort queue with lowest cost at the front of the queue
struct compare_cost {
    bool operator()(Vertex const & v1, Vertex const & v2) {
    // returns true if v2 is ordered before v1
        return v1.cost > v2.cost;
    }
};

//A star search heuristic using priority queue
void A_Star_Search(State& state) {

    vector<Vertex> visited; //Container for all visited vertices
    priority_queue<Vertex, vector<Vertex>, compare_cost> PQ; //Priority Queue
    int num_nodes = 0;
    int length = 0;
    
    // Initialize first Vertex
    Vertex start_v = Vertex(state);
    start_v.parent.push_back(start_v); //Make start vertex parent of itself
    start_v.dist = 0; // Root node has distance 0
    start_v.move.push_back(0); // Default block_id to 0
    start_v.move.push_back(0); // Default move to 0
    visited.push_back(start_v); // Add start vertex as visited
   
    // Compute manhattan distance and add to PQ
    start_v.compute_manhattan_distance();
    start_v.cost = start_v.heuristic;
    PQ.push(start_v);

    clock_t time = clock(); //Start timer
   
    // Pop vertex from PQ and expand graph of new states
    while(1) {
        Move_Gen mv_gen;
        Vertex u = PQ.top();
        PQ.pop();
        num_nodes++;

        // Check if state has reached goal
        if (u.state.check_complete(u.state.get_state())) {
            time = clock() - time;
            
            // Print Path
            vector<vector<int> > move_path;
            move_path = find_path(start_v, u, move_path);
            print_path(move_path);
            length = u.dist;

            u.state.display();
            cout << num_nodes << " " << setprecision(3) << (float)time/CLOCKS_PER_SEC << " " << length << endl;
            break;
        }
    
        // Store array of block moves
        vector<Block> blocks = mv_gen.all_valid_block_moves(u.state.get_state());
        vector<Move> moves;

        // Iterate through all valid block moves
        for (int i=0; i < blocks.size(); i++) {
            vector<int> moves_list = blocks[i].get_moves();
            // For every move each block can make, construct vector of moves
            for (int j=0; j < moves_list.size(); j++) {
                Move move = Move(blocks[i].get_id(), moves_list[j]);
                moves.push_back(move);
            }
        }

        // Create child vertices and pop them onto stack
        for (int k=0; k < moves.size(); k++){
            State s = mv_gen.applyMoveCloning(u.state, moves[k]);
            
            Vertex v = Vertex(s); // Child
            s.normalize();
            Vertex v_norm = Vertex(s); // normalized vertex used for comparing to visited vertices
            
            // Calculating new vertex cost
            v.cost = u.dist + 1;  // f(n)
            v.compute_manhattan_distance();
            v.cost += v.heuristic; // h(n)

            v.parent.push_back(u); // Make u parent of v
            v.dist  = u.dist + 1; // Increment distance of v
            v.move.push_back(moves[k].get_id()); //record move that led to that state
            v.move.push_back(moves[k].get_move()); //record move that led to that state

            // Check if v hasn't been visited and check for loops
            if (!is_discovered(v_norm, visited) && !state_compare(v_norm.state.get_state(), normalize(u.parent[0].state.get_state()))) { 
                PQ.push(v);
                visited.push_back(v_norm);
            }
        }
    }
}


// Function uses Iterative Deepening to traverse SBP using Stack (LIFO) with depth limit L
bool Iterative_Deepening(State& state, int L) {
    vector<Vertex> visited; //Container for all visited vertices
    int num_nodes = 0; int length = 0; 
    stack<Vertex> S;

    Vertex start_v = Vertex(state);
    start_v.parent.push_back(start_v);
    start_v.dist = 0; // Root node has distance 0
    start_v.move.push_back(0); // Default block_id to 0
    start_v.move.push_back(0); // Default move to 0
    visited.push_back(start_v); // Add start vertex as visited
    S.push(start_v); // Push starting vertex onto stack

    clock_t time = clock(); //Start timer
    while (!S.empty())
    {
        Move_Gen mv_gen;
        Vertex u = S.top(); // Pop vertex
        S.pop();
        num_nodes++;

        // Check if state has reached goal
        if (u.state.check_complete(u.state.get_state())) {
            time = clock() - time;
            
            //Print Path
            vector<vector<int> > move_path;
            move_path = find_path(start_v, u, move_path);
            print_path(move_path);
            length = u.dist;

            u.state.display();
            cout << num_nodes << " " << setprecision(3) << (float)time/CLOCKS_PER_SEC << " " << length << endl;
            return true;
        }

        // Store array of block moves
        vector<Block> blocks = mv_gen.all_valid_block_moves(u.state.get_state());
        vector<Move> moves;

        // Iterate through all valid block moves
        for (int i=0; i < blocks.size(); i++) {
            vector<int> moves_list = blocks[i].get_moves();
            // For every move each block can make, construct vector of moves
            for (int j=0; j < moves_list.size(); j++) {
                Move move = Move(blocks[i].get_id(), moves_list[j]);
                moves.push_back(move);
            }
        }

        // Create child vertices and pop them onto stack
        for (int k=0; k < moves.size(); k++){
            State s = mv_gen.applyMoveCloning(u.state, moves[k]);
            
            Vertex v = Vertex(s); // Child
            s.normalize();
            Vertex v_norm = Vertex(s); // normalized vertex used for comparing to visited vertices

            v.parent.push_back(u); // Make u parent of v
            v.dist  = u.dist + 1; // Increment distance of v
            v.move.push_back(moves[k].get_id()); //record move that led to that state
            v.move.push_back(moves[k].get_move()); //record move that led to that state

            // Check if v hasn't been visited, depth < limit, and check for loops
            if (!is_discovered(v_norm, visited) && !state_compare(v_norm.state.get_state(), normalize(u.parent[0].state.get_state())) && v.dist <= L) { 
                S.push(v);
                visited.push_back(v_norm);
            }
        }
    }
    return false;
}

// Function incremenets depth limit and calls Iterative Deepening Search with increasing value L
void Iterative_Deepening_Loop(State& state) {
    int L = 0;
    while(1) {
        State s;
        s.set_state(state.copy());
        if (Iterative_Deepening(s, L)) break;
        L++;
    }
}

int main(int argc, char* argv[]){


    cout << "Random Walk Implementation:(N = 3)" << endl;
    cout << "-----------------------------------" << endl;

    // Running Random Walk Generator with N = 3
    State puzzle;
    const char* path = "./SBP-level0.txt";
    puzzle.load_puzzle(path);
    random_walks(puzzle, 3);
    file.close();
    

    cout << endl << "BFS Implementation:" << endl;
    cout << "---------------------" << endl;
    // Running BFS Implementation
    State puzzle2;
    const char* path2 = "./SBP-level1.txt";
    puzzle2.load_puzzle(path2);
    BFS(puzzle2);
    file.close();
    
    
    cout << endl << "DFS Implementation:" << endl;
    cout << "-----------------------" << endl;

    // Running DFS Implementation
    State puzzle3;
    const char* path3 = "./SBP-level1.txt";
    puzzle3.load_puzzle(path3);
    DFS(puzzle3);
    file.close();

    cout << endl << "Iterative Deepening Search Implementation:" << endl;
    cout << "-------------------------------------------" << endl;

    // Running Iterative Deepenidng Search Implementation
    State puzzle4;
    const char* path4 = "./SBP-level1.txt";
    puzzle4.load_puzzle(path4);
    puzzle4.display();
    Iterative_Deepening_Loop(puzzle4);
    file.close();
    

    cout << endl << "A Star Search Implementation:" << endl;
    cout << "------------------------------" << endl;

    // Running A Star Search Heuristic using Priority Queue
    State puzzle5;
    const char* path5 = "./SBP-level1.txt";
    puzzle5.load_puzzle(path5);
    puzzle5.display();
    A_Star_Search(puzzle5);
    file.close();

}

