import ch.aplu.jcardgame.*;
import javax.swing.*;
import java.awt.*;
import java.util.List;
import java.util.ArrayList;

// Made this code with the help of the official Java Documentation:
// https://docs.oracle.com/en/java/javase/11/docs/api/java.desktop/javax/swing/JFrame.html?is-external=true

public class CardSelectionWindow extends JFrame {
    private final List<Card> selectedCards = new ArrayList<>();
    private final JButton confirmButton = new JButton("Confirm");
    private int numberToSelect;
    private JLabel statusLabel;

    // Craete the window to show the cards the user needs to select
    public CardSelectionWindow(List<Card> cards, int numberToSelect) {
        this.numberToSelect = numberToSelect;

        String plurality = numberToSelect == 1 ? "" : "s";
        setTitle("Select " + numberToSelect + " card" + plurality);
        setSize(1000, numberToSelect == 1 ? 200 : 600);
        setLayout(new BorderLayout());

        statusLabel = new JLabel(numberToSelect+ " more card" + plurality + " to select", SwingConstants.CENTER);
        add(statusLabel, BorderLayout.NORTH);

        int rows = (int) Math.ceil(cards.size() / 4.0);
        JPanel cardPanel = new JPanel(new GridLayout(rows, 12, 5, 5));
        add(cardPanel, BorderLayout.CENTER);

        for (Card card : cards) {
            JButton cardButton = new JButton(cardToString(card));
            cardButton.setBorder(BorderFactory.createLineBorder(Color.BLACK));
            cardButton.addActionListener(e -> {
                if (selectedCards.contains(card)) {
                    selectedCards.remove(card);
                    cardButton.setBorder(BorderFactory.createLineBorder(Color.BLACK));
                } else if (selectedCards.size() < numberToSelect) {
                    selectedCards.add(card);
                    cardButton.setBorder(BorderFactory.createLineBorder(Color.GREEN, 3));
                }
                confirmButton.setEnabled(selectedCards.size() == numberToSelect);
                if (selectedCards.size() == numberToSelect) {
                    statusLabel.setText("All done :) Click confirm to continue with the game!");
                } else {
                    statusLabel.setText((numberToSelect - selectedCards.size()) + " more to select");
                }
            });
            cardPanel.add(cardButton);
        }

        confirmButton.setEnabled(false);
        confirmButton.addActionListener(e -> dispose());
        add(confirmButton, BorderLayout.SOUTH);

        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    }

    // Show the actual popup window and return the cards the user chose
    public static ArrayList<Card> selectCards(List<Card> cards, int numberToSelect) {
        CardSelectionWindow window = new CardSelectionWindow(cards, numberToSelect);
        window.setVisible(true);

        // Wait until the window is closed
        while (window.isDisplayable()) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        return new ArrayList<>(window.selectedCards);
    }

    private String cardToString(Card card) {
        String rank = card.getRank().toString();
        String suit = card.getSuit().toString();
        return rank + " of " + suit;
    }

}