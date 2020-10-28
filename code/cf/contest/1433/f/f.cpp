/*  Author: ysj1173886760
 *  Date: 2020-10-21-19-24
 *  Language: C++
 *  Enjoy Coding!
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<cmath>
using namespace std;

int a[75][75];
int dp[72][72][36][72];

const int inf = 0x3f3f3f3f;

int main() {
    int n, m, k;
    cin >> n >> m >> k;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m ;j++) {
            cin >> a[i][j];
        }
    }
    memset(dp, -0x3f, sizeof(dp));
    dp[0][m][0][0] = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            for (int cnt = m/2; cnt >= 0; cnt--) {
                for (int rem = k - 1; rem >= 0; rem--) {
                    int lstx = (j == 1) ? i - 1: i;
                    int lsty = (j == 1) ? m: j - 1;    
                    if (i != lstx) {
                        dp[i][j][0][rem] = max(dp[i][j][0][rem], dp[lstx][lsty][cnt][rem]); // just don`t pick the element
                        dp[i][j][1][rem] = max(dp[i][j][1][rem], dp[lstx][lsty][cnt][(rem - a[i][j] % k + k) % k] + a[i][j]);
                    } else {
                        dp[i][j][cnt][rem] = max(dp[i][j][cnt][rem], dp[lstx][lsty][cnt][rem]);
                        if (cnt) dp[i][j][cnt][rem] = max(dp[i][j][cnt][rem], dp[lstx][lsty][cnt - 1][(rem - a[i][j] %k + k) % k] + a[i][j]);
                    }
                }
            }
        }
    }
    int ans = 0;
    for (int i = m/2; i >= 0; i--) {
        ans = max(ans, dp[n][m][i][0]);
    }
    cout << ans << endl;
    return 0;
}
