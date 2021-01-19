#include "util.hpp"
#include "subgraph_match.hpp"
#include "subgraph_match_hash.hpp"
#include "subgraph_match_index.hpp"

using namespace std;

struct timeval time_start;
struct timeval time_end;
typedef vector<string> stringvec;


vector<LabelSubgraph> load_queries(const std::string path)
{
    FILE *fp = fopen(path.c_str(), "r");
    if (fp == NULL) {
        std::cout << "fail to open " << path << std::endl;
        quit();
    }

    vector<LabelSubgraph> queries;
    int v_num, e_num;
    while (fscanf(fp, "%d%d", &v_num, &e_num) != EOF) {
        LabelSubgraph q(v_num, e_num);
        q.vertex2label.reserve(v_num);
        q.edge_vec.reserve(e_num);
        int l, u, v;
        for (int i = 0; i < v_num; ++i) {
            fscanf(fp, "%d", &l);
            q.vertex2label.push_back(l);
        }
        for (int i = 0; i < e_num; ++i) {
            fscanf(fp, "%d%d", &u, &v);
            if (u > v) swap(u, v);
            q.edge_vec.push_back(Edge(u, v));
        }
        queries.push_back(q);            
    }

    return queries;
}

vector<int> load_labels(const std::string path)
{
    FILE *fp = fopen(path.c_str(), "r");
    if (fp == NULL) {
        std::cout << "fail to open " << path << std::endl;
        quit();
    }

    vector<int> labels;
    int u, l;
    while (fscanf(fp, "%d%d", &u, &l) != EOF) labels.push_back(l);

    return labels;
}

vector<int> load_hashes(const std::string path)
{
    vector<int> hashes;
    std::ifstream hash_File(path.c_str());
    std::string currLine;
    std::getline(hash_File, currLine);
    while(std::getline(hash_File, currLine)) {
        std::stringstream linestream(currLine);
        std::string ids;
        std::getline(linestream,ids,',');
        std::string hs;
        std::getline(linestream,hs,',');
        hashes.push_back(stoi(hs));
    }
    return hashes;
}

EdgeVectorh load_graphh(const std::string path, vector<int>& hashes)
{
    EdgeVectorh edge_vec;
    FILE *fp = fopen(path.c_str(), "r");
    if (fp == NULL) {
        std::cout << "fail to open " << path << std::endl;
        quit();
    }

    char line[512];
    while (fgets(line, 512, fp) != NULL) {
        if (line[0] == '#') continue;
        int u = 0, v = 0;
        const char *c = line;
        while (isdigit(*c))
            u = (u << 1) + (u << 3) + (*c++ - 48);
        c++;
        while (isdigit(*c))
            v = (v << 1) + (v << 3) + (*c++ - 48);
        edge_vec.push_back(Edgeh{u, v, hashes[v]});
    }
    fclose(fp);
    
    return edge_vec;
}

void read_directory(const string& name, stringvec& v) {
    boost::filesystem::path p(name);
    boost::filesystem::directory_iterator start(p);
    boost::filesystem::directory_iterator end;
    while (start != end) {
        if (!boost::filesystem::is_directory((*start).path()))
            v.push_back((*start).path().string());
        start++;
    }
}

void readCSV(const string file, int*& hashes, bool withHeader=false, char delim=' ') {
    ifstream hash_File(file.c_str());
    string currLine;
    if (withHeader)
        getline(hash_File, currLine);
    while(getline(hash_File, currLine)) {
        stringstream linestream(currLine);
        string ids;
        getline(linestream,ids,delim);
        int idx = stoi(ids);
        string hs;
        getline(linestream,hs,delim);
        hashes[idx] = stoi(hs);
    }
}

void readId(const string file, int*& ids) {
    ifstream hash_File(file.c_str());
    string currLine;
    getline(hash_File, currLine);
    int i = 0;
    while(getline(hash_File, currLine)) {
        stringstream linestream(currLine);
        string id;
        getline(linestream,id,',');
        ids[i++] = stoi(id);
    }
}

template <typename T>
void sort_indexes(T*& v, int*& ids, int N) {
    int* idx = new int[N];
    iota(idx, idx+N, 0);
    stable_sort(idx, idx+N, [v](int i1, int i2) {return v[i1] < v[i2];});

    for (int i=0; i<N; i++)
        ids[idx[i]] = i;
    delete[] idx;
}

int main(int argc, char* argv[])
{
    string outdirect = "../hash_data/";
    string name = argv[1];
    int N = stoi(argv[2]);
    string groIDDirect = "../GRO_data/"+name+"/"+name;
    string sm_queries_file = argv[3];
    auto queries = load_queries(sm_queries_file);
    string graph_labels_file = "./data/"+name+".l";
    auto labels = load_labels(graph_labels_file);
    string graphDirect = "./data/" + name + "/";
    double OGT;
    {
        string graph_edges_file = graphDirect + name + ".g";
        auto edge_vec = load_graph(graph_edges_file);
        OrgSubGraphMatch sm;
        sm.build(edge_vec, labels);        
        gettimeofday(&time_start, NULL);
        for (const auto& q : queries)
            auto ans = sm.subgraph_matching(q);
        gettimeofday(&time_end, NULL);
        OGT = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_usec - time_start.tv_usec) / 1000.0;
    }
    {
        string graph_edges_file = graphDirect + name + ".g";
        auto edge_vec = load_graph(graph_edges_file);
        vector<int> hashes(N);
        iota(hashes.begin(), hashes.end(), 0);
        OrgSubGraphMatchHash sm;
        sm.build(edge_vec, labels, hashes);
        gettimeofday(&time_start, NULL);
        for (const auto& q : queries)
            auto ans = sm.subgraph_matching(q);
        gettimeofday(&time_end, NULL);
        double HGT = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_usec - time_start.tv_usec) / 1000.0;
        cout << "Orig Order Rate: " << OGT/HGT << endl;
    }
    string groDirect = graphDirect + "GRO";
    stringvec groFiles;
    read_directory(groDirect, groFiles);
    for (const string& f:groFiles) {
        string idname = groIDDirect + f.substr(f.find_last_of('_'), f.find_last_of('.')-f.find_last_of('_'))+"_newID.txt";
        cout << idname << endl;
        auto edge_vec = load_graph(f);
        vector<int> hashes(N);
        iota(hashes.begin(), hashes.end(), 0);
        OrgSubGraphMatchHash sm;
        sm.build(edge_vec, labels, hashes);        
        gettimeofday(&time_start, NULL);
        for (const auto& q : queries)
            auto ans = sm.subgraph_matching(q);
        gettimeofday(&time_end, NULL);
        double GROT = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_usec - time_start.tv_usec) / 1000.0;
        cout << f << " Rate: " << OGT/GROT << endl;
    }
    {
        string hnode = outdirect + name + "/" + name + "_range_nb/" + name + "_range_nb_node.csv";
        string hid = outdirect + name + "/" + name + "_range_nb/" + name + "_range_nb_id.csv";
        cout << hnode << endl;
        auto edge_vec = load_graph(graphDirect+name+"_range.g");
        auto hashes = load_hashes(hnode);
        OrgSubGraphMatchHash sm;
        sm.build(edge_vec, labels, hashes);        
        gettimeofday(&time_start, NULL);
        for (const auto& q : queries)
            auto ans = sm.subgraph_matching(q);
        gettimeofday(&time_end, NULL);
        double HGT = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_usec - time_start.tv_usec) / 1000.0;
        cout << "HG Rate: " << OGT/HGT << endl;
    }
    {
        string inode = outdirect + name + "/" + name + "_index_nb/" + name + "_index_nb_node.csv";
        string iid = outdirect + name + "/" + name + "_index_nb/" + name + "_index_nb_id.csv";
        cout << inode << endl;
        auto hashes = load_hashes(inode);
        auto edge_vec = load_graphh(graphDirect+name+"_index.g", hashes);
        OrgSubGraphMatchIndex sm;
        sm.build(edge_vec, labels);
        gettimeofday(&time_start, NULL);
        for (const auto& q : queries)
            auto ans = sm.subgraph_matching(q);
        gettimeofday(&time_end, NULL);
        double IGT = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_usec - time_start.tv_usec) / 1000.0;
        cout << "IG Rate: " << OGT/IGT << endl;
    }
}
