#ifndef NAIVE_GREEDY_H
#define NAIVE_GREEDY_H

#include <cstring>

void naive_greedy(vertex *vertices, unsigned int *tour, unsigned int N) {
  bool used[N];
  memset(used, false, N);
  
  // does not matter where we start
  tour[0] = 0;
  used[0] = true;

  for (auto i = 1; i < N; i++) {
    auto bestIndex = -1;
    auto shortestDistance = 10000000.0;

    for (auto j = 0; j < N; j++) {
      if (used[j]) continue;

      auto dist = distance(vertices[tour[i-1]], vertices[j]);
      
      if (bestIndex == -1 || dist < shortestDistance) {
        bestIndex = j;
        shortestDistance = dist;
      }
    }

    tour[i] = bestIndex;
    used[bestIndex] = true;
  }
}

#endif
