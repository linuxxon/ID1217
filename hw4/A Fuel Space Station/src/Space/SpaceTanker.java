/*
 * Class defining behaviour of a space tanker
 * 
 * Author: Rasmus Linusson
 * 
 */
package Space;

import java.util.concurrent.ThreadLocalRandom;
/**
 *
 * @author linuxxon
 */
public class SpaceTanker extends Thread {
    private int nitrogenLevel = 10;
    private int quantumLevel = 10;
    private int neededNitrogen = 0;
    private int neededQuantum = 0;
    private int name;
    
    public SpaceStation station;
    
    public SpaceTanker(int name, SpaceStation myStation) {
        this.name = name;
        station = myStation;
    }
    
    @Override
    public void run() {
        /* Make 10 transports between somehere */
        ThreadLocalRandom rand = ThreadLocalRandom.current();
        
        for (int i=0; i < 10; i++) {
            boolean entered = false;
            
            /* Travel in space */
            try {
                Thread.sleep(rand.nextInt(5,16));
            } catch (InterruptedException ex) {}
            
            /* Calculate new fuel needs */
            neededNitrogen = rand.nextInt(0,6);
            neededQuantum = rand.nextInt(1,6);
            
            /* Calculate carried fuel levels */
            nitrogenLevel = 15;
            quantumLevel = 15;
            
            /* Enter station.
            /* Tanker first enters to deposit, then enters again to fuel
             * When depositing a request for the negative amount of fuel
             * being delivered is sent as to allow simultanious deposit
             * and fueling by different vehicles.
             * 
             * This contrast to having two functions for entering the space
             * fueling bay or w/e
            */
            System.out.printf("Tanker %d requesting deposit of %d nitro %d quantum\n", name, nitrogenLevel, quantumLevel);
            station.enterWrapper(name, -nitrogenLevel, -quantumLevel);
            
            System.out.printf("Tanker %d begin depositing fuel\n", name);
            /* Deposit all fuel units */
            for ( ; nitrogenLevel > 0; nitrogenLevel--)
                station.depositNitrogen();
            for ( ; quantumLevel > 0; quantumLevel--)
                station.depositQuantum();
            
            /* See if there is enough fuel available to refuel the ship
             * otherwise, do like other ships and wait
            */
            System.out.printf("Tanker %d deposited all fuel, see if enough is available to refuel\n", name);
            if (!station.reserve(neededNitrogen, neededQuantum)) {
                System.out.printf("Tanker %d, not enough available", name);
                station.leave(name);
                station.enterWrapper(name, neededNitrogen, neededQuantum);
            }
            
            /* Refuel the ship */
            for ( ; neededNitrogen > 0; neededNitrogen--)
                station.getNitrogen();
            for ( ; neededQuantum > 0; neededQuantum--)
                station.getQuantum();
            
            /* Leave this place */
            station.leave(name);
        }
        System.out.printf("Tanker %d done\n");
    }
    
}
