#include <bits/stdc++.h>
#define ll long long
#define MIN_LENGTH 3
#define DEBUG false

using namespace std;

vector<int> scores = {0, 0, 0, 100, 400, 800, 1400, 1800, 2200, 2600, 3000, 3400, 3800, 4200, 4600, 5000};
string wordlist = "CSW21_longer.txt";
ifstream wordlist_file;
string history = "anagrams_log.txt";
ofstream history_file(history, ios_base::app);
set<string> words;

struct comp {
    bool operator()(string a, string b) const {
        if (a.size() == b.size()) return a < b;
        return a.size() > b.size();
    }
};

string getCurrentDateTimeString() {
    stringstream ss;
    
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    ss << std::setfill('0') << std::setw(2) << 1 + localTime->tm_mon << "/" << std::setw(2) << localTime->tm_mday << "/"
       << 1900 + localTime->tm_year << " " << std::setw(2) << localTime->tm_hour << ":" << std::setw(2) << localTime->tm_min;

    return ss.str();
}

int get_score(int len) {
    return (len < scores.size()) ? scores[len] : (1400 + 400 * (len - 7));
}

template <typename T>
int find_anagrams(string substr, string board, int size, ll mask, T& valid_words) {
    int score = 0;
    if (DEBUG) cout << substr << " " << board << " " << size << " " << mask << endl;
    if (words.find(substr) != words.end()) {
        if (valid_words.insert(substr).second) 
            score += get_score(substr.size());
    }
    for (int i = 0; i < size; i++) {
        if ((((1 << i) & mask) == 0)) {
            substr.push_back(board[i]);
            string next = (*(words.lower_bound(substr)));
            if (DEBUG) cout << "NEXT: " << next << endl;
            if (next.size() >= substr.size() && next[substr.size()-1] == board[i]) {
                score += find_anagrams(substr, board, size, mask | (1 << i), valid_words);
            }
            substr.pop_back();
        }
    }
    return score;
}

set<string, comp> valid_words;
int find_anagrams(string board) {
    valid_words.clear();
    return find_anagrams("", board, board.size(), 0, valid_words);
}

void solve(bool inputting) {
    while (inputting) {
        int points = 0, min_length = 3;
        string board, notes;
        cout << "please input board, notes, min length to print: " << flush;
        cin >> board >> notes >> min_length;
        std::transform(board.begin(), board.end(), board.begin(), ::toupper);
        if (board == "QUIT") return;

        points = find_anagrams(board);
        cout << endl << points << endl;
        history_file << board << " " << points << " | " << notes << " " << getCurrentDateTimeString() << endl;
        
        string prev = *(valid_words.begin());
        for (string s : valid_words) {
            points -= get_score(s.size());
            if (s.size() < min_length) continue;
            if (s.size() != prev.size()) cout << endl;
            if (s.size() != prev.size()) history_file << endl;
            cout << s << " ";
            history_file << s << " ";
            prev = s;
        }
        assert(points == 0);
        cout << endl;
        history_file << endl << endl;
    }
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

    solve(1);
    history_file << "=======================================" << endl;
    history_file.close();
    return 0;
}