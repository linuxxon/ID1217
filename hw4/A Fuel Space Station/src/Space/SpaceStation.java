/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package Space;

import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.atomic.AtomicInteger;

/**
 *
 * @author linuxxon
 */
public class SpaceStation {
    /* *Left is the amount of units that are in the tanks
     * *Available is the amount of units that have not been promised
     * to a captian.
    */
    private AtomicInteger nitrogenLeft, quantumLeft;
    private int nitrogenAvailable, quantumAvailable;
    private int nitrogenCapacity, quantumCapacity;
    private AtomicInteger freeSpots;
    
    /* Objects to wait on and use for notification */
    private final Object noSpot = new Object();
    private final Object noFuel = new Object();
    private final Object tankerWait = new Object();
    
    public SpaceStation(int slots) {
        init(slots, 100, 100);
    } public SpaceStation(int slots, int capacity) {
        init(slots, capacity, capacity);
    } public SpaceStation(int slots, int nitrogenCapacity, int quantumCapacity) {
        init(slots, nitrogenCapacity, quantumCapacity);
    }
    
    private void init(int slots, int nitrogenCapacity, int quantumCapacity) {
        freeSpots = new AtomicInteger(slots);
        nitrogenLeft = new AtomicInteger(nitrogenCapacity);
        nitrogenAvailable = nitrogenCapacity;
        quantumLeft = new AtomicInteger(quantumCapacity);
        quantumAvailable = quantumCapacity;
        this.nitrogenCapacity = nitrogenCapacity;
        this.quantumCapacity = quantumCapacity;
        
        System.out.printf("Space fueling stations has %d slots and capacity of %d nitro %d quantum\n", slots, nitrogenCapacity, quantumCapacity);
    }
    
    /* Wrapper for looping enter station in case of interruption */
    public void enterWrapper(int name, int nit, int quant) {
        boolean entered = false;
        while (!entered) {
            entered = true;
            try {
                enter(name, nit, quant);
            } catch (InterruptedException ex) {
                entered = false;
            }
        }
    }
    
    public void enter(int name, int nit, int quant) throws InterruptedException {
        /* Make sure there is enough fuel */
        while (!reserve(nit, quant)) {
            System.out.printf("Vehicle or tanker %d couldn't reserve fuel, wait for tanker. Need %d nitro %d quantum, have %d, %d\n", name, nit, quant, nitrogenAvailable, quantumAvailable);
            if (nit < 0 || quant < 0) { /* Is tanker, wait for a vehicle to exit and try again */
                synchronized (tankerWait) {
                    tankerWait.wait();
                }
            }
            else {
                synchronized (noFuel) {
                    noFuel.wait();
                }
            }
        }
        
        System.out.printf("Vehicle or tanker %d reserved fuel, try slot\n", name);
        /* Drive upto a fueling slot */
        while (!tryFreeSlot()) {
            System.out.printf("Vehicle or tanker %d didn't get a fueling slot, wait\n", name);
            synchronized (noSpot) {
                noSpot.wait();
            }
        }
        
        /* Waiting by the fuel pump with enough fuel to top off */
    }
    
    /* Update counter of free spots and notify anyone in "queue" */
    public void leave(int name) {
        /* Concurrency make artefacts, make sure not to print them */
        int nitLeft = nitrogenLeft.get();
        int quantLeft = quantumLeft.get();
        nitLeft = nitLeft < 0 ? 0 : nitLeft;
        quantLeft = quantLeft < 0 ? 0 : quantLeft;
        
        System.out.printf("Vehicle or tanker %d leaving, nitroAvail %d quantumAvail %d, left %d   %d\n",
                name, nitrogenAvailable, quantumAvailable, nitLeft, quantLeft);
        freeSpots.incrementAndGet();
        synchronized (noSpot) {
            noSpot.notify();
        }
        synchronized (tankerWait) {
            tankerWait.notifyAll(); /* Wake waiting tankers */
        }
    }

    /* Enact pumping of one unit nitrogen */
    public void getNitrogen() {
        while (nitrogenLeft.decrementAndGet() < 0) {
            /* Since request was granted and fuel was resereved
             * just have to wait until units has been deposited
            */
            nitrogenLeft.incrementAndGet(); /* Restore past value */
        }
        try {
            Thread.sleep(ThreadLocalRandom.current().nextInt(1,11));
        } catch (InterruptedException ex) {}
    }

    /* Enact pumping of one unit quantum */
    public void getQuantum() {
        while (quantumLeft.decrementAndGet() < 0) {
            /* Since fuel request was granted and thus fuel reserved
             * just have to wait until more units have been deposited
             * as there are (most likley) several vehicles competeing
             * for the ones deposited
             * 
             * Implemented using spinning locks, perhaps change this
            */
            quantumLeft.incrementAndGet();
        }
        try {
            Thread.sleep(ThreadLocalRandom.current().nextInt(1,11));
        } catch (InterruptedException ex) {}
    }
    
    /* Functions to deposit fuel */
    public void depositNitrogen() {
        /* Deposit one unit
         * Spin while full
        */
        while (nitrogenLeft.incrementAndGet() > nitrogenCapacity)
            nitrogenLeft.decrementAndGet();
            
        try {
            Thread.sleep(ThreadLocalRandom.current().nextInt(1,11));
        } catch (InterruptedException ex) {}
    }
    public void depositQuantum() {
        /* Deposit one unit
         * Spinlock while full
        */
        while (quantumLeft.incrementAndGet() > quantumCapacity)
            quantumLeft.decrementAndGet();
        
        try {
            Thread.sleep(ThreadLocalRandom.current().nextInt(1,11));
        } catch (InterruptedException ex) {}
    }
    
    public synchronized boolean reserve(int nitrogen, int quantum) {
        if (nitrogen <= 0 && quantum <= 0) {
            /* Is tanker (...or idiot) */
            if ((-nitrogen)+nitrogenAvailable <= nitrogenCapacity &&
                (-quantum)+quantumAvailable <= quantumCapacity) {
                nitrogenAvailable += -nitrogen;
                quantumAvailable += -quantum;
                
                /* Wake waiting vechiles */
                synchronized (noFuel) {
                    noFuel.notifyAll();
                }
                return true;
            }
            else
                return false;
        }
        if (nitrogenAvailable >= nitrogen && quantumAvailable >= quantum) {
            nitrogenAvailable -= nitrogen;
            quantumAvailable -= quantum;
            return true;
        } else
            return false;
    }
    
    /* Try to enter one of the spots */
    private synchronized boolean tryFreeSlot() {
        if (freeSpots.get() > 0) {
            if (freeSpots.decrementAndGet() >= 0) 
                return true;
            else {
                /* Restore old value */
                freeSpots.incrementAndGet();
                return false;
            }
        }
        return false;
    }
}