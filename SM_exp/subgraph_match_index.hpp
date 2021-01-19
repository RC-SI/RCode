#ifndef _ORG_SUBGRAPH_MATCH_INDEX_H
#define _ORG_SUBGRAPH_MATCH_INDEX_H

#include <array>
#include "util.hpp"

typedef std::array<int, 3> Edgeh;
typedef std::vector<Edgeh> EdgeVectorh;

class OrgSubGraphMatchIndex
{
public:
    int v_num, l_num;
    long long e_num;

    OrgSubGraphMatchIndex();
    ~OrgSubGraphMatchIndex();

    void build(const EdgeVectorh& _e_v, const std::vector<int> _v_l);
    std::vector<std::vector<int>> subgraph_matching(const LabelSubgraph& q);

private:
    EdgeVector edge_vec;
    std::vector<UVertex> graph, labels;
    std::vector<UVertex> graphh;
    std::vector<int> vertex2label;
    int *pool_edges = NULL, *pool_vertices = NULL, *pool_entries = NULL, *pool_ids = NULL;
    int *temp_join_res[2] = {NULL, NULL}, temp_join_size[2];
    int *temp_join_entries[2] = {NULL, NULL}, temp_join_entries_size[2];
    int *temp_join_ids[2] = {NULL, NULL};

    void gen_join_order(int u, std::vector<std::vector<int>>& q_edge_list,
            std::vector<int>& visited, std::vector<int>& join_order);

};



#endif
