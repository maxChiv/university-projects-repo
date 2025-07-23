import ch.aplu.jcardgame.*;
import ch.aplu.jgamegrid.*;

import java.awt.*;
import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

public class BidManager {
    private final PinochleUI pinochleUI;

    /**
     * Bidding elements
     */
    private int currentBid = 0;
    private boolean hasHumanBid = false;
    private int humanBid = 0;
    static final int BID_SELECTION_VALUE = 10;
    static final int MAX_SINGLE_BID = 20;
    private boolean hasComputerPassed = false;
    private boolean hasHumanPassed = false;
    private int firstBidIndex;
    private int bidWinPlayerIndex = 0;
    private final List<Integer> computerAutoBids = new ArrayList<>();
    private final List<Integer> humanAutoBids = new ArrayList<>();
    private int computerAutoBidIndex = 0;
    private int humanAutoBidIndex = 0;

    public static final String RANDOM_BID = "random";
    public static final String COMPUTER_BID = "computer";
    public static final String HUMAN_BID = "human";

    public BidManager(PinochleUI pinochleUI) {
        this.pinochleUI = pinochleUI;
    }

    /**
     * Bid Section
     */

    /**
     * Handles the human player clicking the "increment bid" button.
     * Updates internal game state for human's pending bid.
     */
    public void handleHumanBidIncrement() {
        hasHumanBid = false; // Indicate human is still adjusting bid
        if (humanBid >= MAX_SINGLE_BID) {
            pinochleUI.displayBidButtons(false); // UI action: disable specific button
            pinochleUI.setStatus("Maximum amount of a single bid reached"); // UI action: update status
        } else {
            humanBid += BID_SELECTION_VALUE;
        }
        pinochleUI.updateBidTextUI(Pinochle.HUMAN_PLAYER_INDEX, currentBid, humanBid); // UI action: update display
    }

    /**
     * Handles the human player clicking the "confirm bid" button.
     * Updates internal game state for the confirmed bid.
     */
    public void handleHumanBidConfirm() {
        currentBid += humanBid; // Update game state: apply human's pending bid
        hasHumanBid = true; // Update game state: human bid confirmed
        humanBid = 0;       // Update game state: reset pending bid

        pinochleUI.updateBidTextUI(Pinochle.HUMAN_PLAYER_INDEX, currentBid, 0); // UI action: display new current bid
        pinochleUI.setStatus(""); // UI action: clear status
    }

    /**
     * Handles the human player clicking the "pass" button.
     * Updates internal game state to indicate human player has passed.
     */
    public void handleHumanBidPass() {
        humanBid = 0;           // Update game state: clear pending bid
        hasHumanPassed = true; // Update game state: human passed
        pinochleUI.updateBidTextUI(Pinochle.HUMAN_PLAYER_INDEX, currentBid, 0); // UI action: display no new bid
        pinochleUI.setStatus(""); // UI action: clear status
    }

    /**
     * Manages the overall bidding process between players.
     * Orchestrates UI interaction and game state updates.
     */
    public void askForBid(GameContext gameContext ,MeldManager meldManager) {
        Properties properties = gameContext.getProperties();
        Player[] players = gameContext.getPlayers();
        int nbPlayers = players.length;
        boolean isAuto = gameContext.isAuto();

        pinochleUI.setupBiddingUI(); // UI action: initialize bid actors and listeners
        pinochleUI.displayBidButtons(false); // UI action: initially hide human bid buttons

        setupPlayerAutoBids(properties); // Game logic: load auto-bidding data

        boolean isContinueBidding = true;
        pinochleUI.updateBidTextUI(-1, 0, 0); // UI action: initial bid prompt

        // Game logic: Determine who bids first
        Random rand = new Random(Pinochle.seed); // Use class seed for consistency
        String bidOrder = properties.getProperty("players.bid_first", "random");
        int playerIndex = switch (bidOrder) {
            case RANDOM_BID -> rand.nextInt(nbPlayers);
            case COMPUTER_BID -> Pinochle.COMPUTER_PLAYER_INDEX;
            case HUMAN_BID -> Pinochle.HUMAN_PLAYER_INDEX;
            default -> Pinochle.COMPUTER_PLAYER_INDEX;
        };
        firstBidIndex = playerIndex;

        do {
            for (int i = 0; i < nbPlayers; i++) {
                askForBidForPlayerIndex(players[playerIndex], meldManager, isAuto); // Game logic: handle a single player's bid
                playerIndex = (playerIndex + 1) % nbPlayers; // Game logic: next player
                isContinueBidding = !hasHumanPassed && !hasComputerPassed; // Game logic: check if bidding continues
                if (!isContinueBidding) {
                    bidWinPlayerIndex = playerIndex; // Game logic: determine winner
                    break;
                }
            }
        } while (isContinueBidding);

        pinochleUI.removeBiddingUI(); // UI action: remove bidding elements
        pinochleUI.updateBidResultUI(currentBid, bidWinPlayerIndex); // UI action: display final result
        Pinochle.logger.addBidInfoToLog(bidWinPlayerIndex, currentBid); // Game logic: logging
    }

    // Handles the bidding turn for a specific player (computer or human).
    // Contains the core bidding rules and interaction flow.
    private void askForBidForPlayerIndex(Player player, MeldManager meldManager, boolean isAuto) {
        int playerIndex = player.getPlayerIndex();
        if (playerIndex == Pinochle.COMPUTER_PLAYER_INDEX) {
            int bidValue = player.getBid(currentBid, this, meldManager);

            pinochleUI.updateBidTextUI(playerIndex, currentBid, currentBid + bidValue); // UI action: display computer's potential bid

            // Game logic: update computer's pass status and current bid
            if (bidValue == 0) {
                hasComputerPassed = true;
            }
            currentBid += bidValue;

            pinochleUI.updateBidTextUI(playerIndex, currentBid, 0); // UI action: display new current bid
        } else { // Human Player
            pinochleUI.displayBidButtons(true); // UI action: show human bid buttons
            pinochleUI.updateBidTextUI(playerIndex, currentBid, humanBid); // UI action: initial display for human bid

            if (isAuto) {
                humanBid = player.getBid(currentBid, this, meldManager);
                currentBid += humanBid;
            } else {
                // Handled by the Handler for human bid as its UI interaction
                player.getBid(currentBid, this, meldManager);
            }
            pinochleUI.displayBidButtons(false); // UI action: hide buttons after human's turn
        }
    }

    // Loads auto-bidding values from properties. (Game setup/logic)
    private void setupPlayerAutoBids(Properties properties) {
        String player0Bids = properties.getProperty("players.0.bids", "");
        String player1Bids = properties.getProperty("players.1.bids", "");

        if (!player0Bids.isEmpty()) {
            computerAutoBids.addAll(Arrays.asList(player0Bids.split(",")).stream().map(Integer::parseInt).toList());
        } else {
            computerAutoBids.clear(); // Ensure it's empty if no property is set
        }

        if (!player1Bids.isEmpty()) {
            humanAutoBids.addAll(Arrays.asList(player1Bids.split(",")).stream().map(Integer::parseInt).toList());
        } else {
            humanAutoBids.clear(); // Ensure it's empty if no property is set
        }
    }

    public int getCurrentBid() {
        return currentBid;
    }

    public int getBidWinPlayerIndex() {
        return bidWinPlayerIndex;
    }

    public boolean isHasHumanBid() {
        return hasHumanBid;
    }

    public boolean isHasHumanPassed() {
        return hasHumanPassed;
    }

    public List<Integer> getComputerAutoBids() {
        return computerAutoBids;
    }

    public List<Integer> getHumanAutoBids() {
        return humanAutoBids;
    }

    public int getComputerAutoBidIndex() {
        return computerAutoBidIndex;
    }

    public int getHumanAutoBidIndex() {
        return humanAutoBidIndex;
    }

    public void incrementHumanAutoBidIndex() {
        humanAutoBidIndex++;
    }

    public void incrementComputerAutoBidIndex() {
        computerAutoBidIndex++;
    }

    public void setHasHumanBid(boolean hasHumanBid) {
        this.hasHumanBid = hasHumanBid;
    }

    public void setHasHumanPassed(boolean hasHumanPassed) {
        this.hasHumanPassed = hasHumanPassed;
    }

    public boolean isFirstBidderComputer() {
        return firstBidIndex == Pinochle.COMPUTER_PLAYER_INDEX;
    }
}
