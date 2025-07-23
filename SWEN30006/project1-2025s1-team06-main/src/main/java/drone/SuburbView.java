package drone;

import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;

class SuburbView implements Location.Observer {

    @Override
    public void notifyEvent(Location.Id id, String s, Location.DroneEvent e) {
        int i, j;
        // We test for class here because we don't want suburb to know anything about SuburbView.
        // SuburbView needs to know everything about Suburb and the types of locations in it.
        if (id.getClass() == Suburb.StreetId.class) {
            Suburb.StreetId sid = (Suburb.StreetId) id;
            i = sid.street*3-2;
            j = (sid.oddHouse - 1)/2 + 2;
        } else if (id.getClass() == Suburb.AvenueId.class) {
            Suburb.AvenueId aid = (Suburb.AvenueId) id;
            i = aid.numSouth - 1;
            j = (aid.avenue == Suburb.Avenue.Out ? 1 : NUMHOUSES + 2);
        } else {
            throw new IllegalArgumentException("Unknown id type: " + id.getClass());
        }
        switch (e) {
            case arrive, endDelivery:
                tm.setValueAt("["+s+"]", i, j); break;
            case startDelivery:
                tm.setValueAt("v"+s+"v", i, j); break;
            case depart:
                tm.setValueAt("", i, j); break;
        }
    }

    final JFrame f;
    final TableModel tm;
    final int NUMSTREETS, NUMHOUSES;
    SuburbView(int NUMSTREETS, int NUMHOUSES){
        // Suburb s = Suburb.get();
        Location.addObserver(this);
        // System.out.println("Suburb retrieved");

        this.NUMSTREETS = NUMSTREETS;
        this.NUMHOUSES = NUMHOUSES;
        Object[][] objects = new Object[NUMSTREETS*3][NUMHOUSES+3];
        String[] headings = new String[NUMHOUSES+3];

        // Headings
        headings[0] = "Street";
        headings[1] = "Out Ave";
        headings[NUMHOUSES+3-1] = "Back Ave";
        for (int i = 1; i <= NUMHOUSES; i++) {
            headings[i+1] = "H" + i;
        }
        // System.out.println("Headings created");

        // Avenues
        for (int i = 0; i < NUMSTREETS*3; i++) {
            objects[i][1] = ""; // Road
            objects[i][NUMHOUSES+2] = ""; // Road
        }
        // System.out.println("Avenues created");

        // Streets and House numbers
        for (int i = 0; i < NUMSTREETS; i++) {
            objects[i * 3 + 1][0] = Suburb.StreetName.values[i] + " Street"; // Street name
            for (int j = 0; j < NUMHOUSES; j++) {
                objects[i * 3    ][j + 2] = Integer.toString(j * 2 + 1);  // Odd houses
                objects[i * 3 + 1][j + 2] = ""; // Road
                objects[i * 3 + 2][j + 2] = Integer.toString((j + 1) * 2); // Even houses
            }
        }
        // System.out.println("Streets created");

        tm = new DefaultTableModel(objects,headings);
        JTable jt = new JTable(tm);

        DefaultTableCellRenderer roadRenderer = new DefaultTableCellRenderer();
        roadRenderer.setHorizontalAlignment(JLabel.CENTER);
        roadRenderer.setBackground(Color.GRAY);
        roadRenderer.setForeground(Color.YELLOW);
        DefaultTableCellRenderer streetNameRenderer = new DefaultTableCellRenderer();
        streetNameRenderer.setHorizontalAlignment(JLabel.CENTER);
        streetNameRenderer.setBackground(Color.LIGHT_GRAY);

        class CustomRenderer extends DefaultTableCellRenderer
        {
            public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column)
            {
                Component cellComponent = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
                if(row % 3 == 1) {
                    cellComponent.setBackground(Color.GRAY);
                    cellComponent.setForeground(Color.YELLOW);
                } else {
                    cellComponent.setBackground(Color.BLACK);
                    cellComponent.setForeground(Color.WHITE);
                }
                return cellComponent;
            }
        }

        // Street Names
        jt.getColumnModel().getColumn(0).setCellRenderer(streetNameRenderer);
        jt.getColumnModel().getColumn(0).setPreferredWidth(50);

        // Avenues
        jt.getColumnModel().getColumn(1).setCellRenderer(roadRenderer);
        jt.getColumnModel().getColumn(1).setPreferredWidth(15);
        jt.getColumnModel().getColumn(NUMHOUSES+2).setCellRenderer(roadRenderer);
        jt.getColumnModel().getColumn(NUMHOUSES+2).setPreferredWidth(15);

        // Streets and houses
        CustomRenderer customRenderer = new CustomRenderer();
        customRenderer.setHorizontalAlignment(JLabel.CENTER);
        for (int i = 2; i < NUMHOUSES+2; i++) {
            jt.getColumnModel().getColumn(i).setCellRenderer(customRenderer);
            jt.getColumnModel().getColumn(i).setPreferredWidth(15);
        }
        // System.out.println("SuburbView created");

        jt.setBounds(100,100,800,600);
        jt.setRowHeight(50);
        JScrollPane sp=new JScrollPane(jt);
        f=new JFrame();
        f.setTitle("Suburb");
        f.add(sp);
        f.setSize(600,600);
        f.setVisible(true);
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

}
