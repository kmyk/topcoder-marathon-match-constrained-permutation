#include <iostream>
#include <sstream>
#include <cmath>
#include <numeric>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <functional>
#include <cassert>
#define repeat(i, n) for (int i = 0; (i) < int(n); ++(i))
#define repeat_from(i, m, n) for (int i = (m); (i) < int(n); ++(i))
#define repeat_reverse(i, n) for (int i = (n)-1; (i) >= 0; --(i))
#define repeat_from_reverse(i, m, n) for (int i = (n)-1; (i) >= int(m); --(i))
#define whole(x) begin(x), end(x)
using ll = long long;
using namespace std;
template <class T> inline void setmax(T & a, T const & b) { a = max(a, b); }
template <class T> inline void setmin(T & a, T const & b) { a = min(a, b); }

class ConstrainedPermutation { public: vector<int> permute(int N, vector<string> constraints); };

constexpr double ticks_per_sec = 2800000000;
constexpr double ticks_per_sec_inv = 1.0 / ticks_per_sec;
inline double rdtsc() { // in seconds
    uint32_t lo, hi;
    asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return (((uint64_t)hi << 32) | lo) * ticks_per_sec_inv;
}
constexpr int TLE = 10; // sec

default_random_engine gen;

constexpr int MAX_N = 1000;
int n, k;
char lt[MAX_N * MAX_N];
int16_t order[MAX_N];  // int16_t works a little faster
void permute() {
    assert (n <= MAX_N);

    // init
    iota(order, order + n, 0);
    shuffle(order, order + n, gen);
    int satisfied = 0;
    repeat (j, n) repeat (i, j) {
        satisfied += lt[order[i] * n + order[j]];
    }

    // simulated annealing
    double clock_begin = rdtsc();
    double t = 0;
    double temp = INFINITY;
int forced = 0;
    for (int iteration = 0; ; ++ iteration) {
        if (t > 0.9 or iteration % 10 == 0) {
            t = (rdtsc() - clock_begin) / TLE;
            if (t > 0.95) {
cerr << "iteration: " << iteration << endl;
cerr << "forced: " << forced << endl;
cerr << "elapsed: " << t * TLE << endl;
                break;
            }
            temp = (1 - t);
        }
        int i = uniform_int_distribution<int>(0, n - 1)(gen);
        int delta = 0;
        int best = i;
        int best_delta = -1;
        if (iteration % 2 == 0) {
            repeat_from (j, i + 1, n) {
                delta -= lt[order[i] * n + order[j]];
                delta += lt[order[j] * n + order[i]];
                if (best_delta < delta) {
                    best = j;
                    best_delta = delta;
                }
            }
            if (i < best) {
                rotate(order + i, order + i + 1, order + best + 1);
            }
        } else {
            repeat_reverse (j, i) {
                delta -= lt[order[j] * n + order[i]];
                delta += lt[order[i] * n + order[j]];
                if (best_delta < delta) {
                    best = j;
                    best_delta = delta;
                }
            }
            if (best < i) {
                rotate(order + best, order + i, order + i + 1);
            }
        }
        satisfied += max(0, best_delta);
    }
// assert (rdtsc() - clock_begin < TLE);

    // output
// cerr << "p: "; for (int p_i : p) cerr << p_i << ' '; cerr << endl;
cerr << "score: " << satisfied /(double) k << endl;
}

vector<int> ConstrainedPermutation::permute(int a_n, vector<string> constraints_strings) {
    // input
    n = a_n;
    k = constraints_strings.size();
    repeat (i, k) {
        istringstream iss(constraints_strings[i]);
        int x, y;
        iss >> x >> y;
        lt[x * n + y] = true;
    }

    // solve
    ::permute();

    // output
    vector<int> p(n);
    repeat (i, n) {
        p[order[i]] = i;
    }
    return p;
}
