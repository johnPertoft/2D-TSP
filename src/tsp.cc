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
#include <cmath>
#include <ctime>

#include "clarke_wright.h"

#include "greedy.h"

#define MAX 1000

#define TIME_LIMIT 1750000000 // PROD

//#define TIME_LIMIT 1600000 // MAC

#define MAX_NN 25

bool dlb[MAX];

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

void nnList(vector<vector<int>>& nns, int max_nn) {
    int n = nns.size();
    for (int i = 0; i < n; ++i) {
        vector<tuple<int, int>> helper(n);
        for (int j = 0; j < n; ++j) {
            if (j != i) {
                helper[j] = make_tuple(j, ds[i][j]);
            } else {
                helper[j] = make_tuple(j, 10000000000);
            }
        }

        sort(helper.begin(), helper.end(), nn_sort());

        for (int j = 0; j < max_nn; ++j) {
            nns[i][j] = get<0>(helper[j]);
        }
    }
}


//////// Path construction algorithms ///////////

// Find decent path with nearest neighbor.
inline void nearest_neighbor_path(int start, vector<int>& path) {
    const auto n = path.size();
    list<int> l;
    for (int i = 0; i < n; ++i) l.emplace_back((start+i) % n);

    for (int i = 0; i < n; ++i) {
        auto min = l.begin();
        for (auto k = l.begin(); k != l.end(); ++k) {
            const auto d1 = ds[path[i-1]][*k],
                d2 = ds[path[i-1]][*min];
            if (d1 < d2) min = k;
        }

        path[i] = *min;
        l.erase(min);
    }
}

inline void rev(vector<int>& path, vector<int>& indices, int s, int e) {
    int n = path.size();
    int i = s - 1;
    int j = e;
    int diff = -1;
    if (i + 1 < j) diff = j - (i + 1);
    else {
        diff = n - (i + 1) + j + 1;
    }

    diff = (diff + 1) / 2;

    int start = i + 1;
    int end = j;
    int tmp = -1;

    while (diff > 0) {
        start %= n;
        end = end == -1 ? n - 1 : end;

        swap(path[start], path[end]);
  
        //indices[path[start]] = start;
        //indices[path[end]] = end;

        ++start;
        --end;
        --diff;
    }
    
    for (int i = 0; i < n; ++i) {
        indices[path[i]] = i;
    }
}


//////// Local optimization algorithms ///////////

inline bool opt2_greedy(
                        vector<int>& path,
                        vector<int>& indices,
                        vector<vector<int>>& nns,
                        int max_nn) {
    const int n = path.size();
    bool improved = true;
    while (improved) {
        improved = false;

        for (int i = 0; i < n - 1; ++i) {
            for (int k = 0; k < max_nn - 1; ++k) { // nns => vertices
                const int a = path[i];
                const int b = path[i + 1];

                int vert = nns[a][k];

                int j = indices[vert]; // vertex to index

                const int c = vert; // first vertex
                const int d = path[(j + 1) % n]; // second vertex
                if (b == c || b == d) continue;

                if (ds[a][c] + ds[b][d] < ds[a][b] + ds[c][d]) {
                    improved = true;

                    rev(path, indices, i + 1, j);
                }
            }
        }

        if ((chrono::system_clock::now() - tsp_begin).count() > TIME_LIMIT) {
            return false;
        }
    }

    return true;
}


// Perform a random double bridge move.
/*
inline void perturb(vector<int>& p, vector<int>& indices) {
    const auto n = p.size();
    if (n < 8) return;

    static random_device rd;
    static uniform_int_distribution<int> d(2, n - 3);
    static auto r = bind(d, ref(rd));

    for (int i = 0; i < n / 10; ++i) {
        auto pos = r();

        int s1 = pos - 1;
        int s2 = pos + 1;

        int tmp = p[s1];
        p[s1] = p[s2];
        p[s2] = tmp;

        indices[p[s2]] = s2;
        indices[p[s1]] = s1;
    }
}
*/

static uint64_t x = time(0)*1000;

int xor_rand(int m) {
  x ^= x << 11;
  x ^= x >> 35;
  x ^= x << 4;
  x *= 2685821657736338717LL;
  int out = x % m;
  return (out < 0) ? -out : out;
}

inline void perturb(vector<int>& p, vector<int>& indices) {
  const auto& n = p.size();
  int num_shuffles = n * 0.15;
  for (int x = 0; x < num_shuffles; x++) {
    int index = xor_rand(n);
    if (index == n - 1) {
      swap(p[index], p[0]);
      swap(indices[p[index]], indices[p[0]]);
    
    } else {
      swap(p[index], p[index+1]);
      swap(indices[p[index]], indices[p[index+1]]);
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

    int max_nn = min(MAX_NN, n);
    vector<vector<int>> nns(n, vector<int>(max_nn));
    nnList(nns, max_nn);

    vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[path[i]] = i;
    }

    fill_n(dlb, n, false);
    opt2_greedy(path, indices, nns, max_nn);

    int best = length(path);
    vector<int> bestPath = path;

    int iters = 0;

    while (true) {
        ++iters;

        perturb(path, indices); // Perturb that stuffs

        bool tle = opt2_greedy(path, indices, nns, max_nn);

        int l = length(path);

        //cout << l << endl;

        if (l < best) {
            bestPath = path;
            best = l;
        }

        if (!tle) {
            break;
        }
    }
    
    for (const auto v : bestPath) cout << v << endl;

    // Use this pattern to print debug prints without breaking kattis
#ifdef VERBOSE
    cout << "length: " << length(bestPath) << endl;
    cout << "iters: " << iters << endl;
#endif

    return 0;
}
