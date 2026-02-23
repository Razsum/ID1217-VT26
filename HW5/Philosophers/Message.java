package HW5.Philosophers;

import java.io.Serializable;

// Serializable - Makes the object convertable into bytestream to be sent over network 
public class Message implements Serializable {
    public enum Type {
        REQUEST, RELEASE, GRANTED
    }

    public Type type;
    public int id;

    public Message(Type type, int id) {
        this.type = type;
        this.id = id;
    }
}