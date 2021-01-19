import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import java.util.stream.Collectors;

import javax.management.relation.Relation;

import org.neo4j.dbms.api.DatabaseManagementService;
import org.neo4j.dbms.api.DatabaseManagementServiceBuilder;
import org.neo4j.graphdb.*;
import org.neo4j.kernel.internal.GraphDatabaseAPI;
import org.neo4j.internal.helpers.collection.Iterables;
import scala.Int;

import static org.neo4j.configuration.GraphDatabaseSettings.DEFAULT_DATABASE_NAME;

/**
 * Unit test for simple App.
 */
public class CNNTest {
    private static final String directory = "/home/zhengweiguo/yfyang/hash_merge/databases/";
    private static final String rdirectory = "/home/zhengweiguo/yfyang/hash_merge/final_neo4j/";
    private static final String pdirectory = "/home/zhengweiguo/yfyang/hash_merge/pair_data/";
    private static final int maxi = Integer.MAX_VALUE;
    private String name;
    private String subname;
    private int N;
    private int[] ms1;
    private int[] Ms1;
    private int[] ms2;
    private int[] Ms2;
    private int m;
    private int SN;
    private int[] nodes1;
    private int[] nodes2;
    private List<Set<RelationshipType> > nrts1;
    private List<Set<RelationshipType> > nrts2;

    // tag::vars[]

    GraphDatabaseAPI graphDb;
    private DatabaseManagementService managementService;
    // end::vars[]

    void setUpDb(String name, String indexs, boolean NB, int SN) throws IOException {
        File databaseDirectory;
        this.name = name;
        this.subname = name + "_" + indexs;
        if (NB)
            this.subname += "_nb";
        String final_file = directory + this.name + "/" + this.subname;
        if (NB)
            final_file += "_f";
        databaseDirectory = new File(final_file);
        this.SN = SN;
        this.ms1 = new int[SN];
        this.Ms1 = new int[SN];
        this.ms2 = new int[SN];
        this.Ms2 = new int[SN];
        this.managementService = new DatabaseManagementServiceBuilder( databaseDirectory ).build();
        this.graphDb = (GraphDatabaseAPI) managementService.database(DEFAULT_DATABASE_NAME);
        registerShutdownHook( managementService );
        try ( Transaction tx = graphDb.beginTx() ) {
            this.N = (int) Iterables.count(tx.getAllNodes());
        }
        this.nodes1 = new int[SN];
        this.nodes2 = new int[SN];
        int[] ids = new int[N];
        try (BufferedReader br = new BufferedReader(new FileReader(rdirectory + this.name + "/" + this.subname + "_id.csv"))) {
            int index = 0;
            String line = br.readLine();
            while ((line = br.readLine()) != null) {
                int id = Integer.parseInt(line);
                ids[index++] = id;
            }
        }
        String pairfile = pdirectory + this.name + "/" + String.valueOf(SN) + "/";
        if (NB)
            pairfile += "NB/0_nb.txt";
        else
            pairfile += "0.txt";
        int pindex = 0;
        try (BufferedReader br = new BufferedReader(new FileReader(pairfile))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] pairs = line.split(" ");
                int id1 = Integer.parseInt(pairs[0]);
                int id2 = Integer.parseInt(pairs[1]);
                nodes1[pindex] = ids[id1];
                nodes2[pindex++] = ids[id2];
            }
        }
        this.nrts1 = new ArrayList<>();
        this.nrts2 = new ArrayList<>();
        for (int i=0; i!=this.SN; i++) {
            Set<RelationshipType> rtset1 = new HashSet<>();
            Set<RelationshipType> rtset2 = new HashSet<>();
            try ( Transaction tx = graphDb.beginTx() ) {
                Node node1 = tx.findNode(Label.label("N"), "UID", this.nodes1[i]);
                for (RelationshipType rt : node1.getRelationshipTypes())
                    rtset1.add(rt);
                this.nrts1.add(rtset1);
                Node node2 = tx.findNode(Label.label("N"), "UID", this.nodes2[i]);
                for (RelationshipType rt : node2.getRelationshipTypes())
                    rtset2.add(rt);
                this.nrts2.add(rtset2);
            }
        }
        int[] ml = new int[N];
        int[] Ml = new int[N];
        if (NB)
            this.subname += "_f";
        try (BufferedReader br = new BufferedReader(new FileReader(rdirectory + this.name + "/" + this.subname + "_node.csv"))) {
            String line = br.readLine();
            int index = 0;
            while ((line = br.readLine()) != null) {
                String[] hs = line.split(",");
                int cnm = Integer.parseInt(hs[0]);
                int cnM = Integer.parseInt(hs[1]);    
                ml[index] = cnm;
                Ml[index++] = cnM;
            }
        }
        m = Integer.MAX_VALUE;
        int M = Integer.MIN_VALUE;
        for (int i=0; i!=this.SN; i++) {
            this.ms1[i] = ml[nodes1[i]];
            this.ms2[i] = ml[nodes2[i]];
            this.Ms1[i] = Ml[nodes1[i]];
            this.Ms2[i] = Ml[nodes2[i]];
            int ms = Math.min(ms1[i], ms2[i]);
            int Ms = Math.max(Ms1[i], Ms2[i]);
            if (ms!=N && ms!=maxi) {
                if (ms<m)
                    m = ms;
                if (Ms>M)
                    M = Ms;
            }
        }
        System.out.println("Read mM Done");
    }

    // tag::shutdownHook[]
    private static void registerShutdownHook( final DatabaseManagementService managementService )
    {
        // Registers a shutdown hook for the Neo4j instance so that it
        // shuts down nicely when the VM exits (even if you "Ctrl-C" the
        // running application).
        Runtime.getRuntime().addShutdownHook( new Thread()
        {
            @Override
            public void run()
            {
                managementService.shutdown();
            }
        } );
    }
    // end::shutdownHook[]

    void shutdown() {
        this.managementService.shutdown();
    }

    CNNTest(String name, String index, boolean HD, int SN) throws IOException {
        this.setUpDb(name, index, HD, SN);
    }
    
    public static void main( final String[] args ) throws IOException {
        CNNTest hello2 = new CNNTest(args[0], args[1], Boolean.parseBoolean(args[2]), Integer.parseInt(args[3]));
        // hello2.CNHashCountingDebug();
        long startTime2 = System.nanoTime();
        long t2 = hello2.CNIndexCounting();
        long endTime2   = System.nanoTime();
        long totalTime2 = (endTime2 - startTime2)/1000000;
        System.out.println("Total CNs: " + t2 + ", time: " + totalTime2 + " ms");
        hello2.shutdown();
    }

    long CNIndexCounting() {
        long result = 0;
        for (int i=0;i!=this.SN; i++) {
            int id1 = this.nodes1[i];
            int id2 = this.nodes2[i];
            int m1 = this.ms1[i];
            int M1 = this.Ms1[i];
            int m2 = this.ms2[i];
            int M2 = this.Ms2[i];
            if (m1!=maxi && m2!=maxi) {
                int cnm = Math.max(m1,m2);
                int cnM = Math.min(M1,M2);
                if (cnm <= cnM) {
                    try ( Transaction tx = this.graphDb.beginTx() ) {
                        Node node1 = tx.findNode(Label.label("N"), "UID", id1);
                        Node node2 = tx.findNode(Label.label("N"), "UID", id2);
                        List<RelationshipType> rtset = new ArrayList<>();
                        if (this.nrts1.get(i).size() > this.nrts2.get(i).size()) {
                            for (RelationshipType rt : this.nrts2.get(i)) {
                                if (this.nrts1.get(i).contains(rt))
                                    rtset.add(rt);
                            }
                        }
                        else {
                            for (RelationshipType rt : this.nrts1.get(i)) {
                                if (this.nrts2.get(i).contains(rt))
                                    rtset.add(rt);
                            }
                        }
                        RelationshipType[] currRTS = rtset.toArray(new RelationshipType[rtset.size()]);
                        Set<Long> adjs = new HashSet<>();
                        for (Relationship rel : node1.getRelationships(Direction.OUTGOING, currRTS))
                            adjs.add(rel.getEndNodeId());
                        for (Relationship rel : node2.getRelationships(Direction.OUTGOING, currRTS))
                            result += adjs.contains(rel.getEndNodeId())?1:0;
                    }    
                }
            }
        }
        return result;
    }
}
