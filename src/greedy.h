#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

#include "api_disjoint_set.h"

#define MX 1000

struct greedy_sort {

    inline bool operator() (const tuple<int, int, int>& v1, const tuple<int, int, int>& v2) {
        return get<2>(v1) < get<2>(v2);
    }

};

inline void greedy(vector<int>& res, const int ds[][MX]) {
    int n = res.size();
    bool used[n];
    fill_n(used, n, false);
    int ss = ((n - 1) * n / 2);
    vector<tuple<int, int, int>> distances(ss);

    int idx = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (j != i) {
                distances[idx] = make_tuple(i, j, ds[i][j]);
                ++idx;
            }
        }
    }

    sort(distances.begin(), distances.end(), greedy_sort());

    int distancesSize = idx;
    idx = 0;

    int degrees[n];
    fill_n(degrees, n, 0);

    int pathCount = n;

    int optimalEdges1[n];
    int optimalEdges2[n];
    int optEdgesIdx = 0;

    bool takenFirst = false;

    popup::disjoint_set disjointSet;

    for (int i = 0; i < n; ++i) {
        disjointSet.make_set(i);
    }

    while (pathCount > 0) {
        auto& tup = distances[idx];
        int start = get<0>(tup);
        int end = get<1>(tup);
        ++idx;
        idx %= distancesSize;

        if (degrees[start] < 2 && degrees[end] < 2 &&
            (disjointSet.find(start) != disjointSet.find(end) || pathCount == 2)) {

            disjointSet.union_elements(start, end);

            optimalEdges1[optEdgesIdx] = start;
            optimalEdges2[optEdgesIdx] = end;
            ++optEdgesIdx;

            ++degrees[start];
            ++degrees[end];

            if (degrees[start] == 2) {
                --pathCount;
            }

            if (degrees[end] == 2) {
                --pathCount;
            }
        }
    }

    res[0] = optimalEdges1[0];
    res[1] = optimalEdges2[0];

    bool usedEdges[n];
    fill_n(usedEdges, n, false);
    usedEdges[0] = true;

    for (int i = 2; i < n; ++i) {
        int current = res[i - 1];
        int before = res[i - 2];
        for (int j = 1; j < n; ++j) {
            if (!usedEdges[j]) {
                int start = optimalEdges1[j];
                int end = optimalEdges2[j];

                if (start == current && end != before) {
                    res[i] = end;
                    usedEdges[j] = true;
                    break;
                } else if (start != before && end == current) {
                    res[i] = start;
                    usedEdges[j] = true;
                    break;
                }
            }
        }
    }
}
