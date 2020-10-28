/*  Author: ysj1173886760
 *  Date: 2020-10-28-00-07
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
    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];
    int ans = 0;
    int now = 1;
    int lst = 1;
    while (now < n) {
        int sum = 0;
        for (int i = 1; i <= lst; i++) {
            now++;
            sum++;
            while (now < n && a[now] > a[now - 1]) {
                now++;
                sum++;
            }
        }
//        cout << now << " ";
        lst = sum;
        ans++;
    }
//    cout << endl;
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

