#include "indexMCGraph.h"
#include "hashGraph.h"

typedef unordered_set<int> intset;

void getDegOrder(vec*& G, int*& Vrank, int N) {
    int* order = new int[N];
    tqdm bar;
    int* degrees = new int[N];
    map<int, intset> D;
    int d;
    for (int i=0; i<N; i++) {
        d = G[i].size();
        degrees[i] = d;
        D[d].insert(i);
    }
    boost::dynamic_bitset<> mark(N);
    int marked = 0;
    int n;
    while (marked<N) {
        for (map<int, intset>::iterator it=D.begin(); it!=D.end(); it++) {
            if (!it->second.empty()) {
                n = *(it->second.begin());
                break;
            }
        }
        mark.set(n);
        d = degrees[n];
        D[d].erase(n);
        for (const auto& adj : G[n]) {
            if (!mark.test(n)) {
                d = degrees[adj];
                D[d].erase(adj);
                D[d-1].insert(adj);
                degrees[adj] = d-1;
            }
        }
        Vrank[n] = marked;
        order[marked++] = n;
        bar.progress(marked, N);
    }
}

int main(int argc,char** argv) {
    string directname = "../data/";
    string outdirect = "../hash_data/";
    string groDirect = "../GRO_data/";
    string name = argv[1];
    int N = stoi(argv[2]);
    int PN = stoi(argv[3]);
    vec nodes(PN);
    for (int i=0; i<PN; i++)
        nodes[i] = (N/PN)*i;
    vec* G = new vec[N];
    utils::readGraph(directname+name+".txt", G, N);
    int* Vrank = new int[N];
    getDegOrder(G, Vrank, N);
    Graph OG(G, N);
    double OGT = OG.MC(nodes, Vrank);
    {
        int* hashes = new int[N];
        iota(hashes, hashes+N, 0);
        hashGraph HG(G, hashes, N);
        double HGT = HG.MC(nodes, Vrank);
        cout << "Orig Order Rate: " << OGT/HGT << endl;
        delete[] hashes;
    }
    groDirect += name;
    stringvec groFiles;
    utils::read_directory(groDirect, groFiles);
    for (const string& f:groFiles) {
        hashGraph HG(G, N, f);
        int* VrankN = new int[N];
        for (int i=0; i!=N; i++)
            VrankN[HG.getId(i)] = Vrank[i];
        vec newNodes(nodes.size());
        for (int i=0; i!=nodes.size(); i++)
            newNodes[i] = HG.getId(nodes[i]);
        double GROT = HG.MC(newNodes, VrankN);
        cout << f << " Rate: " << OGT/GROT << endl;
        delete[] VrankN;
    }
    string hdirect = outdirect + name + "/" + name + "_range_nb/" + name + "_range_nb_";
    string hnode = hdirect + "node.csv";
    string hid = hdirect + "id.csv";
    cout << hnode << endl;
    cout << hid << endl;
    {
        hashGraph HG(G, N, hnode, hid);
        int* VrankN = new int[N];
        for (int i=0; i!=N; i++)
            VrankN[HG.getId(i)] = Vrank[i];
        vec newNodes(nodes.size());
        for (int i=0; i!=nodes.size(); i++)
            newNodes[i] = HG.getId(nodes[i]);
        double HGT = HG.MC(newNodes, VrankN);
        cout << "HG Rate: " << OGT/HGT << endl;
        delete[] VrankN;
    }
    string idirect = outdirect + name + "/" + name + "_index_nb/" + name + "_index_nb_";
    string inode = idirect + "node.csv";
    string iid = idirect + "id.csv";
    cout << inode << endl;
    cout << iid << endl;
    {
        indexVecGraph IG(G, N, inode, iid);
        cout << "Build Index Graph Done" << endl;
        int* VrankN = new int[N];
        for (int i=0; i!=N; i++)
            VrankN[IG.getId(i)] = Vrank[i];
        vec newNodes(nodes.size());
        for (int i=0; i!=nodes.size(); i++)
            newNodes[i] = IG.getId(nodes[i]);
        double IGT = IG.MC(newNodes, VrankN);
        cout << "IG Rate: " << OGT/IGT << endl;
        delete[] VrankN;
    }
    delete[] Vrank;
    delete[] G;
}