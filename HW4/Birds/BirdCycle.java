package HW4.Birds;

public class BirdCycle {
    public static void main(String[] args) {
        int wormCapacity = 8;
        int numBabyBird = 3;
        int numCycle = 3;

        WormDish worms = new WormDish(numCycle, wormCapacity);
        ParentBird parentBird = new ParentBird(worms);
        BabyBird babyBird[] = new BabyBird[numBabyBird];

        // Create and start baby bird array
        parentBird.start();
        for (int i = 0; i < numBabyBird; i++) {
            babyBird[i] = new BabyBird(worms, i);
            babyBird[i].start();
        }
        
        try {
            parentBird.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        for (int i = 0; i < numBabyBird; i++) {
            try {
                babyBird[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        System.out.println("There's no more worms left! Ending after "+ numCycle +" cycles");
    }
}
