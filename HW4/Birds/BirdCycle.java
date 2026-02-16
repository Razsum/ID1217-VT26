package HW4.Birds;

public class BirdCycle {
    public static void main(String[] args) {
        int wormCapacity = 8;
        int numBabyBird = 3;

        WormDish worms = new WormDish(wormCapacity);
        ParentBird parentBird = new ParentBird(worms);
        BabyBird babyBird[] = new BabyBird[numBabyBird];
        parentBird.start();

        // Create and start baby bird array
        for (int i = 0; i < numBabyBird; i++) {
            babyBird[i] = new BabyBird(worms, i);
            babyBird[i].start();
        }
    }
}
