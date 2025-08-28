#include <bits/stdc++.h>
#define ll long long

using namespace std;

const int board_rows = 4, board_cols = 4;
string board[board_rows][board_cols];
set<string> words;

ll coord_to_mask(int row, int col) {
    return (1 << (row * board_cols + col));
}

vector<pair<int, int>> find_neighbors(int row, int col) {
    vector<pair<int, int>> neighbors;
    for (int i = max(0, row - 1); i <= min(board_rows-1, row + 1); ++i) {
        for (int j = max(0, col - 1); j <= min(board_cols-1, col + 1); ++j) {
            if (!(i == row && j == col)) {
                neighbors.emplace_back(i, j);
            }
        }
    }

    return neighbors;
}

struct comp {
    bool operator()(string a, string b) const {
        if (a.size() == b.size()) return a < b;
        return a.size() > b.size();
    }
};


vector<vector<vector<set<pair<string, ll>>>>> generate_combos_dp(int target_length) {
    vector<vector<vector<set<pair<string, ll>>>>> generated;
    generated.resize(board_cols * board_rows);
    generated[0].resize(board_rows);
    for (int i = 0; i < target_length; i++) { // i = leng-1
        generated[i].resize(board_rows);
        for (int row = 0; row < board_rows; ++row) {
            generated[i][row].resize(board_cols);
        }
    }
    
    for (int row = 0; row < board_rows; ++row) {
        for (int col = 0; col < board_cols; ++col) {
            generated[0][row][col].emplace(board[row][col], coord_to_mask(row, col));
        }
    }

    for (int i = 1; i < target_length; i++) { // i = leng-1
        for (int row = 0; row < board_rows; ++row) {
            for (int col = 0; col < board_cols; ++col) {
                auto neighbors = find_neighbors(row, col);
                for (auto& n : neighbors) {
                    int neighbor_row, neighbor_col;
                    tie(neighbor_row, neighbor_col) = n;
                    auto neighbor_mask = coord_to_mask(neighbor_row, neighbor_col);
                    for (auto& short_item : generated[i-1][row][col]) {
                        if (neighbor_mask & short_item.second) {
                            continue;
                        }
                        string closest_next_word = *(words.lower_bound(short_item.first));
                        if ((closest_next_word.size() < i) | (closest_next_word[i-1] != (*short_item.first.rbegin()))) continue;
                        generated[i][neighbor_row][neighbor_col].emplace((short_item.first + board[neighbor_row][neighbor_col]), (short_item.second | neighbor_mask));
                    }
                }
            }
        }
    }
    return generated;
}

set<string, comp> solve_board(int k) { 
    auto generated = generate_combos_dp(k);
    set<string, comp> valid_words;
    for (int i = 0; i < k; i++) { // i = leng-1
        for (int row = 0; row < board_rows; ++row) {
            for (int col = 0; col < board_cols; ++col) {
                for (auto& combo : generated[i][row][col]) {
                    string word = combo.first;
                    if (words.find(word) != words.end()) {
                        valid_words.insert(combo.first); // O(logW)
                    }
                }
            }
        }
    }
    return valid_words;
}

vector<double> cum_letter_weights(26);
double weight_sum = 0;
void generate_board() {
    std::uniform_real_distribution<double> unif(0,weight_sum);
    std::mt19937 re(std::time(0));
    for (int i = 0; i < board_cols * board_rows; i++) {
        double random_double = unif(re);
        char random_char = ('A' + (lower_bound(cum_letter_weights.begin(), cum_letter_weights.end(), random_double) - cum_letter_weights.begin()));
        board[i / board_rows][i % board_cols] = random_char;
    }
}

void string_to_board(string board_string) {
    for (int i = 0; i < board_cols * board_rows; i++) {
        board[i / board_rows][i % board_cols] = board_string[i];
    }
}

// J E O D A C M T G C G E D O C O
string test = "RHECNRDTNADHEATF";//"BEFTEEZANINTTERQ";//"STRYHGTIAAVDRSEE"; TIALITNCAUNOEEDS CONTINUED 9 FOUND BUT LOST STILL :(
int min_length = 3, long_word = 6;
vector<int> scores = {0, 0, 0, 100, 400, 800, 1400, 1800, 2200, 2600, 3000, 3400};
string board_string;
string wordlist = "CSW21.txt";
ifstream wordlist_file;
string letterdata = "letterdata.txt";
ifstream letterdata_file;
string history = "log.txt";
ofstream history_file(history, ios_base::app);
    
std::string getCurrentDateTimeString() {
    std::stringstream ss;
    
    // Get current time
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    // Format date and time into the stringstream
    ss << std::setfill('0')
       << std::setw(2) << 1 + localTime->tm_mon << "/"
       << std::setw(2) << localTime->tm_mday << "/"
       << 1900 + localTime->tm_year << " "
       << std::setw(2) << localTime->tm_hour << ":"
       << std::setw(2) << localTime->tm_min;

    return ss.str();
}

void solve(bool list_words, string notes) {
    auto start = std::chrono::high_resolution_clock::now();
    string_to_board(board_string);
    auto valid_words = solve_board(16);
    if (list_words) {
        for (string s : valid_words) {
            if (s.size() > min_length) cout << s << "\n";
        }
        cout << endl;
        return;
    }
    int points = 0;
    //vector<string> longer;
    string longer = " ", longg = " ", tens = " ";
    for (string s : valid_words) {
        if (s.size() < min_length) continue;
        if (s.size() < scores.size()) points += scores[s.size()];
        else longer += (s + "\n");
        // else longer.push_back(s);
        if (s.size() > long_word) longg += (s + " ");
        if (s.size() >= 10) tens += (s + " ");
    }
    for (int i = 0; i < board_rows * board_cols; i++) {
        cout << board[i / board_rows][i % board_cols];
    }
    cout << longg;
    cout << "\n" << points << longer << "\n";
    // for (string s : longer) {
    //     cout << s << "\n";
    // }
    // cout << longer;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout << "Runtime: " << duration.count() << " milliseconds" << endl;
    
    for (int i = 0; i < board_rows * board_cols; i++) {
        history_file << board[i / board_rows][i % board_cols];
    }
    // for (string s : longer) {
    //     history_file << " " << s;
    // }
    // cout << longer;
    history_file << longer << points << " | " << notes << tens << getCurrentDateTimeString() << "\n";
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    wordlist_file.open(wordlist);
    string word;
    while (wordlist_file >> word) {
        words.insert(word);
    }
    wordlist_file.close();

    letterdata_file.open(letterdata);
    char letter; double freq;
    for (int i = 0; i < 26; i++) {
        letterdata_file >> letter >> freq;
        cum_letter_weights[letter-'A'] = freq;
    }
    letterdata_file.close();

    for (int i = 1; i < 26; i++) {
        cum_letter_weights[i] += cum_letter_weights[i-1];
    }
    weight_sum = cum_letter_weights[cum_letter_weights.size()-1];

    bool inputting = true, lst = false;
    if (inputting) cin >> board_string;
    while (inputting) {
        if (board_string == "LIST") {
            lst = true;
            cin >> board_string;
        }
        else if (board_string.size() != board_rows*board_cols) break;
        else {
            lst = false;
        }
        string notes;
        if (!lst) {
            cin >> notes;
        }
        transform(board_string.begin(), board_string.end(), board_string.begin(), ::toupper);
        solve(lst, notes);
        cin >> board_string;
    }
    history_file << "=======================================" << endl;
    history_file.close();
    return 0;
}