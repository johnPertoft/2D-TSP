/**
 * Written by Johan Stenberg <jostenbe@kth.se> 30-01-2014.
 */

#ifndef API_DISJOINT_SET_H
#define API_DISJOINT_SET_H

#include <vector>
#include <unordered_map>
#include <utility>

namespace popup {

    /**
     * Struct for element metadata. Initialized
     * is only used if queried for elements not existing.
     */
    struct disjoint_set_node {
        int rank;
        int parent;
    };

    /**
     * Class containing the disjoint set.
     */
    class disjoint_set {
    private:
        static const int MAX = 100001;
        std::vector<disjoint_set_node> m_v;
    public:
        disjoint_set() : m_v() {
            m_v.resize(MAX);
        }

        /**
         * Inserts the element x in a new subset.
         *
         * Constant complexity.
         */
        void make_set(const int x) {
            disjoint_set_node node;
            node.rank = 0;
            node.parent = x;
            m_v[x] = node;
        }

        /**
         * Find a element in the disjoint set.
         *
         * Logarithmic complexity.
         */
        int find(const int x) {
            disjoint_set_node& node = m_v[x];
            if (node.parent == x) {
                return x;
            } else {
                int result = find(node.parent);
                node.parent = result;
                return result;
            }
        }

        /**
         * Unions 2 sets in the disjoint set.
         *
         * Logarithmic complexity.
         */
        void union_elements(const int x, const int y) {
            int x_root = find(x);
            int y_root = find(y);

            disjoint_set_node& x_root_node = m_v[x_root];
            disjoint_set_node& y_root_node = m_v[y_root];
            if (x_root != y_root) {
                if (x_root_node.rank < y_root_node.rank) {
                    x_root_node.parent = y_root;
                } else if (y_root_node.rank < x_root_node.rank) {
                    y_root_node.parent = x_root;
                } else {
                    y_root_node.parent = x_root;
                    ++x_root_node.rank;
                }
            }
        }

        void move(const int target, const int y) {

        }

        std::pair<int, int> get_sum_and_count(const int parent) {
            int sum = 0;
            int count = 0;

            for (int i = 0; i < MAX; ++i) {
                if (m_v[i].parent == parent) {
                    ++count;
                    count += i;
                }
            }

            return std::make_pair(sum, count);
        }

    };

}

#endif
