package drone;

import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

class Logger implements Location.Observer {
    static PrintWriter printWriter;
    Logger(String filename) {
        FileWriter fileWriter;
        try {
            fileWriter = new FileWriter(filename);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        printWriter = new PrintWriter(fileWriter);
        Location.addObserver(this);
    }
    @Override
    public void notifyEvent(Location.Id id, String s, Location.DroneEvent e) {
        printWriter.printf("%5d: %s %s %s\n", Simulation.now(), id, s, e);
        printWriter.flush();
    }

    public void logEvent(String format, Object... args) {
        printWriter.printf(format, args);
        printWriter.flush();
    }
}
