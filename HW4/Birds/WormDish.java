package HW4.Birds;

public class WormDish {
    private int cycleCounter;
    private int worms = 0;
    private final int capacity;
    private final int numCycle;
    private boolean done = false;

    public WormDish(int numCycle, int capacity) {
        this.numCycle = numCycle;
        this.capacity = capacity;
    }

    public synchronized void gatherWorms() throws InterruptedException {

        while (cycleCounter < numCycle) {
            while (worms > 0) {
                wait();
            }

            cycleCounter++;
            Thread.sleep(1000); // Simulates worm-gathering
            worms = capacity;
            System.out.println("Parent bird fills up the worm storage with (" + worms + "/" + capacity + ") worms.");

            notifyAll();
        }
        Thread.sleep(1000);
        done = true;
        notifyAll();
    }

    public synchronized boolean eatWorms(int babyBirdId) throws InterruptedException {
        while (worms == 0 && !done) {
            wait();
        }
        if (worms == 0 && done) {
            return false;
        }

        Thread.sleep(300); // Simulate eating
        worms--;
        System.out.println("Baby bird " + babyBirdId + " ate a worm! Worms left:(" + worms + "/" + capacity + ").");

        if (worms == 0) {
            System.out.println("Baby bird " + babyBirdId + " ate the last worm! Waking up the parent bird.");
            notifyAll();
        }
        return true;

    }

}
