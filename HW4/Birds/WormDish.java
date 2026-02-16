package HW4.Birds;

public class WormDish {
    private int worms = 0;
    private final int capacity;

    public WormDish(int capacity) {
        this.capacity = capacity;
    }

    public synchronized void gatherWorms() throws InterruptedException {
        while (worms > 0) {
            wait();
        }

        Thread.sleep(1000); // Simulates worm-gathering
        worms = capacity;
        System.out.println("Parent bird fills up the worm storage with (" + worms + "/" + capacity + ") worms.");

        notifyAll();
    }

    public synchronized void eatWorms(int babyBirdId) throws InterruptedException {
        while (worms == 0) {
            wait();
        }

        Thread.sleep(500); // Simulate eating
        worms--;
        System.out.println("Baby bird " + babyBirdId + " ate a worm! Worms left:(" + worms + "/" + capacity + ").");

        if (worms == 0) {
            System.out.println("Baby bird " + babyBirdId + " ate the last worm! Waking up the parent bird.");
            notifyAll();
        }

    }

}
