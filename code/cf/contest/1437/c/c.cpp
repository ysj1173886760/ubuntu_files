/*  Author: ysj1173886760
 *  Date: 2020-10-27-23-12
 *  Language: C++
 *  Enjoy Coding!
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<cmath>
using namespace std;

const int INF = 0x3f3f3f3f;

void solve() {
    int n;
    cin >> n;
    vector<int> a(n + 1);
    for (int i = 1; i <= n; i++) cin >> a[i];
    vector<vector<int>> dp (n + 1, vector<int>(2 * n + 1, INF));
    sort(a.begin(), a.end());
    for (int i = 0; i <= 2 * n; i++) dp[0][i] = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= 2 * n; j++) {
            dp[i][j] = min(dp[i][j - 1], dp[i - 1][j - 1] + abs(a[i] - j));
        }
    }
    int ans = INF;
    for (int i = 1; i <= 2 * n; i++) ans = min(ans, dp[n][i]);
    cout << ans << endl;
}
int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
