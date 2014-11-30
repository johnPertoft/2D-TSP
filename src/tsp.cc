#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <limits>
#include <chrono>
#include <cmath>
#define MAX 1000
using namespace std;
int ds[MAX][MAX];

// Calculate tour length for the given path.
inline int length(vector<int> path) {
  int l = 0;
  for (int i = 1; i < path.size(); ++i) l += ds[path[i-1]][path[i]];
  l += ds[path[path.size()-1]][path[0]];
  return l;
}

// Find decent path with nearest neighbor.
inline void nearest_neighbor_path(int start, vector<int>& path) {
  const auto& n = path.size();
  list<int> l;
  for (int i = 0; i < n; ++i) l.emplace_back((start+i) % n);

  for (int i = 0; i < n; ++i) {
    auto min = l.begin();
    for (auto k = l.begin(); k != l.end(); ++k) {
      const auto& d1 = ds[path[i-1]][*k], 
      d2 = ds[path[i-1]][*min];
      if (d1 < d2) min = k;
    }
    
    path[i] = *min;
    l.erase(min);
  }
}

// Optimize path by exchanging pairs of edges with shorter pairs of edges.
inline void opt2(vector<int>& path) {
  const auto& n = path.size();
  for (int i = 0; i < n-1; ++i) {
    for (int j = i+1; j < n-1; ++j) {
      const auto& a = path[i],
      b = path[i+1],
      c = path[j],
      d = path[j+1];
      if (ds[a][c] + ds[b][d] < ds[a][b] + ds[c][d]) {
         // Reverse path between b and c.
        reverse(path.begin()+i+1, path.begin()+j+1);
      }
    }
  }
}

// Optimize path by exchanging edge triplets with shorter edge triplets.
inline void opt3(vector<int>& path) {
  const auto& n = path.size();
  for (int i = 0; i < n-1; ++i) {
    for (int j = i+1; j < n-1; ++j) { 
      for (int k = j+1; k < n-1; ++k) { 
        auto& a = path[i],
        b = path[i+1],
        c = path[j],
        d = path[j+1],
        e = path[k],
        f = path[k+1];
        //TODO Try the other way of combining also. ABCDEF -> ADECBF
        if (ds[a][b] + ds[c][d] + ds[e][f] > ds[a][d] + ds[b][e] + ds[c][f]) {
          reverse(path.begin()+i+1, path.begin()+j+1); // Reverse path between b and c.
          swap(b, d);
          swap(c, f);
        }
      }
    }
  }
}

int main() {
  const auto begin = chrono::system_clock::now();

  // Read graph and calculate distances. Note that the distance matrix is symmetrical.
  int n;
  cin >> n;
  double* Xs = new double[n];
  double* Ys = new double[n];
  for (int i = 0; i < n; ++i) cin >> Xs[i] >> Ys[i];
  for (int i = 0; i < n; ++i) {
    for (int j = i; j < n; ++j) {
      int dist = round(sqrt(pow(Xs[i]-Xs[j], 2) + pow(Ys[i]-Ys[j], 2))); 
      ds[i][j] = ds[j][i] = dist;
    }
  }

  // Construct decent path with nearest neighbor.
  vector<int> path (n);
  nearest_neighbor_path(0, path);
  opt2(path);
  //opt3(path); // TODO Fix.
  auto min = length(path);

  // Try different start positions as greedy gives different results.
  vector<int> p (n);
  vector<int> starts (n);
  for (int i = 1; i < starts.size(); ++i) starts[i] = i;
  random_shuffle(starts.begin(), starts.end());
  
  for (const auto& s : starts) {
    // Find path greedy.
    nearest_neighbor_path(s, p);
    opt2(path);
    auto l = length(p);

    // Keep shortest.
    if (l < min) {
      path = p;
      min = l;
    }

    // Abort if out of time.
    if ((chrono::system_clock::now() - begin).count() > 1500000000) break;
  }

  // temp testing stuff
  // this rotation gives improvement, need to make it more general though
  rotate(path.begin(), path.begin()+1, path.end());
  opt2(path);
  opt2(path);
  ////////////////

  // Print shortest path.
  for (const auto& v : path) cout << v << endl;

  #ifdef VERBOSE
  cout << "length: " << length(path) << endl; 
  #endif

  return 0;
}


