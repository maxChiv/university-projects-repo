import ch.aplu.jcardgame.Card;
import ch.aplu.jcardgame.Hand;

import java.util.Arrays;
import java.util.Random;
import java.util.stream.Collectors;
import java.util.List;


public abstract class Player {
    protected Hand hand;
    protected final int playerIndex;
    protected final GameContext gameContext;

    public Player(GameContext gameContext, int playerIndex) {
        this.gameContext = gameContext;
        this.playerIndex = playerIndex;
    }

    public abstract Card playCard();
    public abstract String chooseTrump();
    public abstract int getBid(int currentBid, BidManager bidManager, MeldManager meldManager);

    public Hand getHand() {
        return hand;
    }

    public void setHand(Hand hand) {
        this.hand = hand;
    }

    public int getPlayerIndex() {
        return playerIndex;
    }

    public abstract String getPlayerType();
}


class HumanPlayer extends Player {
    private final PinochleUI pinochleUI;

    public HumanPlayer(GameContext gameContext, PinochleUI pinochleUI) {
        super(gameContext, Pinochle.HUMAN_PLAYER_INDEX);
        this.pinochleUI = pinochleUI;
    }

    @Override
    public Card playCard() {
        // Easy refrencing
        List<String> autoMovements = gameContext.getplayerAutoMovements().get(playerIndex);
        int[] autoIndexHands = gameContext.getAutoIndexHands();

        Card cardToPlay = null;
        if (gameContext.isAuto() && autoMovements != null && autoIndexHands[playerIndex] < autoMovements.size()) {
            String nextMovement = autoMovements.get(autoIndexHands[playerIndex]);
            if (nextMovement != null && !nextMovement.isEmpty()) {
                cardToPlay = CardUtil.applyAutoMovement(this.hand, nextMovement);
                autoIndexHands[playerIndex]++;
            }
        }

        if (cardToPlay != null) {
            pinochleUI.delayOnce(); // Simulate seeing auto move
            return cardToPlay;
        }

        // Manual play
        this.hand.setTouchEnabled(true);
        pinochleUI.setStatus("Player " + playerIndex + " (" + getPlayerType() + ") is playing. Please double click on a card.");

        while (gameContext.getSelectedCardForHuman() == null) {
            pinochleUI.delayOnce(); // Simulate seeing auto move
        }
        cardToPlay = gameContext.getSelectedCardForHuman();

        gameContext.clearSelectedCardForHuman(); // Consume the selection
        // The validation (checkValidTrick) is done in Pinochle.handleHumanCardSelection before setting selectedCardForHuman
        return cardToPlay;
    }

    @Override
    public String chooseTrump() {
        if (gameContext.isAuto()) {
            return gameContext.getProperties().getProperty("players.trump", "S"); // Default to Spades if auto and not specified
        }

        pinochleUI.displayTrumpSelectionUI();
        gameContext.clearSelectedCardForHuman(); // Ensure its null

        while (gameContext.getSelectedTrumpForHuman() == null) {
            pinochleUI.delayOnce();
        }
        String chosenTrump = gameContext.getSelectedTrumpForHuman();
        gameContext.clearSelectedTrumpForHuman();
        pinochleUI.removeTrumpSelectionUI();
        return chosenTrump;
    }

    @Override
    public int getBid(int currentBid, BidManager bidManager, MeldManager meldManager) {
        // Easy refrencing
        List<Integer> humanAutoBids = bidManager.getHumanAutoBids();
        int humanAutoBidIndex = bidManager.getHumanAutoBidIndex();

        // Game logic: handle auto-human bid if enabled
        if (gameContext.isAuto() && humanAutoBids != null && humanAutoBidIndex < humanAutoBids.size()) {
            int humanBid = humanAutoBids.get(humanAutoBidIndex);
            // currentBid += humanBid; // Done upon return
            bidManager.incrementHumanAutoBidIndex();
            if (humanBid == 0) {
                bidManager.setHasHumanPassed(true);
            }
            bidManager.setHasHumanBid(true); // Auto-bid is always "confirmed"
            pinochleUI.updateBidTextUI(Pinochle.HUMAN_PLAYER_INDEX, currentBid, 0); // UI action: update display after auto-bid
            return humanBid;
        } else {
            bidManager.setHasHumanBid(false); // Reset flags to wait for user input
            bidManager.setHasHumanPassed(false);
            while (!bidManager.isHasHumanBid() && !bidManager.isHasHumanPassed()) pinochleUI.delayOnce(); // Game logic: wait for UI input
            return -1;
        }
    }

    @Override
    public String getPlayerType() {
        return "Human";
    }
}


class ComputerPlayer extends Player {
    ComputerBiddingStrategy biddingStrategy;
    ComputerTrickTakingStrategy trickTakingStrategy;
    private int thinkingTime;

    public ComputerPlayer(GameContext gameContext, ComputerBiddingStrategy biddingStrategy,
                          ComputerTrickTakingStrategy computerTrickTakingStrategy, int thinkingTime) {
        super(gameContext, Pinochle.COMPUTER_PLAYER_INDEX);
        this.biddingStrategy = biddingStrategy;
        this.trickTakingStrategy = computerTrickTakingStrategy;
        this.thinkingTime = thinkingTime;
    }

    @Override
    public Card playCard() {
        // Easy refrencing
        List<String> autoMovements = gameContext.getplayerAutoMovements().get(playerIndex);
        int[] autoIndexHands = gameContext.getAutoIndexHands();

        Card cardToPlay = null;
        // Computer always uses "auto" logic, either from file or its own AI
        if (autoMovements != null && autoIndexHands[playerIndex] < autoMovements.size()) {
            String nextMovement = autoMovements.get(autoIndexHands[playerIndex]);
            if (nextMovement != null && !nextMovement.isEmpty()) {
                cardToPlay = CardUtil.applyAutoMovement(this.hand, nextMovement);
                autoIndexHands[playerIndex]++;
            }
        }

        if (cardToPlay != null) {
            return cardToPlay;
        }

        // Not auto, so decide card.
        return trickTakingStrategy.playCard(this.hand, gameContext);
    }


    @Override
    public String chooseTrump() {

        if (gameContext.isAuto()) { // isAuto could also mean use predetermined from properties for computer
            return gameContext.getProperties().getProperty("players.trump", "S"); // Default to Spades
        }

        // Basic AI: choose the suit it has the most cards in, or random
        Suit selectedTrumpSuit = null;
        int maxCount = -1;

        for (Suit suit : Suit.values()) {
            int count = 0;
            for (Card card : this.hand.getCardList()) {
                if (card.getSuit() == suit) {
                    count++;
                }
            }
            if (count > maxCount) {
                maxCount = count;
                selectedTrumpSuit = suit;
            }
        }
        if (selectedTrumpSuit == null) { // Should not happen if hand is not empty
            selectedTrumpSuit = Arrays.stream(Suit.values()).findAny().get();
        }
        return selectedTrumpSuit.getSuitShortHand();
    }

    @Override
    public int getBid(int currentBid, BidManager bidManager, MeldManager meldManager) {
        // Easy Refrencing
        List<Integer> computerAutoBids = bidManager.getComputerAutoBids();
        int computerAutoBidIndex = bidManager.getComputerAutoBidIndex();

        int bidValue;
        // Game logic: determine computer's bid
        if (gameContext.isAuto() && computerAutoBids != null && computerAutoBidIndex < computerAutoBids.size()) {
            bidValue = computerAutoBids.get(computerAutoBidIndex);
            bidManager.incrementComputerAutoBidIndex();
        } else {
            bidValue = biddingStrategy.calculateBid(this.hand, gameContext, bidManager, meldManager);
        }
        return bidValue;
    }

    @Override
    public String getPlayerType() {
        return "Computer";
    }
}