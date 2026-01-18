#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class Tic_tac_toe {
private:
    int game_rows = 0;
    int game_mode = 1;
    int total_size = 0;
 
    vector<pair<int,int>> Total_moves;                    // Both player moves 
    vector<pair<int,int>> Player_1;                         // Player 1  moves
    vector<pair<int,int>> Player_2;                         // Player 2 Moves.
    vector<vector<pair<int,int>>> Vector_lines;              // All vector lines.
  
public:
    void Game_Board(int size);
    void User_Input(int rows, int play_mode);
    void Vector_Generator();
    bool Check_Winner(int play_mode);
    void Tie_Breaker();
};


bool Used_cells(const vector<pair<int,int>>& total_moves, pair<int,int> p) {    // Check if a cell is already used
    for (auto &x : total_moves)
        if (x == p)    return true;
    return false;
}

bool Check_Points(const vector<pair<int,int>>& moves, const vector<pair<int,int>>& line) {     // Check if player moves contain all points of a vector line                              
    for (auto &pt : line) {
        bool found = false;
        for (auto &m : moves) {
            if (m == pt) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

pair<double,double> Centroid(const vector<pair<int,int>>& moves) {              // Compute centroid of points
    double x = 0, y = 0;
    for (auto &p : moves) {
        x += p.first;
        y += p.second;
    }
    return {x / moves.size(), y / moves.size() };                                // Returns G(g,f)
}


double Dot(pair<double,double> a, pair<double,double> b) {         // Dot product
    return a.first*b.first + a.second*b.second;                     // a.b =  x1.x2 + y1.y2
}

 
double Mag(pair<double,double> v) {                                 // Vector magnitude
    return sqrt(v.first*v.first + v.second*v.second);               // Magnitude = root (X2 + y2)
}


void Tic_tac_toe::Game_Board(int size) {                             // Update and Print board 

    game_rows = size;
    total_size = game_rows * game_rows;

    vector<vector<int>> grid(game_rows, vector<int>(game_rows, 0));

    for (auto &p : Player_1)
        grid[p.first][p.second] = 1;                                    // Assign Player 1 moves as 1

    for (auto &p : Player_2)
        grid[p.first][p.second] = 2;                                     // Assign Player 2 moves as 2

    cout << "\nCurrent Board:\n";

    for (int i = 0; i < game_rows; i++) {
        for (int j = 0; j < game_rows; j++) {
            if (grid[i][j] == 1) cout << "X\t";                          // Player 1 = X
            else if (grid[i][j] == 2) cout << "O\t";                     // Player 2 = O
            else cout << i*game_rows + j + 1 << "\t";                   // Value = N.y + X
        }
        cout << "\n";
    }
}

void Tic_tac_toe::Vector_Generator() {                           // Generates all the vector lines.

    Vector_lines.clear();

    vector<pair<int,int>> D1, D2;

    for (int t = 0; t < game_rows; t++) {

        vector<pair<int,int>> row, col;

        for (int k = 0; k < game_rows; k++) {
            row.push_back({t, k});                                 // Row Vector = (t, k)
            col.push_back({k, t});                                 // Column vector = (k,t)
        }

        Vector_lines.push_back(row);
        Vector_lines.push_back(col);

        D1.push_back({t, t});                                     // D1 vector = (t,t)
        D2.push_back({game_rows - t - 1, t});                     // D2 vector = (N-t-1)
    }

    Vector_lines.push_back(D1);
    Vector_lines.push_back(D2);
}

void Tic_tac_toe::User_Input(int rows, int play_mode) {              // Take continuous user input..

    game_rows = rows;
    game_mode = play_mode;
    total_size = game_rows * game_rows;

    while (Total_moves.size() < total_size) {

        int value;
        cout << "\nPlayer " << ((Total_moves.size()%2==0)?1:2)
             << ", Enter move (1-" << total_size << "): ";
        cin >> value;

        if (cin.fail() || value < 1 || value > total_size) {
            cout << "Invalid input!\n";
            cin.clear();
            while (cin.get()!='\n');
            continue;
        }

        int x = (value - 1) / game_rows;                                     // X cordinate = User_input / N;
        int y = (value - 1) % game_rows;                                     // X cordinate = User_input % N;

        pair<int,int> p = {x, y};
 
        if (Used_cells(Total_moves, p)) {                                    // Check for cells.
            cout << "Cell already filled!\n";
            continue;
        }

        if (Total_moves.size()%2==0) Player_1.push_back(p);
        else Player_2.push_back(p);                                                // Stor in total player

        Total_moves.push_back(p);

        Game_Board(rows);

        if (Check_Winner(play_mode)) break;                                        
    }
}

bool Tic_tac_toe::Check_Winner(int play_mode) {                       // Check the winner continuosly..

    for (auto &v : Vector_lines)
        if (Check_Points(Player_1, v)) {                                    // Check for player 1
            cout << "\nPlayer 1 Wins!\n";
            return true;
        }

    for (auto &v : Vector_lines)
        if (Check_Points(Player_2, v)) {                                     // Check for player 2
            cout << "\nPlayer 2 Wins!\n";
            return true;
        }

    if (Total_moves.size() == total_size) {
        if (play_mode == 2) Tie_Breaker();                                  // Calls for Tie- breaker mode
        else cout << "\nGame Draw!\n";
        return true;
    }

    return false;
}

void Tic_tac_toe::Tie_Breaker() {                                        // Decide the winer in tie breaker mode.

    auto evaluate = [&](const vector<pair<int,int>>& moves) {

        auto C = Centroid(moves);
        vector<pair<double,double>> V;

        for (auto &p : moves)                                                         // Build vectors from centroid
            V.push_back({p.first - C.first, p.second - C.second});                    // V(x,y) = (x2-x1, y2-y1)

        double sum = 0;
        int count = 0;

        for (int i = 0; i < V.size(); i++) {
            for (int j = i+1; j < V.size(); j++) {

                double m1 = Mag(V[i]);                                   // Mgnitude of Vector 1
                double m2 = Mag(V[j]);                                   // Mgnitude of Vector 2

                if (m1 == 0 || m2 == 0) continue;

                double cosv = Dot(V[i], V[j]) / (m1 * m2);               // Cos = (A.B)/(|A||B|)
    
                if (fabs(cosv) == 1.0) continue;                         // Ignore angle = 0, 180

                sum += fabs(cosv);
                count++;
            }
        }
        return (count == 0) ? 1.0 : sum / count;                     
    };

    double p1 = evaluate(Player_1);
    double p2 = evaluate(Player_2);

    cout << "\nTie-Breaker Result:\n";

    if (p1 < p2) cout << "Player 1 Wins by Tie-Breaker!\n";
    else if (p2 < p1) cout << "Player 2 Wins by Tie-Breaker!\n";
    else cout << "Perfect Tie!\n";
}

int main() {

    int mode, N;

    cout << " Welcome to TIC - TAC - TOE Game.";                                       // Welcome message

    cout << "1) Normal Mode\n2) Tie-Breaker Mode \nEnter mode: ";                        // Play mode
    cin >> mode;

    cout << "Enter board size (>=3): ";                                                // Game board Size
    cin >> N;

    Tic_tac_toe Game;

    Game.Game_Board(N);
    Game.Vector_Generator();
    Game.User_Input(N, mode);

    return 0;
}