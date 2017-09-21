#include <iostream>
#include <sstream>
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
#define unittest_name_helper(counter) unittest_ ## counter
#define unittest_name(counter) unittest_name_helper(counter)
#define unittest __attribute__((constructor)) void unittest_name(__COUNTER__) ()
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

int compute_satisfied_of(int i, vector<int> const & p, vector<pair<int, int> > const & constraints) {
    int n = p.size();
    int satisfied = 0;
    repeat (j, n) {
        satisfied += binary_search(whole(constraints), p[i] < p[j] ? make_pair(i, j) : make_pair(j, i));
    }
    return satisfied;
}
int compute_satisfied(vector<int> const & p, vector<pair<int, int> > const & constraints) {
    int n = p.size();
    int satisfied = 0;
    repeat (i, n) {
        satisfied += compute_satisfied_of(i, p, constraints);
    }
    assert (satisfied % 2 == 0);
    return satisfied / 2;
}

double compute_score(vector<int> const & p, vector<pair<int, int> > const & constraints) {
    int k = constraints.size();
    return compute_satisfied(p, constraints) /(double) k;
}

vector<int> ConstrainedPermutation::permute(int n, vector<string> constraints_strings) {
    // input
    int k = constraints_strings.size();
    vector<pair<int, int> > constraints(k);
    repeat (i, k) {
        istringstream iss(constraints_strings[i]);
        iss >> constraints[i].first >> constraints[i].second;
    }
    sort(whole(constraints));

    // solve
    vector<int> p(n);
    iota(whole(p), 0);
    int satisfied = compute_satisfied(p, constraints);
    double clock_begin = rdtsc();
    for (int iteration = 0; ; ++ iteration) {
        double clock_end = rdtsc();
        if (clock_end - clock_begin > TLE * 0.95) {
cerr << "iteration: " << iteration << endl;
cerr << "elapsed: " << clock_end - clock_begin << endl;
            break;
        }
        int x = uniform_int_distribution<int>(0, n - 1)(gen);
        int y = uniform_int_distribution<int>(0, n - 1)(gen);
        if (x > y) swap(x, y);
        int delta = 0;
        delta -= compute_satisfied_of(x, p, constraints);
        delta -= compute_satisfied_of(y, p, constraints);
        delta += binary_search(whole(constraints), p[x] < p[y] ? make_pair(x, y) : make_pair(y, x));
        swap(p[x], p[y]);
        delta += compute_satisfied_of(x, p, constraints);
        delta += compute_satisfied_of(y, p, constraints);
        delta -= binary_search(whole(constraints), p[x] < p[y] ? make_pair(x, y) : make_pair(y, x));
// assert (satisfied + delta == compute_satisfied(p, constraints));
        if (delta >= 0) {
            satisfied += delta;
// if (delta) cerr << "Score = " << satisfied /(double) k << endl;
        } else {
            swap(p[x], p[y]);
        }
    }
// assert (rdtsc() - clock_begin < TLE);

    // output
// cerr << "p: "; for (int p_i : p) cerr << p_i << ' '; cerr << endl;
cerr << "score: " << compute_score(p, constraints) << endl;
    return p;
}
