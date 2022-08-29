#include "zash-markov.h"

namespace ns3 {

State::State ()
{
}
State::State (vector<int> s)
{
  state = s;
}

TransitionCol::TransitionCol ()
{
}
TransitionCol::TransitionCol (vector<int> s)
{
  state = s;
}

// function for printing the elements in a vector
void
showVector (vector<int> g)
{
  vector<int>::iterator it;
  for (it = g.begin (); it != g.end (); ++it)
    cout << '\t' << *it;
  cout << '\n';
}

void
showVectorOfVector (vector<vector<int>> vec)
{
  cout << "transitionSpace" << '\n';
  for (size_t i = 0; i < vec.size (); i++)
    {
      for (size_t j = 0; j < vec[i].size (); j++)
        {
          cout << '\t' << vec[i][j];
        }
      cout << '\n';
    }
}

void
showTransitionColList (vector<TransitionCol> tcol)
{
  cout << "transitionMatrix **" << '\n';
  for (size_t i = 0; i < tcol.size (); i++)
    {
      cout << "totalOcc = " << tcol[i].totalOcc << '\n';
      cout << "state = ";
      for (size_t j = 0; j < tcol[i].state.size (); j++)
        {
          cout << '\t' << tcol[i].state[j];
        }
      cout << '\n';
    }
}

void
showState (State st)
{
  cout << "State **" << '\n';
  cout << "state = ";
  for (size_t i = 0; i < st.state.size (); i++)
    {
      cout << '\t' << st.state[i];
    }
  cout << '\n';
  cout << "occ = " << st.occurrences << '\n';
  cout << "pct = " << st.percentage << '\n';
}

void
showTransitionCol (TransitionCol *tcol)
{
  cout << "state = ";
  for (size_t j = 0; j < tcol->state.size (); j++)
    {
      cout << '\t' << tcol->state[j];
    }
  cout << '\n';
  cout << "totalOcc = " << tcol->totalOcc << '\n';
  cout << "nextStates = " << tcol->nextStates.size () << '\n';
  cout << &tcol->nextStates << endl;
  for (State *state : tcol->nextStates)
    {
      cout << state << endl;
      showState (*state);
    }
}

compareVec::compareVec (vector<int> i) : key (i)
{
}
bool
compareVec::operator() (vector<int> i)
{
  return (i == key);
}

comparePair::comparePair (pair<vector<int>, vector<int>> i) : key (i)
{
}
bool
comparePair::operator() (pair<vector<int>, vector<int>> i)
{
  return (i.first == key.first && i.second == key.second);
}

compareMatrix::compareMatrix (vector<int> i) : key (i)
{
}
bool
compareMatrix::operator() (TransitionCol *i)
{
  return (i->state == key);
}

compareState::compareState (vector<int> i) : key (i)
{
}
bool
compareState::operator() (State *i)
{
  // showVector(i->state);
  // showVector(key);
  return (i->state == key);
}

void
MarkovChain::showMatrix (bool isLocal)
{
  cout << "---------------------------------" << endl;
  if (isLocal)
    {
      cout << "LOCAL" << endl;
    }
  else
    {
      cout << "GLOBAL" << endl;
    }
  cout << "transitionMatrix size = " << transitionMatrix.size () << endl;
  cout << "transitionMatrix address = " << &transitionMatrix << endl;
  for (TransitionCol *tcol : transitionMatrix)
    {
      cout << "---------------------------------" << endl;
      cout << "tcol address = " << tcol << endl;
      showTransitionCol (tcol);
    }
}

void
MarkovChain::buildTransition (vector<int> currentState, vector<int> lastState)
{
  // cout << "---------------------------------" << endl;
  if (none_of (stateSpace.begin (), stateSpace.end (), compareVec (currentState)))
    {
      stateSpace.push_back (currentState);
    }
  // showVector(currentState);
  // showVectorOfVector(stateSpace);
  // showTransitionCol(transitionMatrix);
  if (!lastState.empty ())
    {
      auto it =
          find_if (transitionMatrix.begin (), transitionMatrix.end (), compareMatrix (lastState));
      TransitionCol *transitionCol;
      if (it != transitionMatrix.end ())
        {
          // cout << "Found transitionCol" << endl;
          it[0]->totalOcc++;
          transitionCol = it[0];
          // showTransitionCol(transitionCol);
        }
      else
        {
          // cout << "Not Found transitionCol" << endl;
          transitionCol = new TransitionCol (lastState);
          transitionMatrix.push_back (transitionCol);
        }

      // showMatrix(true);

      auto it2 = find_if (transitionCol->nextStates.begin (), transitionCol->nextStates.end (),
                          compareState (currentState));
      if (it2 != transitionCol->nextStates.end ())
        {
          // cout << "Found nextState" << endl;
          it2[0]->occurrences++;
        }
      else
        {
          // cout << "Not Found nextState" << endl;
          State *state = new State (currentState);
          transitionCol->nextStates.push_back (state);
        }

      // cout << "nextStates = " << transitionCol->nextStates.size() << endl;

      for (State *state : transitionCol->nextStates)
        {
          state->percentage = (float) state->occurrences / transitionCol->totalOcc;
          // showState(*state);
        }

      // cout << "nextStates local = " << transitionCol->nextStates.size() <<
      // endl;

      pair<vector<int>, vector<int>> transition (lastState, currentState);
      if (none_of (transitionSpace.begin (), transitionSpace.end (), comparePair (transition)))
        {
          transitionSpace.push_back (transition);
        }

      // showMatrix(true);

      // cout << "nextStates = " << transitionCol.nextStates.size() << endl;
    }
}

float
MarkovChain::getProbability (vector<int> currentState, vector<int> lastState)
{
  pair<vector<int>, vector<int>> transition (lastState, currentState);
  if (none_of (transitionSpace.begin (), transitionSpace.end (), comparePair (transition)))
    {
      return 0.0;
    }
  float prob = 0.0;
  auto it = find_if (transitionMatrix.begin (), transitionMatrix.end (), compareMatrix (lastState));
  if (it != transitionMatrix.end ())
    {
      auto it2 = find_if (it[0]->nextStates.begin (), it[0]->nextStates.end (),
                          compareState (currentState));
      if (it2 != it[0]->nextStates.end ())
        {
          prob = it2[0]->percentage;
        }
    }
  return prob;
}

} // namespace ns3
