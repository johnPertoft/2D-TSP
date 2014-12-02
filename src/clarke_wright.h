#include <vector>
#include <cstdlib>
#include <ctime>
#include <tuple>
#include <iostream>

#define MAX 1000

using namespace std;

struct savings_sort { // Sorts in decending order

    inline bool operator() (const tuple<int, int, int>& v1, const tuple<int, int, int>& v2) {
        return get<2>(v1) > get<2>(v2);
    }

};

inline void clarke_wright(vector<int>& res, const int ds[][MAX], int n) {
    srand(time(0));

    int hub = rand() % n;
    int ss = ((n - 1) * (n - 2) + 200) / 2;
    vector<tuple<int, int, int>> savings(ss);
    int idx = 0;

    for (int i = 0; i < n; ++i) {
        if (i != hub) {
            for (int j = i + 1; j < n; ++j) {
                if (j != hub && j != i) {
                    auto save = make_tuple(i, j, ds[i][hub] + ds[hub][j] - ds[i][j]);
                    savings[idx] = save;
                    ++idx;
                }
            }
        }
    }

    sort(savings.begin(), savings.end(), savings_sort());

    int savingsSize = idx;
    idx = 0;

    int degrees[n];
    fill_n(degrees, n, 0);

    int pathCount = n - 1;

    int optimalEdges1[n];
    int optimalEdges2[n];
    int optEdgesIdx = 0;

    bool takenFirst = false;

    while (pathCount > 2) {
        auto& tup = savings[idx];
        int start = get<0>(tup);
        int end = get<1>(tup);
        ++idx;
        idx %= savingsSize;

        if ((degrees[start] == 0 && degrees[end] == 1) ||
            (degrees[end] == 0 && degrees[start] == 1) || (!takenFirst)) {
            takenFirst = true;
            optimalEdges1[optEdgesIdx] = start;
            optimalEdges2[optEdgesIdx] = end;
            optEdgesIdx += 1;

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

    int leftFirst = -1;
    int leftSecond = -1;
    for (int i = 0; i < n; ++i) {
        if (degrees[i] != 2 && i != hub) {
            if (leftFirst == -1) leftFirst = i;
            else {
                leftSecond = i;
                break;
            }
        }
    }

    res[0] = leftFirst;
    res[1] = hub;
    res[2] = leftSecond;

    bool usedEdges[optEdgesIdx];
    fill_n(usedEdges, optEdgesIdx, false);
    bool usedVertices[n];
    fill_n(usedVertices, n, false);
    usedVertices[leftFirst] = usedVertices[leftSecond] = usedVertices[hub] = true;

    for (int i = 2; i < n - 1; ++i) { // could be made faster
        int current = res[i];
        for (int j = 0; j < optEdgesIdx; ++j) {
            if (!usedEdges[j]) {
                int opt1 = optimalEdges1[j];
                int opt2 = optimalEdges2[j];
                if (opt1 == current || opt2 == current) {
                    int next = opt1 == current ? opt2 : opt1;
                    if (!usedVertices[next]) {
                        res[i + 1] = next;
                        usedEdges[j] = true;
                        usedVertices[next] = true;
                        break;
                    }
                }
            }
        }
    }

    bool used[n];
    fill_n(used, n, false);
    for (int i = 0; i < n; ++i) used[res[i]] = true;

    for (int i = 0; i < n; ++i) if (!used[i]) throw "not using all edges";

    for (int i = 0; i < n; ++i) if (!usedVertices[i]) throw "not using all edges.";
}
