package Space;

/**
 *
 * @author linuxxon
 */

public class SpaceVehicle extends Thread {
    private int neededNitrogen = 0;
    private int neededQuantum = 0;
    
    public SpaceStation station;
    
    @Override
    public void run() {
        /* Make 10 transports between somehere */
        for (int i=0; i < 10; i++) {
            boolean entered = false;
            
            /* Calculate new fuel needs */
            neededNitrogen = 4;
            neededQuantum = 3;
            
            /* Enter station.
            * Only get in when my quantity is available */
            while (!entered) {
                try {
                    station.enter(neededNitrogen, neededQuantum);
                    entered = true;
                } catch (InterruptedException ex) { }
            }
            
            for ( ; neededNitrogen > 0; neededNitrogen--)
                station.getNitrogen();
            for ( ; neededQuantum > 0; neededQuantum--)
                station.getQuantum();
            
                
            station.leave();
        }
    }
    
}
