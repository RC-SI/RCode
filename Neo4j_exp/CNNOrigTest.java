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
public class CNNOrigTest {
    private static final String directory = "/home/zhengweiguo/yfyang/hash_merge/databases/";
    private static final String rdirectory = "/home/zhengweiguo/yfyang/hash_merge/final_neo4j/";
    private static final String pdirectory = "/home/zhengweiguo/yfyang/hash_merge/pair_data/";
    private static final int maxi = Integer.MAX_VALUE;
    private String name;
    private String subname;
    private int N;
    private int SN;
    private int[] nodes1;
    private int[] nodes2;

    // tag::vars[]

    GraphDatabaseAPI graphDb;
    private DatabaseManagementService managementService;
    // end::vars[]

    void setUpDb(String name, String indexs, Boolean NB, int SN) throws IOException {
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
        this.nodes1 = new int[SN];
        this.nodes2 = new int[SN];
        this.managementService = new DatabaseManagementServiceBuilder( databaseDirectory ).build();
        this.graphDb = (GraphDatabaseAPI) managementService.database(DEFAULT_DATABASE_NAME);
        registerShutdownHook( managementService );
        try ( Transaction tx = graphDb.beginTx() ) {
            this.N = (int) Iterables.count(tx.getAllNodes());
        }
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
        try (BufferedReader br = new BufferedReader(new FileReader(pairfile))) {
            String line;
            int pindex = 0;
            while ((line = br.readLine()) != null) {
                String[] pairs = line.split(" ");
                int id1 = Integer.parseInt(pairs[0]);
                int id2 = Integer.parseInt(pairs[1]);
                nodes1[pindex] = ids[id1];
                nodes2[pindex++] = ids[id2];
            }
        }
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

    CNNOrigTest(String name, String index, boolean HD, int SN) throws IOException {
        this.setUpDb(name, index, HD, SN);
    }
    
    public static void main( final String[] args ) throws IOException {
        CNNOrigTest hello = new CNNOrigTest(args[0], args[1], Boolean.parseBoolean(args[2]), Integer.parseInt(args[3]));
        long startTime = System.nanoTime();
        long t1 = hello.CNCounting();
        long endTime   = System.nanoTime();
        long totalTime = (endTime - startTime)/1000000;
        System.out.println("Total CNs: " + t1 + ", time: " + totalTime + " ms");
        hello.shutdown();
    }

    long CNCounting() {
        long result = 0;
        for (int i=0;i!=this.SN; i++) {
            try ( Transaction tx = this.graphDb.beginTx() ) {
                Node node1 = tx.findNode(Label.label("N"), "UID", this.nodes1[i]);
                Node node2 = tx.findNode(Label.label("N"), "UID", this.nodes2[i]);
                Set<Long> adjs = new HashSet<>();
                for (Relationship rel : node1.getRelationships(Direction.OUTGOING))
                    adjs.add(rel.getEndNodeId());
                for (Relationship rel : node2.getRelationships(Direction.OUTGOING))
                    result += adjs.contains(rel.getEndNodeId())?1:0;
            }
        }
        return result;
    }
}
