
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

class TransitionCol {
    vector<int> state, nextStates;
    int totalOcc;
};

struct compare {
    vector<int> key;
    compare(vector<int> i) : key(i) {}

    bool operator()(vector<int> i) {
        return (i == key);
    }
};

class MarkovChain {
    vector<vector<int>> stateSpace;
    vector<vector<int>> transitionSpace;
    vector<TransitionCol> transitionMatrix;

   public:
    //function for printing the elements in a vector
    void showVector(vector<int> g) {
        vector<int>::iterator it;
        for (it = g.begin(); it != g.end(); ++it)
            cout << '\t' << *it;
        cout << '\n';
    }

    void showVectorOfVector(vector<vector<int>> vec) {
        for (int i = 0; i < vec.size(); i++) {
            for (int j = 0; j < vec[i].size(); j++) {
                cout << '\t' << vec[i][j];
            }
            cout << '\n';
        }
    }

    void buildTransition(vector<int> currentState, vector<int> lastState) {
        if (none_of(stateSpace.begin(), stateSpace.end(), compare(currentState))) {
            stateSpace.push_back(currentState);
        }
        // showVector(currentState);
        showVectorOfVector(stateSpace);
        if (!lastState.empty()) {
            
        }
    }
};

int main() {
    MarkovChain markovChain;

    vector<int> currentState = {0, 1, 0, 1};
    vector<int> lastState = {0, 1, 1, 1};
    markovChain.buildTransition(currentState, lastState);
    markovChain.buildTransition(lastState, currentState);
    return 0;
}