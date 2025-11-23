#include "../include/contracts.h"
#include "../include/simpleBool.h"

int is_prime(int number) {
  if (number == 1) {
    return FALSE;
  }
  for (int d = 2; d * d <= number; ++d) {
    if (number % d == 0) {
      return FALSE;
    }
  }
  return TRUE;
}

int prime_count(int a, int b) {
  if (b == 1 || a > b) {
    return 0;
  }

  int count = 0;
  for (int number = a; number <= b; ++number) {
    if (is_prime(number)) {
      ++count;
    }
  }
  return count;
}
