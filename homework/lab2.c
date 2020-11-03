#include<stdio.h>
#define maxn  10000
int q1[100010], q2[10010];


int main() {
    int n, m, k;
    scanf("%d %d %d", &n, &m, &k);
    int h1 = 0, h2 = 0, t1 = 0, t2 = 0;
    for (int i = 1; i <= n; i++) {
        q1[++t1] = i;
    }
    for (int i = 1; i <= m; i++) {
        q2[++t2] = i;
    }
    for (int i = 1; i <= k; i++) {
        h1 = h1 % maxn + 1;
        h2 = h2 % maxn + 1;
        printf("male %d match female %d\n", q1[h1], q2[h2]);
        t1 = t1 % maxn + 1;
        t2 = t2 % maxn + 1;
        q1[t1] = q1[h1];
        q2[t2] = q2[h2];
    }
    return 0;
}
