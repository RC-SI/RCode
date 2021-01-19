import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

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
public class TriTest {
    private static final String directory = "/home/zhengweiguo/yfyang/hash_merge/databases/";
    private static final String rdirectory = "/home/zhengweiguo/yfyang/hash_merge/final_neo4j/";
    private static final String confFile = "./conf";
    private static final int maxi = Integer.MAX_VALUE;
    private String name;
    private String subname;
    private int N;
    private int[] ms;
    private int[] Ms;
    private RelationshipType[] rts;
    private int m;
    private long[] ids;
    private Map<Long, Integer> idmap;

    // tag::vars[]

    GraphDatabaseAPI graphDb;
    private DatabaseManagementService managementService;
    // end::vars[]

    void setUpDb(String name, String index) throws IOException {
        File databaseDirectory;
        this.name = name;
        this.subname = name + "_" + index + "_nb";
        String final_file = directory + this.name + "/" + this.subname;
        databaseDirectory = new File(final_file);
        this.managementService = new DatabaseManagementServiceBuilder( databaseDirectory ).build();
        this.graphDb = (GraphDatabaseAPI) managementService.database(DEFAULT_DATABASE_NAME);
        registerShutdownHook( managementService );
        try ( Transaction tx = graphDb.beginTx() ) {
            this.N = (int) Iterables.count(tx.getAllNodes());
        }
    }

    void setUpIndex() throws IOException {
        this.ms = new int[N];
        this.Ms = new int[N];
        this.ids = new long[N];
        this.idmap = new HashMap<>();
        try ( Transaction tx = graphDb.beginTx() ) {
            for (int i=0; i<N; i++) {
                Node node = tx.findNode(Label.label("N"), "UID", i);
                ids[i] = node.getId();
                idmap.put(ids[i], i);
            }
        }
        m = Integer.MAX_VALUE;
        int M = Integer.MIN_VALUE;
        try (BufferedReader br = new BufferedReader(new FileReader(rdirectory + this.name + "/" + this.subname + "_node.csv"))) {
            String line = br.readLine();
            int index = 0;
            while ((line = br.readLine()) != null) {
                String[] hs = line.split(",");
                int cm = Integer.parseInt(hs[0]);
                int cM = Integer.parseInt(hs[1]);
                if (cm!=N && cm!=maxi) {
                    if (cm<m)
                        m = cm;
                    if (cM>M)
                        M = cM;
                }
                this.ms[index] = cm;
                this.Ms[index++] = cM;
            }
        }
        rts = new RelationshipType[M-m+1];
        for (int i=0; i<=M-m; i++)
            rts[i] = RelationshipType.withName(String.valueOf(m+i));
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

    TriTest(String name, String index) throws IOException {
        this.setUpDb(name, index);
        this.setUpIndex();
    }
    
    public static void main( final String[] args ) throws IOException {
        TriTest hello2 = new TriTest(args[0], args[1]);
        long startTime2 = System.nanoTime();
        long t2 = hello2.TriIndexCounting();
        long endTime2   = System.nanoTime();
        long totalTime2 = (endTime2 - startTime2)/1000000;
        System.out.println("Total CNs: " + t2 + ", time: " + totalTime2 + " ms");
        hello2.shutdown();
    }

    long TriHashCounting() {
        long result = 0;
        try ( Transaction tx = this.graphDb.beginTx() ) {
            for (int i=0; i<this.N; i++) {
                int m1 = this.ms[i];
                int M1 = this.Ms[i];
                if (m1<=M1) {
                    RelationshipType[] rts1 = Arrays.copyOfRange(rts, m1-m, M1-m+1);
                    Set<Long> adjs = new HashSet<>();
                    Node node1 = tx.getNodeById(this.ids[i]);
                    for (Relationship rel : node1.getRelationships(Direction.OUTGOING, rts1))
                        adjs.add(rel.getEndNodeId());
                    for (long adj : adjs) {
                        int id2 = this.idmap.get(adj);
                        int m2 = this.ms[id2];
                        int M2 = this.Ms[id2];
                        int cnm = Math.max(m1,m2);
                        int cnM = Math.min(M1,M2);
                        if (cnm <= cnM) {
                            Node node2 = tx.getNodeById(adj);
                            RelationshipType[] currRTS = Arrays.copyOfRange(rts, cnm-m, cnM-m+1);
                            for (Relationship rel : node2.getRelationships(Direction.OUTGOING, currRTS))
                                result += adjs.contains(rel.getEndNodeId())?1:0;
                        }
                    }
                }
            }
        }
        return result;
    }

    long TriIndexCounting() {
        long result = 0;
        try ( Transaction tx = this.graphDb.beginTx() ) {
            for (int i=0; i<this.N; i++) {
                int m1 = this.ms[i];
                int M1 = this.Ms[i];
                if (m1<=M1) {
                    RelationshipType[] rts1 = Arrays.copyOfRange(rts, m1-m, M1-m+1);
                    List<RelationshipType> rtset = new ArrayList<>();
                    Set<Long> adjs = new HashSet<>();
                    Node node1 = tx.getNodeById(this.ids[i]);
                    for (Relationship rel : node1.getRelationships(Direction.OUTGOING, rts1)) {
                        adjs.add(rel.getEndNodeId());
                        rtset.add(rel.getType());
                    }
                    RelationshipType[] currRTS = rtset.toArray(new RelationshipType[rtset.size()]);
                    for (long adj : adjs) {
                        int id2 = this.idmap.get(adj);
                        int m2 = this.ms[id2];
                        int M2 = this.Ms[id2];
                        int cnm = Math.max(m1,m2);
                        int cnM = Math.min(M1,M2);
                        if (cnm <= cnM) {
                            Node node2 = tx.getNodeById(adj);
                            for (Relationship rel : node2.getRelationships(Direction.OUTGOING, currRTS))
                                result += adjs.contains(rel.getEndNodeId())?1:0;
                        }
                    }
                }
            }
        }
        return result;
    }

}
