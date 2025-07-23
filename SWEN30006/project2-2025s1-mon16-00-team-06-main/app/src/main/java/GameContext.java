import ch.aplu.jcardgame.*;
import ch.aplu.jgamegrid.*;

import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

public class GameContext {
    private final Hand playingArea;
    private final Player[] players;
    private final Properties properties;
    private String trumpSuit = null;
    private boolean isAuto;
    private final int[] autoIndexHands;
    private final List<List<String>> playerAutoMovements;

    // Human-specific selections
    private volatile Card selectedCardForHuman;
    private volatile String selectedTrumpForHuman;

    public GameContext(
            Hand playingArea,
            Player[] players,
            Properties properties,
            List<List<String>> playerAutoMovements,
            int[] autoIndexHands,
            boolean isAuto
    ) {
        this.playingArea = playingArea;
        this.players = players;
        this.properties = properties;
        this.playerAutoMovements = playerAutoMovements;
        this.autoIndexHands = autoIndexHands;
        this.isAuto = isAuto;
    }

    public Hand getPlayingArea() {
        return playingArea;
    }

    public Player[] getPlayers() {
        return players;
    }

    public Player getPlayer(int index) {
        if (index < 0 || index >= players.length) {
            throw new IndexOutOfBoundsException("Invalid player index: " + index);
        }
        return players[index];
    }

    public Properties getProperties() {
        return properties;
    }

    public String getTrumpSuit() {
        return trumpSuit;
    }

    public void setTrumpSuit(String trumpSuit) {
        this.trumpSuit = trumpSuit;
    }

    public boolean isAuto() {
        return isAuto;
    }

    public int[] getAutoIndexHands() {
        return autoIndexHands;
    }

    public List<List<String>> getplayerAutoMovements() {
        return playerAutoMovements;
    }

    public Card getSelectedCardForHuman() {
        return selectedCardForHuman;
    }

    public void setSelectedCardForHuman(Card card) {
        this.selectedCardForHuman = card;
    }

    public void clearSelectedCardForHuman() {
        this.selectedCardForHuman = null;
    }

    public String getSelectedTrumpForHuman() {
        return selectedTrumpForHuman;
    }

    public void setSelectedTrumpForHuman(String trumpSuit) {
        this.selectedTrumpForHuman = trumpSuit;
    }

    public void clearSelectedTrumpForHuman() {
        this.selectedTrumpForHuman = null;
    }

    public void setupPlayerAutoMovements(int nbPlayers) {
        String player0AutoMovement = properties.getProperty("players." + Pinochle.COMPUTER_PLAYER_INDEX + ".cardsPlayed");
        String player1AutoMovement = properties.getProperty("players." + Pinochle.HUMAN_PLAYER_INDEX + ".cardsPlayed");

        // Initialise playerAutoMovements with empty lists
        for (int i = 0; i < nbPlayers; i++) {
            playerAutoMovements.add(new ArrayList<>());
        }

        if (player0AutoMovement != null && !player0AutoMovement.isEmpty()) {
            playerAutoMovements.set(Pinochle.COMPUTER_PLAYER_INDEX, Arrays.asList(player0AutoMovement.split(",")));
        }
        if (player1AutoMovement != null && !player1AutoMovement.isEmpty()) {
            playerAutoMovements.set(Pinochle.HUMAN_PLAYER_INDEX, Arrays.asList(player1AutoMovement.split(",")));
        }
    }

    public Hand getPlayerHand(int playerIndex) {
        return getPlayer(playerIndex).getHand();
    }
}
