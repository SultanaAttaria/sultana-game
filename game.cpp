#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;

// UTILITY 
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnter() {
    cout << "\n  [ Press ENTER to continue ]";
    cin.get();
}
class Game {
    string title;
public:
    Game(string t) : title(t) {}
    string getTitle() const { return title; }
    virtual void play() = 0;   // pure virtual → abstract class
    virtual ~Game() {}
};
class Question {
    string text;
    string opts[4];
    int    correct;      // 1=A  2=B  3=C  4=D
    string difficulty;
public:
    Question() : correct(0) {}

    // Constructor initialiser list now matches declaration order
    Question(string t, string a, string b,
             string c, string d, int ans, string diff)
        : text(t), correct(ans), difficulty(diff)
    {
        opts[0]=a; opts[1]=b; opts[2]=c; opts[3]=d;
    }

    // Function Overloading  display without number (practice mode)
    void display() const {
        cout << "\n  " << text << "\n\n";
        for (int i = 0; i < 4; i++)
            cout << "    " << (char)('A'+i) << ")  " << opts[i] << "\n";
        cout << "\n  Your Answer (A / B / C / D) : ";
    }

    // Function Overloading  display with question number (quiz mode)
    void display(int n) const {
        cout << "\n  Q" << n << ".  " << text << "\n\n";
        for (int i = 0; i < 4; i++)
            cout << "    " << (char)('A'+i) << ")  " << opts[i] << "\n";
        cout << "\n  Your Answer (A / B / C / D) : ";
    }

    bool   isCorrect(int ans)   const { return ans == correct; }
    string getDiff()            const { return difficulty; }
    string getCorrectText()     const { return opts[correct - 1]; }
    char   getCorrectLetter()   const { return char('A' + correct - 1); }
    string getText()            const { return text; }
};
class Player {
    string name;
    int    score;
    int    total;
    string grade;   // declared last  initialised last (fixes -Wreorder)
public:
    //   order now matches declaration order
    Player(string n) : name(n), score(0), total(0), grade("F") {}

    void addScore()      { score++; }
    void setTotal(int t) { total = t; }

    void calcGrade() {
        double pct = total ? double(score) / total * 100.0 : 0.0;
        //  removed trailing spaces from grade strings
        if      (pct >= 90) grade = "A+";
        else if (pct >= 80) grade = "A";
        else if (pct >= 70) grade = "B";
        else if (pct >= 60) grade = "C";
        else if (pct >= 50) grade = "D";
        else                grade = "F";
    }

    string getName()  const { return name; }
    int    getScore() const { return score; }
    int    getTotal() const { return total; }
    string getGrade() const { return grade; }

    void showResult() const {
        double pct = total ? double(score) / total * 100.0 : 0.0;

        cout << "\n";
        cout << "  Player    :  " << name << "\n";
        cout << "  Score     :  " << score << " / " << total << "\n";
        cout << "  Percent   :  " << fixed << setprecision(1) << pct << "%\n";
        cout << "  Grade     :  " << grade << "\n";
        cout << "\n";

        if      (grade == "A+") cout << "  Outstanding! You nailed it.\n";
        else if (grade == "A")  cout << "  Excellent! Great performance.\n";
        else if (grade == "B")  cout << "  Good work. Keep it up!\n";
        else if (grade == "C")  cout << "  Decent. Room to improve.\n";
        else if (grade == "D")  cout << "  Passed, but study more.\n";
        else                    cout << "  Keep practicing. You will get there.\n";
    }
};

class ScoreBoard {
    struct Entry {
        string name, grade, diff;
        int    score, total;
    };

    vector<Entry> entries;
    const char*   FILE_PATH = "scores.txt";  //  was const string FILE

    static bool cmp(const Entry& a, const Entry& b) {
        return a.score > b.score;
    }

public:
    ScoreBoard() {
        ifstream f(FILE_PATH);
        if (!f) return;
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            // pipe separated name|score|total|grade|diff
            size_t p0 = line.find('|');
            size_t p1 = line.find('|', p0 + 1);
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);
            if (p3 == string::npos) continue;
            Entry e;
            e.name  = line.substr(0, p0);
            e.score = stoi(line.substr(p0+1, p1-p0-1));
            e.total = stoi(line.substr(p1+1, p2-p1-1));
            e.grade = line.substr(p2+1, p3-p2-1);
            e.diff  = line.substr(p3+1);
            entries.push_back(e);
        }
    }

    void save(const Player& p, const string& diff) {
        Entry e = { p.getName(), p.getGrade(), diff,
                    p.getScore(), p.getTotal() };
        entries.push_back(e);
        sort(entries.begin(), entries.end(), cmp);
        ofstream f(FILE_PATH);
        for (auto& x : entries)
            f << x.name  << "|" << x.score << "|"
              << x.total << "|" << x.grade << "|"
              << x.diff  << "\n";
        cout << "\n  Score saved.\n";
    }

    void show() const {
        cout << "\n  Leaderboard\n\n";
        cout << "  " << left
             << setw(4)  << "Rank"
             << setw(16) << "Player"
             << setw(10) << "Score"
             << setw(10) << "Level"
             << "\n";
        cout << "  " << string(40, '-') << "\n";

        if (entries.empty()) {
            cout << "  No scores recorded yet.\n";
        } else {
            int r = 1;
            for (auto& e : entries) {
                if (r > 10) break;
                string scoreStr = to_string(e.score) + "/" + to_string(e.total);
                cout << "  " << left
                     << setw(4)  << r++
                     << setw(16) << e.name
                     << setw(10) << scoreStr
                     << setw(10) << e.diff
                     << "\n";
            }
        }
        cout << "\n";
    }
};
class Quiz {
    vector<Question> bank;
    string           diff;

    void loadQuestions() {
        // Easy
        bank.push_back({"Capital of Pakistan?",
                         "Lahore","Karachi","Islamabad","Peshawar", 3, "Easy"});
        bank.push_back({"Largest planet in the solar system?",
                         "Earth","Saturn","Jupiter","Mars",          3, "Easy"});
        bank.push_back({"Days in a leap year?",
                         "365","366","364","367",                    2, "Easy"});
        bank.push_back({"2 + 2 x 2 = ?",
                         "6","8","4","5",                            1, "Easy"});
        // Medium
        bank.push_back({"What does OOP stand for?",
                         "Ordered Object Protocol",
                         "Object Oriented Programming",
                         "Open Output Process","None",               2, "Medium"});
        bank.push_back({"Inheritance symbol in C++?",
                         "->","::",":",";",                          3, "Medium"});
        bank.push_back({"Output of  10 % 3 ?",
                         "3","1","0","2",                            2, "Medium"});
        bank.push_back({"Which data structure uses LIFO?",
                         "Queue","Array","Stack","Tree",             3, "Medium"});
        // Hard
        bank.push_back({"Time complexity of Binary Search?",
                         "O(n)","O(n^2)","O(log n)","O(1)",         3, "Hard"});
        bank.push_back({"Correct pure virtual function syntax?",
                         "virtual f(){}","virtual f();",
                         "virtual f()=0;","f()=0;",                 3, "Hard"});
        bank.push_back({"Which is NOT a pillar of OOP?",
                         "Inheritance","Recursion",
                         "Polymorphism","Encapsulation",             2, "Hard"});
        bank.push_back({"When is a constructor called?",
                         "Manually","At object creation",
                         "At deletion","Never",                      2, "Hard"});
    }

public:
    Quiz(string d) : diff(d) { loadQuestions(); }

    // Returns updated Player after running the quiz
    Player run(Player p) {
        // Filter questions for chosen difficulty
        vector<Question> qs;
        for (auto& q : bank)
            if (q.getDiff() == diff) qs.push_back(q);

        if (qs.empty()) {
            cout << "\n  No questions found for this level.\n";
            return p;
        }

        // Shuffle
        srand(unsigned(time(nullptr)));
        for (int i = int(qs.size()) - 1; i > 0; i--)
            swap(qs[i], qs[rand() % (i + 1)]);

        p.setTotal(int(qs.size()));
        vector<pair<string,char>> wrong;

        for (int i = 0; i < int(qs.size()); i++) {
            clearScreen();

            cout << "\n  Level  :  " << diff
                 << "   |   Question " << (i+1) << " of " << qs.size()
                 << "   |   Score : " << p.getScore() << "\n";
            cout << "\n";

            qs[i].display(i + 1);

            // input loop now uses a single cin>>ch
            //           and ignores leftover '\n' once after the loop
            char ch; int ans = 0;
            while (true) {
                cin >> ch;
                ch = char(toupper(ch));
                if (ch=='A'){ans=1;break;}
                if (ch=='B'){ans=2;break;}
                if (ch=='C'){ans=3;break;}
                if (ch=='D'){ans=4;break;}
                cout << "  Invalid. Enter A, B, C or D : ";
            }
            cin.ignore(); // flush the '\n' once after valid input

            if (qs[i].isCorrect(ans)) {
                cout << "\n  Correct!\n";
                p.addScore();
            } else {
                cout << "\n  Wrong.  Correct answer : ("
                     << qs[i].getCorrectLetter() << ")  "
                     << qs[i].getCorrectText() << "\n";
                wrong.push_back({qs[i].getText(), qs[i].getCorrectLetter()});
            }

            pressEnter();
        }

        // Wrong answers review
        if (!wrong.empty()) {
            clearScreen();
            cout << "\n  Review  -  Questions you missed\n\n";
            for (int i = 0; i < int(wrong.size()); i++)
                cout << "  " << (i+1) << ".  " << wrong[i].first
                     << "\n      Correct : (" << wrong[i].second << ")\n\n";
            pressEnter();
        }

        p.calcGrade();
        return p;
    }
};

class QuizGame : public Game {
    ScoreBoard board;

    string chooseDifficulty() {
        cout << "\n  Select Difficulty\n\n";
        cout << "    1.  Easy\n";
        cout << "    2.  Medium\n";
        cout << "    3.  Hard\n";
        cout << "\n  Choice : ";
        int c; cin >> c; cin.ignore();
        if (c == 1) return "Easy";
        if (c == 3) return "Hard";
        return "Medium";
    }

public:
    QuizGame() : Game("Quiz Challenge") {}

    // Polymorphism  overrides Game play()
    void play() override {
        bool running = true;
        while (running) {
            clearScreen();

            cout << "\n  Quiz Challenge\n";
            cout << "\n";
            cout << "    1.  Play\n";
            cout << "    2.  Leaderboard\n";
            cout << "    3.  Quit\n";
            cout << "\n  Choice : ";

            int c; cin >> c; cin.ignore();

            if (c == 1) {
                clearScreen();
                cout << "\n  Enter your name : ";
                string name; getline(cin, name);
                if (name.empty()) name = "Player";

                string diff = chooseDifficulty();
                Quiz   quiz(diff);
                Player p(name);
                p = quiz.run(p);

                clearScreen();
                p.showResult();
                board.save(p, diff);
                pressEnter();

            } else if (c == 2) {
                clearScreen();
                board.show();
                pressEnter();

            } else {
                cout << "\n  Goodbye!\n\n";
                running = false;
            }
        }
    }
};
int main() {
    Game* g = new QuizGame();   // base pointer derived object
    g->play();                  // virtual dispatch QuizGame:play()
    delete g;
    return 0;
}