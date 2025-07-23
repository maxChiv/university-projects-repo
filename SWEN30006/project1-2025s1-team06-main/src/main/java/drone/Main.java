package drone;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class Main {
    public static final String DEFAULT_PROPERTIES_PATH = "properties/test.properties";

    public static Properties loadPropertiesFile(String propertiesFile) {
        final Properties properties = PropertiesLoader.loadPropertiesFile(DEFAULT_PROPERTIES_PATH);
        System.out.println("Working Directory = " + System.getProperty("user.dir"));
        System.out.println("Properties file = " + propertiesFile);
        return properties;
    }

    public static void main(String[] args) {
        // assert false : "Exceptions are active!";
        String propertiesPath = (args.length > 0) ? args[0] : DEFAULT_PROPERTIES_PATH;
        final Properties properties = loadPropertiesFile(propertiesPath);
        new Simulation(properties).run();
        System.out.println("Finished");
    }
}