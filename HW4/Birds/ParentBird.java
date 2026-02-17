package HW4.Birds;

public class ParentBird extends Thread {
    private WormDish worms;

    public ParentBird(WormDish worms) {
        this.worms = worms;
    }

    @Override
    public void run() {
        try {
            worms.gatherWorms();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
