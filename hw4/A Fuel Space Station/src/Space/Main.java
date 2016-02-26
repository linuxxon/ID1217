package Space;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 *
 * @author linuxxon
 */
public class Main {
    public static void main(String args[]) {
        /* Defaults */
        int numVehicles = 3;
        int numTankers = 2;
        
        if (args.length > 0)
            numVehicles = Integer.parseInt(args[0]);
        if (args.length > 1)
            numTankers = Integer.parseInt(args[1]);
        
        /* Init space station with three slots and 30 fuel capacity */
        SpaceStation myStation = new SpaceStation(3, 30, 30);
        
        SpaceVehicle vehicles[] = new SpaceVehicle[numVehicles];
        SpaceTanker tankers[] = new SpaceTanker[numTankers];
        
        ExecutorService pool = Executors.newFixedThreadPool(numVehicles);
        
        /* Spawn threads and create vehicles */
        for (int i=0; i < numVehicles; i++) {
            pool.submit(new SpaceVehicle(i, myStation));
        }
        for (int i=numVehicles; i < numTankers+numVehicles; i++) {
            tankers[i-numVehicles] = new SpaceTanker(i, myStation);
            tankers[i-numVehicles].start();
        }
        
        /* Wait for vehicles to finish */
        pool.shutdown();
        while (!pool.isTerminated())
            Thread.yield();
        
        System.exit(0);
    }
}
