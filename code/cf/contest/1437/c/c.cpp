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

void solve() {
    int n;
    cin >> n;
    vector<int> a(n);
    vector<bool> check(3 * n + 1, true);
    for (int i = 0; i < n; i++) cin >> a[i];
    sort(a.begin(), a.end());
    long long ans = 0;
    for (int i = n - 1; i >= 0; i--) {
        int x = a[i];
        int l = x;
        int r = x;
        if (check[x]) {
            check[x] = false;
        } else {
            while(1) {
                if (r < 3 * n) {
                    r++;
                    if(check[r]) {
                        check[r] = false;
                        break;
                    }
                }
                if (l > 1) {
                    l--;
                    if (check[l]) {
                        check[l] = false;
                        break;
                    } 
                }
            }
        }
//        cout << x << " " << ans << endl;
    }
    int now = 3 * n;
    for (int i = n - 1; i >= 0; i--) {
        while (check[now]) now--;
        ans += abs(a[i] - now);
        now--;
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

