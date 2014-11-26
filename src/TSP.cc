#include <stdio.h>

#include "util.h"
#include "naive_greedy.h"
#include "opt2_greedy.h"

int main(void) {
  // Read input
  unsigned int N;
  scanf("%d", &N);
  
  vertex vertices[N];

  for (auto i = 0; i < N; i++) {
    scanf("%lf %lf", &vertices[i].x, &vertices[i].y);
  }

  unsigned int tour[N];
  naive_greedy(vertices, tour, N);

  // Print tour
  for (auto i = 0; i < N; i++) {
    printf("%d\n", tour[i]);
  }

  return 0;
}
