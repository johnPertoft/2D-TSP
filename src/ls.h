#include <vector>

#define MAX 1000

inline int compute_length(vector<int> path, const int ds[][MAX]) {
    int l = 0;
    for (int i = 1; i < path.size(); ++i) l += ds[path[i-1]][path[i]];
    l += ds[path[path.size()-1]][path[0]];
    return l;
}

// Check for copy stuff
void two_opt(vector<int>& tour, const int ds [][MAX], const vector<vector<int>>& nns) {

    int nn_ls = nns[0].size();

    int n = tour.size();
    int c1, c2;
    int suc_c1, suc_c2;
    int pred_c1, pred_c2;
    int pos_c1, pos_c2;
    int i, j, h, l;
    int help, n_exchanges = 0;
    int h1 = 0, h2 = 0, h3 = 0, h4 = 0;
    int radius;
    int gain = 0;
    int max_gain = 0;
    int tour_length;

    bool improvement_flag = true;

    int pos[n];

    tour_length = compute_length(tour, ds);

    for (i = 0; i < n; ++i) {
        pos[tour[i]] = i;
    }

    while (improvement_flag) {
        improvement_flag = false;
        max_gain = 0;

        for (l = 0; l < n; l++) {

            c1 = l;
            pos_c1 = pos[c1];
            suc_c1 = tour[pos_c1+1];
            radius = ds[c1][suc_c1];

            /* First search for c1's nearest neighbours, use successor of c1 */

            for (h = 0; h < nn_ls; h++) {

                c2 = nns[c1][h];
                if (radius > 0) {
                    suc_c2 = tour[pos[c2]+1];
                    gain = - radius + ds[c1][c2] +
                        ds[suc_c1][suc_c2] - ds[c2][suc_c2];
                    if (gain < max_gain) {
                        h1 = c1; h2 = suc_c1; h3 = c2; h4 = suc_c2;
                        improvement_flag = true;
                        max_gain = gain;
                    }
                }
                else break;
            }
            /* Search one for next c1's h-nearest neighbours, use predecessor c1 */
            if (pos_c1 > 0) pred_c1 = tour[pos_c1-1];
            else pred_c1 = tour[n-1];

            radius = ds[pred_c1][c1];

            for (h = 0; h < nn_ls; h++) {

                c2 = nns[c1][h];

                if (radius > 0) {
                    pos_c2 = pos[c2];

                    if (pos_c2 > 0) pred_c2 = tour[pos_c2-1];
                    else pred_c2 = tour[n-1];

                    if (pred_c2 == c1) continue;
                    if (pred_c1 == c2) continue;

                    gain =  - radius + ds[c1][c2] +
                        ds[pred_c1][pred_c2] - ds[pred_c2][c2];

                    if (gain < max_gain) {
                        h1 = pred_c1; h2 = c1; h3 = pred_c2; h4 = c2;
                        improvement_flag = true;
                        max_gain = gain;
                    }
                }
                else break;
            }
        }

        if (improvement_flag) {
            n_exchanges++;
            tour_length += max_gain;
            /* Now perform move */
            if (pos[h3] < pos[h1]) {
                help = h1; h1 = h3; h3 = help;
                help = h2; h2 = h4; h4 = help;
            }
            if (pos[h3] - pos[h2] < n / 2 + 1) {
                /* reverse inner part from pos[h2] to pos[h3] */
                i = pos[h2]; j = pos[h3];
                while (i < j) {
                    c1 = tour[i];
                    c2 = tour[j];
                    tour[i] = c2;
                    tour[j] = c1;
                    pos[c1] = j;
                    pos[c2] = i;
                    i++; j--;
                }
            }
            else {
                i = pos[h1]; j = pos[h4];
                if (j > i) help = n - (j - i) + 1;
                else help = (i - j) + 1;

                help = help / 2;

                for (h = 0; h < help; h++) {
                    c1 = tour[i];
                    c2 = tour[j];
                    tour[i] = c2;
                    tour[j] = c1;
                    pos[c1] = j;
                    pos[c2] = i;
                    i--; j++;
                    if (i < 0) i = n-1;
                    if (j >= n) j = 0;
                }

                tour[n] = tour[0];
            }
        }
    }
}
