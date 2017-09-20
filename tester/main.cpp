// C++11
#include "ConstrainedPermutation.cpp"

using namespace std;

// -------8<------- end of solution submitted to the website -------8<-------

int main() {
    ConstrainedPermutation cp;
    int N, K;
    cin >> N >> K;
    vector<string> constraints(K);
    for (int k = 0; k < K; ++k) {
        int i, j;
        cin >> i >> j;
        constraints[k] = to_string(i) + " " + to_string(j);
    }

    vector<int> ret = cp.permute(N, constraints);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
