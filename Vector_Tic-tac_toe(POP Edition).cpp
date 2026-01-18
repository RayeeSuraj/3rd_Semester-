#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Game_State {
    int game_rows = 0;
    int game_mode = 1;
    int total_size = 0;

    vector<pair<int,int>> Total_moves;
    vector<pair<int,int>> Player_1;
    vector<pair<int,int>> Player_2;
    vector<vector<pair<int,int>>> Vector_lines;
};


bool Used_cells(const vector<pair<int,int>>& total, pair<int,int> p) {
    for (auto &x : total)
        if (x == p) return true;
    return false;
}

bool Check_Points(const vector<pair<int,int>>& moves,
                  const vector<pair<int,int>>& line) {
    for (auto &pt : line) {
        bool found = false;
        for (auto &m : moves)
            if (m == pt) { found = true; break; }
        if (!found) return false;
    }
    return true;
}

pair<double,double> Centroid(const vector<pair<int,int>>& moves) {
    double x = 0, y = 0;
    for (auto &p : moves) {
        x += p.first;
        y += p.second;
    }
    return {x / moves.size(), y / moves.size()};
}

double Dot(pair<double,double> a, pair<double,double> b) {
    return a.first*b.first + a.second*b.second;
}

double Mag(pair<double,double> v) {
    return sqrt(v.first*v.first + v.second*v.second);
}


void Game_Board(Game_State &G) {

    vector<vector<int>> grid(G.game_rows, vector<int>(G.game_rows, 0));

    for (auto &p : G.Player_1)
        grid[p.first][p.second] = 1;

    for (auto &p : G.Player_2)
        grid[p.first][p.second] = 2;

    cout << "\nCurrent Board:\n";

    for (int i = 0; i < G.game_rows; i++) {
        for (int j = 0; j < G.game_rows; j++) {
            if (grid[i][j] == 1) cout << "X\t";
            else if (grid[i][j] == 2) cout << "O\t";
            else cout << i*G.game_rows + j + 1 << "\t";
        }
        cout << "\n";
    }
}

void Vector_Generator(Game_State &G) {

    G.Vector_lines.clear();
    vector<pair<int,int>> D1, D2;

    for (int t = 0; t < G.game_rows; t++) {

        vector<pair<int,int>> row, col;

        for (int k = 0; k < G.game_rows; k++) {
            row.push_back({t, k});
            col.push_back({k, t});
        }

        G.Vector_lines.push_back(row);
        G.Vector_lines.push_back(col);

        D1.push_back({t, t});
        D2.push_back({G.game_rows - t - 1, t});
    }

    G.Vector_lines.push_back(D1);
    G.Vector_lines.push_back(D2);
}

bool Tie_Breaker(Game_State &G) {

    auto evaluate = [&](const vector<pair<int,int>>& moves) {

        auto C = Centroid(moves);
        vector<pair<double,double>> V;

        for (auto &p : moves)
            V.push_back({p.first - C.first,
                         p.second - C.second});

        double sum = 0;
        int count = 0;

        for (int i = 0; i < V.size(); i++) {
            for (int j = i+1; j < V.size(); j++) {

                double m1 = Mag(V[i]);
                double m2 = Mag(V[j]);
                if (m1 == 0 || m2 == 0) continue;

                double cosv = Dot(V[i], V[j]) / (m1*m2);
                if (fabs(cosv) == 1.0) continue;

                sum += fabs(cosv);
                count++;
            }
        }
        return (count == 0) ? 1.0 : sum / count;
    };

    double p1 = evaluate(G.Player_1);
    double p2 = evaluate(G.Player_2);

    cout << "\nTie-Breaker Result:\n";

    if (p1 < p2) cout << "Player 1 Wins!\n";
    else if (p2 < p1) cout << "Player 2 Wins!\n";
    else cout << "Perfect Tie!\n";

    return true;
}

bool Check_Winner(Game_State &G) {

    for (auto &v : G.Vector_lines)
        if (Check_Points(G.Player_1, v)) {
            cout << "\nPlayer 1 Wins!\n";
            return true;
        }

    for (auto &v : G.Vector_lines)
        if (Check_Points(G.Player_2, v)) {
            cout << "\nPlayer 2 Wins!\n";
            return true;
        }

    if (G.Total_moves.size() == G.total_size) {
        if (G.game_mode == 2) Tie_Breaker(G);
        else cout << "\nGame Draw!\n";
        return true;
    }

    return false;
}

void User_Input(Game_State &G) {

    while (G.Total_moves.size() < G.total_size) {

        int value;
        cout << "\nPlayer " << ((G.Total_moves.size()%2==0)?1:2)
             << ", Enter move (1-" << G.total_size << "): ";
        cin >> value;

        if (cin.fail() || value < 1 || value > G.total_size) {
            cin.clear();
            while (cin.get()!='\n');
            cout << "Invalid input!\n";
            continue;
        }

        int x = (value - 1) / G.game_rows;
        int y = (value - 1) % G.game_rows;

        pair<int,int> p = {x, y};

        if (Used_cells(G.Total_moves, p)) {
            cout << "Cell already filled!\n";
            continue;
        }

        if (G.Total_moves.size()%2==0) G.Player_1.push_back(p);
        else G.Player_2.push_back(p);

        G.Total_moves.push_back(p);

        Game_Board(G);

        if (Check_Winner(G)) 
        break;
    }
}

int main() {

    Game_State G;

    cout << "Welcome to TIC-TAC-TOE\n";
    cout << "1) Normal Mode\n2) Tie-Breaker Mode\nEnter mode: ";
    cin >> G.game_mode;

    cout << "Enter board size (>=3): ";
    cin >> G.game_rows;

    G.total_size = pow(G.game_rows , 2);

    Game_Board(G);
    Vector_Generator(G);
    User_Input(G);

    return 0;
}
