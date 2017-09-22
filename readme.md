# TopCoder Marathon Match: Education Week MM. ConstrainedPermutation

-   Problem Statement: <https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16997&pm=14690>
-   Standings: <https://community.topcoder.com/longcontest/?module=ViewStandings&rd=16997>
-   Submit: <https://community.topcoder.com/longcontest/?module=Submit&compid=57374&rd=16997&cd=15273>

## memo

-   山登り
    -   愚直 近傍はswap
    -   なんかTLEった
-   焼き鈍し
    -   ちょっと下がった
-   swapのたびにn個全部舐めてたのを、graph作って隣接するやつだけ見るように
    -   100倍高速に
    -   TLE内に10000000回ぐらいのswapできるように
    -   焼き鈍さない場合との差は縮まる
    -   速くなったからと油断して提出したらまたTLEった
-   swap以外の近傍
    -   問題になるのは相対的なそれだけなので、shiftみたいな操作
    -   permutationであるという制約をdistinctであるに緩和すれば高速にできる
-   提出: 7e5b58b01b725ed09bb46cfe5dbc0481d9a74de3
    -   822174.65 点
    -   提出時 2位
-   wleiteさんの方針はforumを読む限りswap+SA
-   制約違反の多い点を優先的に選択
