
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

class State {
   public:
    vector<int> state;
    int occurrences = 1;
    float percentage = 0.0;
    State() {}
    State(vector<int> s) {
        state = s;
    }
};

class TransitionCol {
   public:
    vector<int> state;
    vector<State*> nextStates;
    int totalOcc = 1;
    TransitionCol() {}
    TransitionCol(vector<int> s) {
        state = s;
    }
};

// function for printing the elements in a vector
void showVector(vector<int> g) {
    vector<int>::iterator it;
    for (it = g.begin(); it != g.end(); ++it)
        cout << '\t' << *it;
    cout << '\n';
}

void showVectorOfVector(vector<vector<int>> vec) {
    cout << "transitionSpace" << '\n';
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec[i].size(); j++) {
            cout << '\t' << vec[i][j];
        }
        cout << '\n';
    }
}

void showTransitionColList(vector<TransitionCol> tcol) {
    cout << "transitionMatrix **" << '\n';
    for (int i = 0; i < tcol.size(); i++) {
        cout << "totalOcc = " << tcol[i].totalOcc << '\n';
        cout << "state = ";
        for (int j = 0; j < tcol[i].state.size(); j++) {
            cout << '\t' << tcol[i].state[j];
        }
        cout << '\n';
    }
}

void showState(State st) {
    cout << "State **" << '\n';
    cout << "state = ";
    for (int i = 0; i < st.state.size(); i++) {
        cout << '\t' << st.state[i];
    }
    cout << '\n';
    cout << "occ = " << st.occurrences << '\n';
    cout << "pct = " << st.percentage << '\n';
}

void showTransitionCol(TransitionCol *tcol) {
    cout << "state = ";
    for (int j = 0; j < tcol->state.size(); j++) {
        cout << '\t' << tcol->state[j];
    }
    cout << '\n';
    cout << "totalOcc = " << tcol->totalOcc << '\n';
    cout << "nextStates = " << tcol->nextStates.size() << '\n';
    cout << &tcol->nextStates << endl;
    for (State *state : tcol->nextStates) {
        cout << state << endl;
        showState(*state);
    }
}

struct compare {
    vector<int> key;
    compare(vector<int> i) : key(i) {}

    bool operator()(vector<int> i) {
        return (i == key);
    }
};

struct comparePair {
    pair<vector<int>, vector<int>> key;
    comparePair(pair<vector<int>, vector<int>> i) : key(i) {}

    bool operator()(pair<vector<int>, vector<int>> i) {
        return (i.first == key.first && i.second == key.second);
    }
};

struct compareMatrix {
    vector<int> key;
    compareMatrix(vector<int> i) : key(i) {}

    bool operator()(TransitionCol *i) {
        return (i->state == key);
    }
};

struct compareState {
    vector<int> key;
    compareState(vector<int> i) : key(i) {}

    bool operator()(State *i) {
        // showVector(i->state);
        // showVector(key);
        return (i->state == key);
    }
};

class MarkovChain {
   public:
    vector<vector<int>> stateSpace;
    vector<pair<vector<int>, vector<int>>> transitionSpace;
    vector<TransitionCol *> transitionMatrix;

    void showMatrix(bool isLocal) {
        cout << "---------------------------------" << endl;
        if (isLocal) {
            cout << "LOCAL" << endl;
        } else {
            cout << "GLOBAL" << endl;
        }
        cout << "transitionMatrix size = " << transitionMatrix.size() << endl;
        cout << "transitionMatrix address = " << &transitionMatrix << endl;
        for (TransitionCol *tcol : transitionMatrix) {
            cout << "---------------------------------" << endl;
            cout << "tcol address = " << tcol << endl;
            showTransitionCol(tcol);
        }
    }

    void buildTransition(vector<int> currentState, vector<int> lastState) {
        // cout << "---------------------------------" << endl;
        if (none_of(stateSpace.begin(), stateSpace.end(), compare(currentState))) {
            stateSpace.push_back(currentState);
        }
        // showVector(currentState);
        // showVectorOfVector(stateSpace);
        // showTransitionCol(transitionMatrix);
        if (!lastState.empty()) {
            auto it = find_if(transitionMatrix.begin(), transitionMatrix.end(), compareMatrix(lastState));
            TransitionCol *transitionCol;
            if (it != transitionMatrix.end()) {
                cout << "Found transitionCol" << endl;
                it[0]->totalOcc++;
                transitionCol = it[0];
                // showTransitionCol(transitionCol);
            } else {
                cout << "Not Found transitionCol" << endl;
                transitionCol = new TransitionCol(lastState);
                transitionMatrix.push_back(transitionCol);
            }

            // showMatrix(true);

            auto it2 = find_if(transitionCol->nextStates.begin(), transitionCol->nextStates.end(), compareState(currentState));
            if (it2 != transitionCol->nextStates.end()) {
                cout << "Found nextState" << endl;
                it2[0]->occurrences++;
            } else {
                cout << "Not Found nextState" << endl;
                State *state = new State(currentState);
                transitionCol->nextStates.push_back(state);
            }

            // cout << "nextStates = " << transitionCol->nextStates.size() << endl;

            for (State *state : transitionCol->nextStates) {
                state->percentage = (float)state->occurrences / transitionCol->totalOcc;
                // showState(*state);
            }

            // cout << "nextStates local = " << transitionCol->nextStates.size() << endl;

            pair<vector<int>, vector<int>> transition(lastState, currentState);
            if (none_of(transitionSpace.begin(), transitionSpace.end(), comparePair(transition))) {
                transitionSpace.push_back(transition);
            }

            // showMatrix(true);

            // cout << "nextStates = " << transitionCol.nextStates.size() << endl;
        }
    }

    float getProbability(vector<int> currentState, vector<int> lastState) {
        pair<vector<int>, vector<int>> transition(lastState, currentState);
        if (none_of(transitionSpace.begin(), transitionSpace.end(), comparePair(transition))) {
            return 0.0;
        }
        float prob = 0.0;
        auto it = find_if(transitionMatrix.begin(), transitionMatrix.end(), compareMatrix(lastState));
        if (it != transitionMatrix.end()) {
            auto it2 = find_if(it[0]->nextStates.begin(), it[0]->nextStates.end(), compareState(currentState));
            if (it2 != it[0]->nextStates.end()) {
                prob = it2[0]->percentage;
            }
        }
        return prob;
    }
};

int main() {
    cout << __cplusplus << endl;
    MarkovChain markovChain;
    // TransitionCol col = TransitionCol({0, 1, 1, 1});
    // col.totalOcc = 1;
    // markovChain.transitionMatrix.push_back(col);

    vector<int> currentState = {0, 1, 0, 1};
    vector<int> lastState = {0, 1, 1, 1};
    vector<int> lastState2 = {0, 1, 1, 0};
    markovChain.buildTransition(currentState, lastState);
    markovChain.buildTransition(currentState, lastState);
    markovChain.buildTransition(lastState2, lastState);
    markovChain.buildTransition(lastState, currentState);
    markovChain.showMatrix(false);

    cout << "Prob: " << markovChain.getProbability(currentState, lastState) << endl;
    return 0;
}