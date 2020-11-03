/*  Author: ysj1173886760
 *  Date: 2020-11-02-23-50
 *  Language: C++
 *  Enjoy Coding!
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<cmath>
using namespace std;

void solve() {
    int n;
    cin >> n;
    vector<int> a(n + 1);
    for (int i = 1; i <= n; i++) cin >> a[i];
    int l = 1e9, r = 0;
    for (int i = 1; i <= n; i++) {
        if (a[i] > l + r) {
            r = a[i] - l;
        } else {
            l = a[i] - r;
        }
    }
    if (l < 0) cout << "NO" << endl;
    else cout << "YES" << endl;
}
int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
