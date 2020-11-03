/*  Author: ysj1173886760
 *  Date: 2020-11-02-22-37
 *  Language: C++
 *  Enjoy Coding!
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<cmath>
using namespace std;

int gcd(int a,int b) {
    return b == 0 ? a : gcd(b, a % b);
}
void solve() {
    int n;
    cin >> n;
    vector<bool> check(4 * n + 1);
    vector<int>ans;
    int now = 2 * n - 1;
    int cnt = 0;
    while (now > 1) {
        if (!check[now]) {
            cout << now * 2 << " ";
            cnt++;
            ans.push_back(now * 2);
            for (int j = 2; j <= now; j++) if (now % j == 0) check[j] = true;
            for (int j = 1; j * now <= 2 * n; j++) check[j * now] = true;
        }
        now -= 2;
        if (cnt == n) break;
    }
    if (cnt != n) {
        now = 2 * n;
        while (now > 1) {
            if (!check[now]) {
                cout << now * 2 << " ";
                cnt++;
                ans.push_back(now * 2);
                for (int j = 2; j * j <= now; j++) if (now % j == 0) check[j] = true;
            }
            now -= 2;
            if (cnt == n) break;
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            int a = ans[i];
            int b = ans[j];
            if (a % b == 0 || b % a == 0 || gcd(a, b) == 1) cout << a << " " << b << endl;
        }
    }
    cout << endl;
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
