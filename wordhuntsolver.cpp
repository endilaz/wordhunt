#include <bits/stdc++.h>
#define ll long long

using namespace std;
// mitm?
// just do simple dfs from each tile, no need to do fancy dp?
// prune early if binary search return value [i] != word[i] (aka no word can be formed after this combo) (reverse the word order tho, so ending at tile instead of starting at tile)
//int n = 200000;
const int board_rows = 4, board_cols = 4;
string board[board_rows][board_cols];
// string placeholder(board_cols, 'A');
// vector<string> board(board_rows, placeholder);
set<string> words;// valid_words;
// vector<vector<set<pair<string, ll>>>> combos;

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

// bool comp(string a, string b) {
//     return a.size() < b.size();
// }
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
            //cout << row << col << endl;
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
                        //if (short_item.first.size() < target_length-1) continue;
                        if (neighbor_mask & short_item.second) {
                            continue;
                        }
                        string closest_next_word = *(words.lower_bound(short_item.first));
                        //cout << short_item.first << " " << closest_next_word << "" << i << "\n";
                        if ((closest_next_word.size() < i) | (closest_next_word[i-1] != (*short_item.first.rbegin()))) continue;
                        generated[i][neighbor_row][neighbor_col].emplace((short_item.first + board[neighbor_row][neighbor_col]), (short_item.second | neighbor_mask));
                    }
                }
            }
        }
    }
    return generated;
}

/*
// [row][col] -> [words pairs], [row][col][index] -> specific word pair
// vector<vector<set<pair<string, ll>>>> generate_combos(int target_length) {
//     vector<vector<set<pair<string, ll>>>> curr_list(board_rows);
//     for (int row = 0; row < board_rows; ++row) {
//         curr_list[row].resize(board_cols);
//         for (int col = 0; col < board_cols; ++col) {
//             curr_list[row][col].emplace(board[row][col], coord_to_mask(row, col));
//         }
//     }

//     if (target_length == 1) {
//         return curr_list;
//     }

//     auto shorter = generate_combos(target_length - 1); // O(n!)
//     for (int row = 0; row < board_rows; ++row) {
//         for (int col = 0; col < board_cols; ++col) {
//             auto neighbors = find_neighbors(row, col);
//             for (auto& n : neighbors) {
//                 int neighbor_row, neighbor_col;
//                 tie(neighbor_row, neighbor_col) = n;
//                 for (auto& short_item : shorter[row][col]) {
//                     if (short_item.first.size() < target_length-1) continue;
//                     auto neighbor_mask = coord_to_mask(neighbor_row, neighbor_col);
//                     if (neighbor_mask & short_item.second) {
//                         continue;
//                     }
//                     curr_list[neighbor_row][neighbor_col].emplace(board[neighbor_row][neighbor_col] + short_item.first, short_item.second | neighbor_mask);
//                 }
//             }
//         }
//     }

//     // for (int row = 0; row < board_rows; ++row) {
//     //     for (int col = 0; col < board_cols; ++col) {
//     //         for (auto& w : curr_list[row][col])
//     //             shorter[row][col].insert(w);
//     //     }
//     // }

//     return shorter;
// }

// vector<vector<set<pair<string, ll>>>> merge(int targ) {
//     vector<vector<set<pair<string, ll>>>> curr_list(board_rows);
//     for (int row = 0; row < board_rows; ++row) {
//         curr_list[row].resize(board_cols);
//         for (int col = 0; col < board_cols; ++col) {
//             curr_list[row][col].emplace(board[row][col], coord_to_mask(row, col));
//         }
//     }
    
//     auto half = generate_combos(targ / 2);

//     for (int row = 0; row < board_rows; ++row) {
//         for (int col = 0; col < board_cols; ++col) {
//             auto neighbors = find_neighbors(row, col);
//             for (auto& n : neighbors) {
//                 int neighbor_row, neighbor_col;
//                 tie(neighbor_row, neighbor_col) = n;
//                 for (auto& h1 : half[row][col]) {
//                     string first_half_copy = h1.first;
//                     reverse(first_half_copy.begin(), first_half_copy.end());
//                     // reverse(h1.first.begin(), h1.first.end());
//                     for (auto& h2 : half[neighbor_row][neighbor_col]) {
//                         if (h1.second & h2.second) continue;
//                         // string new_word = first_half_copy + h2.first;
//                         // ll new_mask = h1.second | h2.second;
//                         // pair<string, ll> comb = make_pair(new_word, new_mask);
//                         curr_list[row][col].emplace(first_half_copy + h2.first, h1.second | h2.second);
//                     }
//                     // reverse(h1.first.begin(), h1.first.end());
//                 }
//             }
//         }
//     }

//     return curr_list;
// }
*/

set<string, comp> solve_board(int k) { // O(logKlogW) * 
    auto generated = generate_combos_dp(k);
    set<string, comp> valid_words;
    for (int i = 0; i < k; i++) { // i = leng-1
        for (int row = 0; row < board_rows; ++row) {
            for (int col = 0; col < board_cols; ++col) {
                for (auto& combo : generated[i][row][col]) {
                    string word = combo.first;
                    //cout << word << " " << i << endl;
                    if (words.find(word) != words.end()) {//binary_search(words.begin(), words.end(), word)) { // O(logK)
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
    //default_random_engine re;
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

    // generate_board();
    // string_to_board("TOINATAELERHHDSS"); // 285700
    // string_to_board("MHONITERLASNSERU"); // wr 542000; HORSETAILS
    // string_to_board("PNROIOHAUGEEGALU"); // 43900
    // string_to_board("IAPNWREUAOHSETSH"); // 195500
    // string_to_board("ROWAPBOHKAYBTTIG"); // 34800
    // string_to_board("CBBWIRGITOUETSNS"); // 188500
    // string_to_board("DRONCTSERETFODLI"); // 263200
    // string_to_board("DSLSMDETWOTEGLPI"); // 139600
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