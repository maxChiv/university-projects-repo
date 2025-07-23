import ch.aplu.jcardgame.*;
import ch.aplu.jgamegrid.*;


import java.awt.*;
import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

public class PinochleUI {
    private final Pinochle pinochle; // Store a reference to the CardGame (itself)

    public PinochleUI(Pinochle pinochle) {
        this.pinochle = pinochle; // Initialize the reference

        // Other UI elements
        playerBidActor = new TextActor("Bidding", Color.white, pinochle.bgColor, smallFont);
        currentBidActor = new TextActor("Current Bid: ", Color.white, pinochle.bgColor, smallFont);
        newBidActor = new TextActor("New Bid: ", Color.white, pinochle.bgColor, smallFont);
        trumpInstructionActor = new TextActor("Trump Selection", Color.white, pinochle.bgColor, smallFont);
    }

    /**
     * General UI Elements
     */
    private final Map<String, String> trumpImages = new HashMap<>(Map.of(
            Suit.SPADES.getSuitShortHand(), "sprites/bigspade.gif",
            Suit.CLUBS.getSuitShortHand(), "sprites/bigclub.gif",
            Suit.DIAMONDS.getSuitShortHand(), "sprites/bigdiamond.gif",
            Suit.HEARTS.getSuitShortHand(), "sprites/bigheart.gif"));

    Font bigFont = new Font("Arial", Font.BOLD, 36);
    Font smallFont = new Font("Arial", Font.BOLD, 18);

    private final Location[] handLocations = {  // Defines where players' hands are displayed.
            new Location(350, 625),
            new Location(350, 75),
    };
    private final Location[] scoreLocations = {  //  Defines where player scores are displayed.
            new Location(575, 675),
            new Location(25, 25),
    };
    private final TextActor[] scoreActors = {null, null, null, null};  // References to the TextActor objects used to display scores.
    private final Location playingLocation = new Location(350, 350);  // The central location for cards played in a trick.
    private final Location textLocation = new Location(350, 450);  // Location for general game status messages
    private final Location[] trickHandLocations = {
            new Location(75, 350),
            new Location(625, 350)
    };


    private Actor trumpActor; // The Actor object displaying the selected trump suit image.

    private final int handWidth = 400;
    private final int trickWidth = 40;

    /**
     * Bidding UI Elements
     */
    private final GGButton bidSelectionActor = new GGButton("sprites/bid_10.gif", false);  // The button for incrementing a bid.
    private final GGButton bidConfirmActor = new GGButton("sprites/done30.gif", false);  // The button for confirming a bid
    private final GGButton bidPassActor = new GGButton("sprites/bid_pass.gif", false);  // The button for passing during bidding
    private TextActor playerBidActor;  // Displays whose turn it is to bid
    private TextActor currentBidActor;  // Displays the current highest bid
    private TextActor newBidActor;  // Displays the human player's potential new bid

    private final Location bidSelectionLocation = new Location(600, 100);  // Location for the bid selection button
    private final Location bidConfirmLocation = new Location(660, 100);  // Location for the bid confirmation button
    private final Location bidPassLocation = new Location(630, 150);  // Location for the bid pass button
    private final Location playerBidLocation = new Location(550, 30);  // Location for the player bid text
    private final Location currentBidLocation = new Location(550, 50);  // Location for the current bid text
    private final Location newBidLocation = new Location(550, 75);  // Location for the new bid text

    /**
     * Trump Selection UI Elements
     */
    private final TextActor trumpInstructionActor;  // Text instructing players to select a trump suit
    private final GGButton clubTrumpActor = new GGButton("sprites/clubs_item.png", false);  // Button for selecting Clubs as trump
    private final GGButton spadeTrumpActor = new GGButton("sprites/spades_item.png", false);  // Button for selecting Spades as trump
    private final GGButton diamondTrumpActor = new GGButton("sprites/diamonds_item.png", false);  // Button for selecting Diamonds as trump
    private final GGButton heartTrumpActor = new GGButton("sprites/hearts_item.png", false);  // Button for selecting Hearts as trump


    private final Location trumpInstructionLocation = new Location(550, 80);  // Location for the trump instruction text
    private final Location clubTrumpLocation = new Location(580, 100);  // Location for the Clubs trump button
    private final Location spadeTrumpLocation = new Location(610, 100);  // Location for the Spades trump button
    private final Location diamondTrumpLocation = new Location(640, 100);  // Location for the Diamonds trump button
    private final Location heartTrumpLocation = new Location(670, 100);  // Location for the Hearts trump button
    private final Location trumpLocation = new Location(620, 120);  // Location to display the selected trump suit image


    /**
     * General UI Elements Methods
     */
    // Sets the game status text displayed on the screen.
    public void setStatus(String string) {
        pinochle.setStatusText(string);
    }

    // Initializes and displays the initial scores for all players.
    public void initScore(int nbPlayers, int[] scores) {
        for (int i = 0; i < nbPlayers; i++) {
            // scores[i] = 0;
            String text = "[" + String.valueOf(scores[i]) + "]";
            scoreActors[i] = new TextActor(text, Color.WHITE, pinochle.bgColor, bigFont);
            pinochle.addActor(scoreActors[i], scoreLocations[i]);
        }
    }

    //  Initializes and displays the initial scores for all players.
    public void updateScore(int player, int[] scores) {
        pinochle.removeActor(scoreActors[player]);
        int displayScore = Math.max(scores[player], 0);
        String text = "P" + player + "[" + String.valueOf(displayScore) + "]";
        scoreActors[player] = new TextActor(text, Color.WHITE, pinochle.bgColor, bigFont);
        pinochle.addActor(scoreActors[player], scoreLocations[player]);
    }

    // Updates and displays the selected trump suit image on the screen.
    public void updateTrumpActorUI(String trumpSuit) {
        if (trumpActor != null) {
            pinochle.removeActor(trumpActor); // Remove previous trump actor if one exists
        }
        String trumpImage = trumpImages.get(trumpSuit);
        trumpActor = new Actor(trumpImage);
        pinochle.addActor(trumpActor, trumpLocation);
    }

    public void displayGameOver(List<Integer> winners) {
        // Set the Game over text
        String winText;
        if (winners.size() == 1) {
            winText = "Game over. Winner is player: " + winners.iterator().next();
        } else if (winners.size() > 1) {
            winText = "Game Over. Drawn winners are players: " +
                    String.join(", ", winners.stream().map(String::valueOf).collect(Collectors.toList()));
        } else {
            winText = "Game Over. No winner (e.g., all passed, or all failed bids).";
        }
        pinochle.addActor(new Actor("sprites/gameover.gif"), textLocation);
        pinochle.setStatusText(winText);
        pinochle.refresh();
    }

    /**
     * Trump UI Elements Methods
     */

    // Displays the trump selection UI, including instruction text and suit buttons.
    // Attaches listeners that will report human choices back to Pinochle
    public void displayTrumpSelectionUI() {
        pinochle.addActor(trumpInstructionActor, trumpInstructionLocation);
        pinochle.addActor(clubTrumpActor, clubTrumpLocation);
        pinochle.addActor(spadeTrumpActor, spadeTrumpLocation);
        pinochle.addActor(heartTrumpActor, heartTrumpLocation);
        pinochle.addActor(diamondTrumpActor, diamondTrumpLocation);

        GGButtonListener buttonListener = new GGButtonListener() {
            @Override
            public void buttonPressed(GGButton ggButton) {
                String selectedSuit = null;
                if (ggButton.equals(clubTrumpActor)) {
                    selectedSuit = Suit.CLUBS.getSuitShortHand();
                } else if (ggButton.equals(spadeTrumpActor)) {
                    selectedSuit = Suit.SPADES.getSuitShortHand();
                } else if (ggButton.equals(heartTrumpActor)) {
                    selectedSuit = Suit.HEARTS.getSuitShortHand();
                } else if (ggButton.equals(diamondTrumpActor)) {
                    selectedSuit = Suit.DIAMONDS.getSuitShortHand();
                }
                // Call back to Pinochle to handle the trump selection logic
                if (selectedSuit != null) {
                    pinochle.handleHumanTrumpSelection(selectedSuit);
                }
            }
            @Override public void buttonReleased(GGButton ggButton) {}
            @Override public void buttonClicked(GGButton ggButton) {}
        };

        clubTrumpActor.addButtonListener(buttonListener);
        spadeTrumpActor.addButtonListener(buttonListener);
        heartTrumpActor.addButtonListener(buttonListener);
        diamondTrumpActor.addButtonListener(buttonListener);
    }

    // Removes all trump selection UI elements from the screen.
    public void removeTrumpSelectionUI() {
        pinochle.removeActor(trumpInstructionActor);
        pinochle.removeActor(clubTrumpActor);
        pinochle.removeActor(spadeTrumpActor);
        pinochle.removeActor(heartTrumpActor);
        pinochle.removeActor(diamondTrumpActor);
    }

    public void playerChooseTrumpTextDisplay(Player player) {
        pinochle.setStatusText("Player " + player.getPlayerIndex() + " (" + player.getPlayerType() + ") choosing trump...");
        delayOnce();
    }


    /**
     * Initalising game
     */

    // This method displays the initial game state (hands, playing area, trick winning areas)
    public void displayInitialGameUI(Player[] players, Hand[] trickWinningHands, Hand playingArea) {
        Hand[] hands = new Hand[players.length]; // Create an array to store each player's hand

        for (int i = 0; i < players.length; i++) {
            hands[i] = players[i].getHand(); // Assuming getHand() is a method in Player interface
        }

        playingArea.setView(pinochle, new RowLayout(playingLocation, (playingArea.getNumberOfCards() + 3) * trickWidth));
        playingArea.draw();

        RowLayout[] layouts = new RowLayout[hands.length];
        for (int i = 0; i < hands.length; i++) {
            layouts[i] = new RowLayout(handLocations[i], handWidth);
            layouts[i].setRotationAngle(180 * i);
            hands[i].setView(pinochle, layouts[i]);
            hands[i].setTargetArea(new TargetArea(playingLocation));
            hands[i].draw();
        }

        RowLayout[] trickHandLayouts = new RowLayout[trickWinningHands.length];
        for (int i = 0; i < trickWinningHands.length; i++) {
            trickHandLayouts[i] = new RowLayout(trickHandLocations[i], handWidth);
            trickHandLayouts[i].setRotationAngle(90 + 180 * i);
            trickWinningHands[i].setView(pinochle, trickHandLayouts[i]);
            trickWinningHands[i].draw();
        }
    }

    // This method enables human player interaction with their hand
    public void enableHumanHandInteraction(Hand humanHand, String currentTrumpSuit) {
        CardListener cardListener = new CardAdapter() {
            public void leftDoubleClicked(Card card) {
                // Call back to Pinochle to handle the card selection logic
                pinochle.handleHumanCardSelection(card);
            }
        };
        humanHand.addCardListener(cardListener);
        humanHand.setTouchEnabled(true);
    }

    // This method disables human player interaction with their hand
    public void disableHumanHandInteraction(Hand humanHand) {
        humanHand.setTouchEnabled(false);
        // Consider removing the listener here if it's not needed until the next turn
    }

    /**
     * Bidding UI Elements Methods
     */
    // Initializes and displays all bidding-related actors and attaches listeners.
    public void setupBiddingUI() {
        // Use the 'pinochle' reference to call CardGame methods
        pinochle.addActor(bidSelectionActor, bidSelectionLocation);
        pinochle.addActor(bidConfirmActor, bidConfirmLocation);
        pinochle.addActor(bidPassActor, bidPassLocation);

        pinochle.addActor(playerBidActor, playerBidLocation);
        pinochle.addActor(currentBidActor, currentBidLocation);
        pinochle.addActor(newBidActor, newBidLocation);

        pinochle.setActorOnTop(bidSelectionActor);
        pinochle.setActorOnTop(bidConfirmActor);
        pinochle.setActorOnTop(bidPassActor);

        // Initial state for buttons
        bidSelectionActor.setActEnabled(false);
        bidConfirmActor.setActEnabled(false);
        bidPassActor.setActEnabled(false);

        // Attach listeners that call back to Pinochle for game logic
        bidSelectionActor.addButtonListener(new GGButtonListener() {
            @Override
            public void buttonPressed(GGButton ggButton) {
                pinochle.getBidManager().handleHumanBidIncrement();
            }
            @Override public void buttonReleased(GGButton ggButton) {}
            @Override public void buttonClicked(GGButton ggButton) {}
        });

        bidConfirmActor.addButtonListener(new GGButtonListener() {
            @Override
            public void buttonPressed(GGButton ggButton) {
                pinochle.getBidManager().handleHumanBidConfirm();
            }
            @Override public void buttonReleased(GGButton ggButton) {}
            @Override public void buttonClicked(GGButton ggButton) {}
        });

        bidPassActor.addButtonListener(new GGButtonListener() {
            @Override
            public void buttonPressed(GGButton ggButton) {
                pinochle.getBidManager().handleHumanBidPass();
            }
            @Override public void buttonReleased(GGButton ggButton) {}
            @Override public void buttonClicked(GGButton ggButton) {}
        });
    }

     // Removes all bidding-related actors (buttons and text) from the screen
    public void removeBiddingUI() {
        pinochle.removeActor(bidSelectionActor);
        pinochle.removeActor(bidConfirmActor);
        pinochle.removeActor(bidPassActor);
        removeBidTextUI(); // Call the specific text removal method
    }

    //Removes only the bid text actors (current, new, player bid) from the screen.
    public void removeBidTextUI() {
        pinochle.removeActor(currentBidActor);
        pinochle.removeActor(newBidActor);
        pinochle.removeActor(playerBidActor);
    }

    // Updates the text displayed for the current bid, new bid, and the current bidding player.
    // This method receives the current game state values from Pinochle.
    public void updateBidTextUI(int playerIndex, int currentBidValue, int newBidValue) {
        removeBidTextUI(); // Clear previous text actors

        String playerBidString = "";
        switch (playerIndex) {
            case -1: playerBidString = "Bidding"; break;
            case 0: playerBidString = "Computer Bid"; break;
            case 1: playerBidString = "Human Bid"; break;
            default: playerBidString = "Bid"; break;
        }

        // Recreate and add current bid actor
        currentBidActor = new TextActor("Current Bid: " + currentBidValue, Color.WHITE, pinochle.bgColor, smallFont);
        pinochle.addActor(currentBidActor, currentBidLocation);

        // Recreate and add new bid actor
        String newBidString = newBidValue == 0 ? "" : String.valueOf(newBidValue);
        newBidActor = new TextActor("New Bid: " + newBidString, Color.WHITE, pinochle.bgColor, smallFont);
        pinochle.addActor(newBidActor, newBidLocation);

        // Recreate and add player bid actor
        playerBidActor = new TextActor(playerBidString, Color.WHITE, pinochle.bgColor, smallFont);
        pinochle.addActor(playerBidActor, playerBidLocation);

        pinochle.delay(pinochle.getUIDelayTime()); // Use delay from Pinochle's properties
    }

    // Displays or hides (enables/disables) the human player's bidding input buttons.
    public void displayBidButtons(boolean isShown) {
        bidSelectionActor.setActEnabled(isShown);
        bidConfirmActor.setActEnabled(isShown);
        bidPassActor.setActEnabled(isShown);
    }


    // Updates and displays the final bid result (winner and score).
    public void updateBidResultUI(int finalBid, int winningPlayerIndex) {
        removeBidTextUI(); // Clear previous bid text

        currentBidActor = new TextActor("Final Bid: " + finalBid, Color.WHITE, pinochle.bgColor, smallFont);
        pinochle.addActor(currentBidActor, currentBidLocation);

        String playerBidString = winningPlayerIndex == Pinochle.COMPUTER_PLAYER_INDEX ? "Computer Win" : "Human Win";
        playerBidActor = new TextActor(playerBidString, Color.WHITE, pinochle.bgColor, smallFont);
        pinochle.addActor(playerBidActor, playerBidLocation);

        pinochle.delay(pinochle.getUIDelayTime() * 2); // Longer delay for result
    }

    // Delays the UI
    public void delayOnce() {
        pinochle.delay(pinochle.getUIDelayTime());
    }


    /**
     * Card Interaction Methods
     */
    // Handles the visual transfer of cards from the playing area to the trick winner's pile, including redrawing
    // the trick winning hands.
    private void transferCardsToWinnerUI(int trickWinPlayerIndex, int nbPlayers, Hand playingArea, Hand[] trickWinningHands) {
        for (Card card : playingArea.getCardList()) {
            trickWinningHands[trickWinPlayerIndex].insert(card, true);
        }
        playingArea.removeAll(true);
        RowLayout[] trickHandLayouts = new RowLayout[nbPlayers];
        pinochle.delay(pinochle.getUIDelayTime());
        for (int i = 0; i < nbPlayers; i++) {
            trickHandLayouts[i] = new RowLayout(trickHandLocations[i], handWidth);
            trickHandLayouts[i].setRotationAngle(90);
            trickWinningHands[i].setView(pinochle, trickHandLayouts[i]);
            trickWinningHands[i].draw();
        }

        pinochle.delay(pinochle.getUIDelayTime());
    }

    // Updates and displays the cards in the central playing area (for trick taking)
    public void updatePlayingAreaUI(Hand playingArea, boolean isTrickComplete, int trickWinPlayerIndex, int nbPlayers, Hand[] trickWinningHands) {
        // Always update the playing area for the current card played
        playingArea.setView(pinochle, new RowLayout(playingLocation, (playingArea.getNumberOfCards() + 2) * trickWidth));
        playingArea.draw();

        // If the trick is complete, visually transfer cards to the winner
        if (isTrickComplete) {
            pinochle.delay(pinochle.getUIDelayTime() * 2); // Longer delay to see the full trick before transfer
            transferCardsToWinnerUI(trickWinPlayerIndex, nbPlayers, playingArea, trickWinningHands);
        }
    }

    public void setNewPlayerHandsUI(Player[] players, Card[] newComputerCards, Card[] newHumanCards) {
        Hand[] hands = new Hand[players.length];

        // --- Human Player Hand Update ---
        Hand humanHand = players[Pinochle.HUMAN_PLAYER_INDEX].getHand();
        List<Card> newHumanCardsList = Arrays.asList(newHumanCards);

        // Add new cards first (will draw them)
        for (Card c : newHumanCards) {
            // Check if card is already in hand to prevent re-inserting and potential duplicates
            if (!humanHand.getCardList().contains(c)) {
                humanHand.insert(c, true);
            }
        }

        // Remove old cards that are no longer supposed to be there
        List<Card> currentHumanCards = new ArrayList<>(humanHand.getCardList()); // Work on a copy
        for (Card c : currentHumanCards) {
            if (!newHumanCardsList.contains(c)) {
                humanHand.remove(c, true);
            }
        }

        // --- Computer Player Hand Update ---
        Hand computerHand = players[Pinochle.COMPUTER_PLAYER_INDEX].getHand();
        List<Card> newComputerCardsList = Arrays.asList(newComputerCards);

        // Add new cards first
        for (Card c : newComputerCards) {
            if (!computerHand.getCardList().contains(c)) {
                computerHand.insert(c, true);
            }
        }

        // Remove old cards that are no longer supposed to be there
        List<Card> currentComputerCards = new ArrayList<>(computerHand.getCardList()); // Work on a copy
        for (Card c : currentComputerCards) {
            if (!newComputerCardsList.contains(c)) {
                computerHand.remove(c, true);
            }
        }

        // Re-apply layouts and redraw all hands (might be redundant due to insert(true)/remove(true),
        // but ensures view properties are up-to-date and all cards are consistently drawn).
        for (int i = 0; i < players.length; i++) {
            hands[i] = players[i].getHand();
        }

        RowLayout[] layouts = new RowLayout[hands.length];
        for (int i = 0; i < hands.length; i++) {
            layouts[i] = new RowLayout(handLocations[i], handWidth);
            layouts[i].setRotationAngle(180 * i);
            hands[i].setView(pinochle, layouts[i]);
            hands[i].draw();
        }

    }
}
