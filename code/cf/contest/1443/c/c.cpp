/*  Author: ysj1173886760
 *  Date: 2020-11-02-23-39
 *  Language: C++
 *  Enjoy Coding!
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<cmath>
using namespace std;

struct info {
    int a, b;
};

bool cmp(const info &a, const info &b) {
    return a.b > b.b;
}
void solve() {
    int n;
    cin >> n;
    vector<info> d(n);
    for (int i = 0; i < n; i++) cin >> d[i].b;
    for (int i = 0; i < n; i++) cin >> d[i].a;
    sort(d.begin(), d.end(), cmp);
    long long sum = 0;
    long long ans = 0x3f3f3f3f3f3f3f3f;
    for (int i = 0; i < n; i++) {
        ans = min(ans, max(sum,1ll *  d[i].b));
        sum += d[i].a;
    }
    ans = min(ans, sum);
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
