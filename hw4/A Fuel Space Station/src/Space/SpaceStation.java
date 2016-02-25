/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package Space;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

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
    private AtomicInteger freeSpots;
    
    private final Lock lock = new ReentrantLock();
    private final Condition noSpot = lock.newCondition();
    private final Condition noFuel = lock.newCondition();
    
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
    }
    
    public void enter(int nit, int quant) throws InterruptedException {
        /* Make sure there is enough fuel */
        while (!reserve(nit, quant)) {
            System.out.println("Couldn't reserve fuel, wait for tanker");
            noFuel.wait();
        }
        
        /* Drive upto a fueling slot */
        while (!tryFreeSlot()) {
            System.out.println("Didn't get a fueling slot, wait");
            freeSpots.wait();
        }
        
        /* Waiting by the fuel pump with enough fuel to top off */
    }
    
    /* Update counter of free spots and notify anyone in "queue" */
    public void leave() {
        freeSpots.incrementAndGet();
        freeSpots.notify();
    }

    /* Enact pumping of one unit nitrogen */
    public void getNitrogen() {
        if (nitrogenLeft.decrementAndGet() < 0)
            throw new IllegalArgumentException("Trying to steal fuel!");
    }

    /* Enact pumping of one unit quantum */
    public void getQuantum() {
        if (quantumLeft.decrementAndGet() < 0)
            throw new IllegalArgumentException("Trying to steal fuel!");
    }    
    
    /** INTERNALS **/
    private synchronized boolean reserve(int nitrogen, int quantum) {
        if (nitrogenAvailable >= nitrogen && quantumAvailable >= quantum) {
            nitrogenAvailable -= nitrogen;
            quantumAvailable -= quantum;
            return true;
        } else {
            return false;
        }
    }
    
    private synchronized boolean tryFreeSlot() {
        if (freeSpots.get() > 0) {
            freeSpots.incrementAndGet();
            return true;
        }
        return false;
    }
}