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

int compute_satisfied_of(int i, vector<unsigned> const & p, vector<pair<int, int> > const & constraints) {
    int n = p.size();
    int satisfied = 0;
    repeat (j, n) {
        satisfied += binary_search(whole(constraints), p[i] < p[j] ? make_pair(i, j) : make_pair(j, i));
    }
    return satisfied;
}
int compute_satisfied(vector<unsigned> const & p, vector<pair<int, int> > const & constraints) {
    int n = p.size();
    int satisfied = 0;
    repeat (i, n) {
        satisfied += compute_satisfied_of(i, p, constraints);
    }
    assert (satisfied % 2 == 0);
    return satisfied / 2;
}
int compute_satisfied_of(int i, vector<unsigned> const & p, vector<vector<int> > const & lt, vector<vector<int> > const & gt) {
    int satisfied = 0;
    for (int j : lt[i]) satisfied += p[i] < p[j];
    for (int j : gt[i]) satisfied += p[j] < p[i];
    return satisfied;
}

double compute_score(vector<unsigned> const & p, vector<pair<int, int> > const & constraints) {
    int k = constraints.size();
    return compute_satisfied(p, constraints) /(double) k;
}

vector<unsigned> permute(int n, vector<pair<int, int> > const & constraints) {
    // make graphs
    vector<vector<int> > lt(n), gt(n);
    for (auto constraint : constraints) {
        int x, y; tie(x, y) = constraint;
        lt[x].push_back(y);
        gt[y].push_back(x);
    }
    repeat (i, n) {
        sort(whole(lt[i]));
        sort(whole(gt[i]));
    }

    // init
    set<int> used;
    vector<unsigned> p(n);
    repeat (i, n) {
        while (used.count(p[i])) {
            p[i] = uniform_int_distribution<unsigned>()(gen);
        }
        used.insert(p[i]);
    }
    int satisfied = compute_satisfied(p, constraints);

    // simulated annealing
    int best_satisfied = satisfied;
    vector<unsigned> result = p;
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
        int p_i = p[i];
        while (used.count(p_i)) {
            p_i = uniform_int_distribution<unsigned>()(gen);
        }
        int delta = 0;
        delta -= compute_satisfied_of(i, p, lt, gt);
        int previous_p_i = p[i];
        p[i] = p_i;
        delta += compute_satisfied_of(i, p, lt, gt);
// assert (satisfied + delta == compute_satisfied(p, constraints));
        if (delta >= 0 or bernoulli_distribution(exp(delta / temp))(gen)) {
// if (delta < 0) cerr << "prob: " << exp(delta / temp) << endl;
forced += (delta < 0);
            satisfied += delta;
            used.erase(previous_p_i);
            used.insert(p[i]);
            if (best_satisfied < satisfied) {
                best_satisfied = satisfied;
                result = p;
// cerr << "score: " << satisfied /(double) k << endl;
            }
        } else {
            p[i] = previous_p_i;
        }
    }
// assert (rdtsc() - clock_begin < TLE);

    // output
// cerr << "p: "; for (int p_i : p) cerr << p_i << ' '; cerr << endl;
cerr << "score: " << compute_score(result, constraints) << endl;
    return result;
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
    vector<unsigned> relaxed = ::permute(n, constraints);

    // output
    vector<int> p = apply_compression(coordinate_compression_map(relaxed), relaxed);
    return p;
}
