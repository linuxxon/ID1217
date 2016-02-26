package Space;

import java.util.concurrent.ThreadLocalRandom;

/**
 *
 * @author linuxxon
 */

public class SpaceVehicle implements Runnable {
    private int neededNitrogen = 0;
    private int neededQuantum = 0;
    private int name;
    
    public SpaceStation station;
    
    public SpaceVehicle(int name, SpaceStation myStation) {
        this.name = name;
        station = myStation;
    }
    
    @Override
    public void run() {
        /* Make 10 transports between somehere */
        ThreadLocalRandom rand = ThreadLocalRandom.current();
        
        for (int i=0; i < 10; i++) {
            /* Travel in space */
            try {
                Thread.sleep(rand.nextInt(5,16));
            } catch (InterruptedException ex) {}
            
            /* Calculate new fuel needs */
            neededNitrogen = rand.nextInt(0,6);
            neededQuantum = rand.nextInt(1,6);
            
            /* Enter station.
            * Only get in when my quantity is available */
            System.out.printf("Vehicle %d need %d nitro and %d quantum\n", name, neededNitrogen, neededQuantum);
            station.enterWrapper(name, neededNitrogen, neededQuantum);
            
            System.out.printf("Vehicle %d is now refueling\n", name);
            
            for ( ; neededNitrogen > 0; neededNitrogen--)
                station.getNitrogen();
            for ( ; neededQuantum > 0; neededQuantum--)
                station.getQuantum();
            
            station.leave(name);
        }
        System.out.printf("Vehicle %d is gone\n", name);
    }
    
}
