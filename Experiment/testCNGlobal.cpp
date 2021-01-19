#include "indexGraph.h"

typedef std::vector<std::string> stringvec;

int main(int argc,char** argv) {
    string directname = "../data/";
    string outdirect = "../hash_data/";
    string pairdirect = "../pair_data/";
    string name = argv[1];
    int N = stoi(argv[2]);
    string PN = argv[3];
    vec* G = new vec[N];
    cout << directname+name+".txt" << endl;
    utils::readGraph(directname+name+".txt", G, N);
    Graph OG(G, N);
    cout << "Build Original Graph Done" << endl;
    string hdirect = outdirect + name + "/" + name + "_range/" + name + "_range_";
    string hnode = hdirect + "node.csv";
    string hid = hdirect + "id.csv";
    cout << hnode << endl;
    cout << hid << endl;
    hashGraph HG(G, N, hnode, hid);
    cout << "Build Hash Graph Done" << endl;
    string idirect = outdirect + name + "/" + name + "_index/" + name + "_index_";
    string inode = idirect + "node.csv";
    string iid = idirect + "id.csv";
    cout << inode << endl;
    cout << iid << endl;
    indexGraph IG(G, N, inode, iid);
    cout << "Build Index Graph Done" << endl;
    cout << "--------------------------" << endl;
    pairdirect += name + "/" + PN;
    stringvec pairFiles;
    utils::read_directory(pairdirect, pairFiles);
    double OGF = 0.0;
    double HGF = 0.0;
    double IGF = 0.0;
    double HGR = 0.0;
    double IGR = 0.0;
    for (const string& f:pairFiles) {
        cout << "Start Processing " << f << endl;
        pvec pairs;
        utils::readPair(f, pairs);
        OGF += OG.calF(pairs, false);
        HGF += HG.calF(pairs, false);
        IGF += IG.calF(pairs, false);
        double OGT = OG.calCN(pairs, false);
        HGR += OGT/HG.calCN(pairs, false);
        IGR += OGT/IG.calCN(pairs, false);
        cout << "Finish Processing " << f << endl;
    }
    int fnum = pairFiles.size();
    cout << "Average Orig F: " << OGF/fnum << endl;
    cout << "Average Hash F: " << HGF/fnum << endl;
    cout << "Average Index F: " << IGF/fnum << endl;
    cout << "Average Hash Rate: " << HGR/fnum << endl;
    cout << "Average Index Rate: " << IGR/fnum << endl;
    delete[] G;
}