#ifndef MARKOV
#define MARKOV

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
// #include <chrono>
// #include <thread>
using namespace std;

namespace ns3 {

class State
{
public:
  vector<int> state;
  int occurrences = 1;
  float percentage = 0.0;
  State ();
  State (vector<int> s);
};

class TransitionCol
{
public:
  vector<int> state;
  vector<State *> nextStates;
  int totalOcc = 1;
  TransitionCol ();
  TransitionCol (vector<int> s);
};

// function for printing the elements in a vector
void showVector (vector<int> g);

void showVectorOfVector (vector<vector<int>> vec);

void showTransitionColList (vector<TransitionCol> tcol);

void showState (State st);

void showTransitionCol (TransitionCol *tcol);

struct compareVec
{
  vector<int> key;
  compareVec (vector<int> i);

  bool operator() (vector<int> i);
};

struct comparePair
{
  pair<vector<int>, vector<int>> key;
  comparePair (pair<vector<int>, vector<int>> i);

  bool operator() (pair<vector<int>, vector<int>> i);
};

struct compareMatrix
{
  vector<int> key;
  compareMatrix (vector<int> i);

  bool operator() (TransitionCol *i);
};

struct compareState
{
  vector<int> key;
  compareState (vector<int> i);

  bool operator() (State *i);
};

class MarkovChain
{
public:
  vector<vector<int>> stateSpace;
  vector<pair<vector<int>, vector<int>>> transitionSpace;
  vector<TransitionCol *> transitionMatrix;

  void showMatrix (bool isLocal);

  void buildTransition (vector<int> currentState, vector<int> lastState);

  float getProbability (vector<int> currentState, vector<int> lastState);
};
} // namespace ns3

// int main() {
//     cout << __cplusplus << endl;

//     // time_t t1, t2;
//     // time(&t1);
//     // this_thread::sleep_for(chrono::milliseconds(1000));
//     // time(&t2);
//     // cout << difftime(t1, t2) << endl;
//     MarkovChain markovChain;
//     // TransitionCol col = TransitionCol({0, 1, 1, 1});
//     // col.totalOcc = 1;
//     // markovChain.transitionMatrix.push_back(col);

//     vector<int> currentState = {0, 1, 0, 1};
//     vector<int> lastState = {0, 1, 1, 1};
//     vector<int> lastState2 = {0, 1, 1, 0};
//     markovChain.buildTransition(currentState, lastState);
//     markovChain.buildTransition(currentState, lastState);
//     markovChain.buildTransition(lastState2, lastState);
//     markovChain.buildTransition(lastState, currentState);
//     markovChain.showMatrix(false);

//     cout << "Prob: " << markovChain.getProbability(currentState, lastState)
//     << endl; return 0;
// }

#endif
