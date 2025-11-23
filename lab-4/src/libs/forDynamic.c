#include "../../include/contracts.h"
#include "../../include/simpleBool.h"
#include <stdlib.h>

int prime_count(int a, int b) {
  if (b == 1 || a > b) {
    return 0;
  }

  char *is_prime = malloc(sizeof(char) * (b + 1));
  is_prime[0] = FALSE, is_prime[1] = FALSE;
  for (size_t i = 2; i <= b; ++i) {
    is_prime[i] = TRUE;
  }

  for (int d = 2; d * d <= b; ++d) {
    if (is_prime[d]) {
      for (int _d = d * d; _d <= b; _d += d) {
        is_prime[_d] = FALSE;
      }
    }
  }

  int count = 0;
  for (int d = a; d <= b; ++d) {
    if (is_prime[d]) {
      ++count;
    }
  }

  free(is_prime);
  return count;
}

float pi(int k) {
  float composition = 1.0;
  for (int i = 1; i <= k; ++i) {
    composition *= (4.0 * i * i) / (4.0 * i * i - 1.0);
  }
  return composition * 2.0;
}
