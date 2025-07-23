import drone.PropertiesLoader;
import drone.Simulation;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.Properties;
import java.util.Scanner;

public class TestDefaultProperties {
    public static Properties loadPropertiesFile(String propertiesFile) {
        final Properties properties = PropertiesLoader.loadPropertiesFile(propertiesFile);
        System.out.println("Working Directory = " + System.getProperty("user.dir"));
        System.out.println("Properties file = " + propertiesFile);
        return properties;
    }

    public String defaultLogs() {
        try (InputStream input = TestDefaultProperties.class.getClassLoader().getResourceAsStream("default_log.txt")) {
            assert input != null;
            Scanner s = new Scanner(input).useDelimiter("\\A");
            return s.hasNext() ? s.next() : "";
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    @org.junit.jupiter.api.Test
    public void testDefaultProperties() throws FileNotFoundException {
        final Properties properties = loadPropertiesFile("");
        String defaultLogs = defaultLogs();
        System.out.println("defaultLogs = " + defaultLogs);

        new Simulation(properties).run();

        Scanner scanner = new Scanner(new File("logfile.txt"));
        StringBuilder stringBuilder = new StringBuilder();
        while (scanner.hasNextLine()) {
            String line = scanner.nextLine();
            if (line.contains("Finished")) {
                stringBuilder.append(line);
            }
        }
        String logs = stringBuilder.toString();
        Assertions.assertEquals(defaultLogs, logs, "Logs should be the same");
    }
}
