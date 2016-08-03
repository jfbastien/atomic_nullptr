#include <atomic>
#include <cstddef>
#include <stdio.h>
#include <thread>

int main() {
  struct S {
    volatile unsigned char pad0;
    std::atomic<std::nullptr_t> atom;
    volatile unsigned char pad1;
  };
  alignas(double) S s;
  s.pad0 = s.pad1 = 0;
  s.atom.store(nullptr);

  printf("s\talign: %zu size: %zu\n", alignof(s), sizeof(s));
  printf("pad0\talign: %zu size: %zu offset: %zu\n", alignof(s.pad0), sizeof(s.pad0), offsetof(S, pad0));
  printf("atom\talign: %zu size: %zu offset: %zu\n", alignof(s.atom), sizeof(s.atom), offsetof(S, atom));
  printf("pad1\talign: %zu size: %zu offset: %zu\n", alignof(s.pad1), sizeof(s.pad1), offsetof(S, pad1));
  fflush(stdout);

  std::thread threads[] = {
    std::thread([&s]() { while (1) ++s.pad0; }),
    std::thread([&s]() { while (1) ++s.pad1; }),
    std::thread([&s]() {
        std::nullptr_t null = nullptr;
        while (1) {
          while (s.atom.compare_exchange_strong(null, null))
            ;
          fprintf(stderr, "Oops!\n");
        }
    })
  };
  for (auto &t : threads) t.join();

  return 0;
}
