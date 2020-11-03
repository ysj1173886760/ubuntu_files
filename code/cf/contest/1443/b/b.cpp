/*  Author: ysj1173886760
 *  Date: 2020-11-02-23-23
 *  Language: C++
 *  Enjoy Coding!
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<string>
#include<cmath>
using namespace std;
void solve() {
    int a, b;
    cin >> a >> b;
    string s;
    cin >> s;
    int ans = 0;
    bool flag = false;
    int sum = 0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '0') {
            sum++;
        } else {
            if (i != 0 && s[i] == s[i - 1]) continue;
            if (flag) {
                ans += min(a, sum * b);
                sum = 0;
            } else {
                sum = 0;
                ans += a;
                flag = true;
            }
        }
    }
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
