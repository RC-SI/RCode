#include "indexGraph.h"

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
    string hdirect = outdirect + name + "/" + name + "_range_nb/" + name + "_range_nb_";
    string hnode = hdirect + "node.csv";
    string hid = hdirect + "id.csv";
    cout << hnode << endl;
    cout << hid << endl;
    hashGraph HG(G, N, hnode, hid);
    cout << "Build Hash Graph Done" << endl;
    string idirect = outdirect + name + "/" + name + "_index_nb/" + name + "_index_nb_";
    string inode = idirect + "node.csv";
    string iid = idirect + "id.csv";
    cout << inode << endl;
    cout << iid << endl;
    indexGraph IG(G, N, inode, iid);
    cout << "Build Index Graph Done" << endl;
    cout << "--------------------------" << endl;
    pairdirect += name + "/" + PN + "/NB/";
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
        OGF += OG.calF(pairs, true);
        HGF += HG.calF(pairs, true);
        IGF += IG.calF(pairs, true);
        double OGT = OG.calCN(pairs, true);
        HGR += OGT/HG.calCN(pairs, true);
        IGR += OGT/IG.calCN(pairs, true);
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