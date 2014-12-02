#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <limits>
#include <chrono>
#include <cmath>
#include <tuple>
#include <random>
#include <cstdlib>

#include "clarke_wright.h"

#include "greedy.h"

#define MAX 1000

#define MAX_NN 40

using namespace std;

chrono::time_point<chrono::system_clock> tsp_begin;
default_random_engine generator;
int ds[MAX][MAX];

// Calculate tour length for the given path.
inline int length(vector<int> path) {
    int l = 0;
    for (int i = 1; i < path.size(); ++i) l += ds[path[i-1]][path[i]];
    l += ds[path[path.size()-1]][path[0]];
    return l;
}

struct nn_sort {

    inline bool operator() (const tuple<int, int>& v1, const tuple<int, int>& v2) {
        return get<1>(v1) < get<1>(v2);
    }

};

void nnList(vector<vector<int>>& nns) {
    int n = nns.size();
    int nn_ls = nns[0].size();
    for (int i = 0; i < n; ++i) {
        vector<tuple<int, int>> helper(n);
        for (int j = 0; j < n; ++j) {
            if (j != i) {
                helper[j] = make_tuple(j, ds[i][j]);
            } else {
                helper[j] = make_tuple(j, 100000000);
            }
        }

        sort(helper.begin(), helper.end(), nn_sort());

        for (int j = 0; j < nn_ls; ++j) {
            nns[i][j] = get<0>(helper[j]);
        }
    }
}


//////// Path construction algorithms ///////////

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

inline void multi_fragment(vector<int>& path) {
    const auto n = path.size();
    int degree[n];
    fill_n(degree, n, 0);
    // TODO: later if we have time
}


//////// Local optimization algorithms ///////////

// Optimize path by exchanging pairs of edges with shorter pairs of edges.
// Returns true if there is time left
// TODO: return if still improving instead
inline bool opt2(vector<int>& path) {
    const auto& n = path.size();
    // TODO: are these limits correct?
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

        if ((chrono::system_clock::now() - tsp_begin).count() > 1600000000) {
            return false;
        }
    }

    return true;
}

// Returns true if the path was made shorter
inline bool opt22(vector<int>& path) {
  bool improved = false;
  const auto& n = path.size();
  for (int i = 0; i < n; i++) {
    for (int j = i+1; j <= n-1; j++) {
      const auto& a = path[i%n],
                  b = path[(i+1)%n], // use mod n to wrap around
                  c = path[j%n],
                  d = path[(j+1)%n];

      auto current = ds[a][b] + ds[c][d];
      auto changed = ds[a][c] + ds[b][d];

      if (changed < current) {
        // TODO: handle case where we wrap around
        // normal case
        reverse(path.begin()+i+1, path.begin()+j+1);
        improved = true;
      }
    }
  }

  return improved;
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
                    // Reverse path between b and c.
                    reverse(path.begin()+i+1, path.begin()+j+1);
                    swap(b, d);
                    swap(c, f);
                }
            }
        }
    }
}

inline void opt33(vector<int>& path) {
    const auto n = path.size();

    for (int i = 0; i < n-5; ++i) {
        for (int j = i+2; j < n-3; ++j) {
            for (int k = j+2; k < n-1; ++k) {
                const auto a = path[i],
                    b = path[i+1],
                    c = path[j],
                    d = path[j+1],
                    e = path[k],
                    f = path[k+1];

                //if (ds[a][b] + ds[c][d] + ds[e][f] <
            }
        }
    }
}

int main() {
    tsp_begin = chrono::system_clock::now();

    // Read graph and calculate distances.
    // Note that the distance matrix is symmetrical.
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
    if (n >= 10) greedy(path, ds);
    else nearest_neighbor_path(0, path);

    opt2(path);
    auto min = length(path);

    // some ideas?
    // Try different start positions as greedy gives different results.
    // Rotation gives better results

    for (int i = 1; i < n; ++i) {
        rotate(path.begin(), path.begin()+1, path.end());
        bool done = false;

        for (int j = 0; j < 10; j++) {
            opt2(path);
            if ((chrono::system_clock::now() - tsp_begin).count() > 1600000000) {
                done = true;
                break;
            }
        }

        if (done) break;
    }
    // Print shortest path.
    for (const auto& v : path) cout << v << endl;

    // Use this pattern to print debug prints without breaking kattis
#ifdef VERBOSE
    cout << "length: " << length(path) << endl;
#endif

    return 0;
}
