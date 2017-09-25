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
#define unittest_name_helper(counter) unittest_ ## counter
#define unittest_name(counter) unittest_name_helper(counter)
#define unittest __attribute__((constructor)) void unittest_name(__COUNTER__) ()
using ll = long long;
using namespace std;
template <class T> inline void setmax(T & a, T const & b) { a = max(a, b); }
template <class T> inline void setmin(T & a, T const & b) { a = min(a, b); }

class ConstrainedPermutation { public: vector<int> permute(int N, vector<string> constraints); };

template <typename T>
map<T, int> coordinate_compression_map(vector<T> const & xs) {
    int n = xs.size();
    vector<int> ys(n);
    iota(whole(ys), 0);
    sort(whole(ys), [&](int i, int j) { return xs[i] < xs[j]; });
    map<T,int> f;
    for (int i : ys) {
        if (not f.count(xs[i])) { // make unique
            int j = f.size();
            f[xs[i]] = j; // f[xs[i]] has a side effect, increasing the f.size()
        }
    }
    return f;
}

template <typename T>
vector<int> apply_compression(map<T,int> const & f, vector<T> const & xs) {
    int n = xs.size();
    vector<int> ys(n);
    repeat (i,n) ys[i] = f.at(xs[i]);
    return ys;
}

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
uint16_t p[MAX_N];
uint16_t result[MAX_N];
bool used[1 << 16];
int16_t graph[MAX_N * MAX_N];
int lt[MAX_N + 1];
int gt[MAX_N + 1];
vector<pair<int, int> > constraints;

int compute_satisfied_delta_of(int i, int from, int to) {
    int satisfied = 0;
    for (int16_t *j = graph + lt[i], *last = graph + lt[i + 1]; j != last; ++ j) satisfied += - (from < p[*j]) + (to < p[*j]);
    for (int16_t *j = graph + gt[i], *last = graph + gt[i + 1]; j != last; ++ j) satisfied += - (from > p[*j]) + (to > p[*j]);
    return satisfied;
}
int compute_satisfied_of(int i) {
    int satisfied = 0;
    int p_i = p[i];
    for (int16_t *j = graph + lt[i], *last = graph + lt[i + 1]; j != last; ++ j) satisfied += p_i < p[*j];
    for (int16_t *j = graph + gt[i], *last = graph + gt[i + 1]; j != last; ++ j) satisfied += p_i > p[*j];
    return satisfied;
}
int compute_satisfied() {
    int satisfied = 0;
    repeat (i, n) {
        satisfied += compute_satisfied_of(i);
    }
    assert (satisfied % 2 == 0);
    return satisfied / 2;
}

void permute() {
    { // make graphs
        int cnt = 0;

        sort(whole(constraints));
        int last_x = -1;
        for (auto constraint : constraints) {
            int x, y; tie(x, y) = constraint;
            while (last_x < x) {
                lt[++ last_x] = cnt;
            }
            graph[cnt ++] = y;
        }
        lt[++ last_x] = cnt;

        sort(whole(constraints), [&](pair<int, int> a, pair<int, int> b) {
            return make_pair(a.second, a.first) < make_pair(b.second, b.first);
        });
        int last_y = -1;
        for (auto constraint : constraints) {
            int x, y; tie(x, y) = constraint;
            while (last_y < y) {
                gt[++ last_y] = cnt;
            }
            graph[cnt ++] = x;
        }
        gt[++ last_y] = cnt;
    }

    // init
    repeat (i, n) {
        do {
            p[i] = uniform_int_distribution<uint16_t>()(gen);
        } while (used[p[i]]);
        used[p[i]] = true;
    }
    int satisfied = compute_satisfied();

    // simulated annealing
    int best_satisfied = satisfied;
    copy(p, p + n, result);
    double clock_begin = rdtsc();
    double t = 0;
    double temp = INFINITY;
int forced = 0;
    for (int iteration = 0; ; ++ iteration) {
        if (t > 0.9 or iteration % 1024 == 0) {
            t = (rdtsc() - clock_begin) / TLE;
            if (t > 0.97) {
cerr << "iteration: " << iteration << endl;
cerr << "forced: " << forced << endl;
cerr << "elapsed: " << t * TLE << endl;
                break;
            }
            temp = (1 - t) / 32;
        }
        int i = uniform_int_distribution<int>(0, n - 1)(gen);
        int p_i;
        do {
            p_i = uniform_int_distribution<uint16_t>()(gen);
        } while (used[p_i]);
        int delta = compute_satisfied_delta_of(i, p[i], p_i);
// assert (satisfied + delta == compute_satisfied(p, constraints));
        if (delta >= 0 or bernoulli_distribution(exp(delta / temp))(gen)) {
// if (delta < 0) cerr << "prob: " << exp(delta / temp) << endl;
forced += (delta < 0);
            satisfied += delta;
            used[p[i]] = false;
            used[p_i] = true;
            p[i] = p_i;
            if (t > 0.7 and best_satisfied < satisfied) {
                best_satisfied = satisfied;
                copy(p, p + n, result);
// cerr << "score: " << satisfied /(double) k << endl;
            }
        }
    }
// assert (rdtsc() - clock_begin < TLE);
cerr << "score: " << best_satisfied /(double) k << endl;
}

vector<int> ConstrainedPermutation::permute(int a_n, vector<string> constraints_strings) {
    // input
    n = a_n;
    k = constraints_strings.size();
    repeat (i, k) {
        istringstream iss(constraints_strings[i]);
        int x, y; iss >> x >> y;
        constraints.emplace_back(x, y);
    }

    // solve
    ::permute();

    // output
    vector<uint16_t> relaxed(result, result + n);
    return apply_compression(coordinate_compression_map(relaxed), relaxed);
}
