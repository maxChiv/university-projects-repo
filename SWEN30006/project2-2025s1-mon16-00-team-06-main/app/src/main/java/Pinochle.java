import ch.aplu.jcardgame.*;
import ch.aplu.jgamegrid.*;

import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

@SuppressWarnings("serial")
public class Pinochle extends CardGame {
    public static final int COMPUTER_PLAYER_INDEX = 0;
    public static final int HUMAN_PLAYER_INDEX = 1;

    static public final int seed = 30008;

    private final String version = "1.0";

    public final int nbPlayers = 2;
    public final int nbStartCards = 12;
    private int[] scores = new int[nbPlayers];
    private final Deck deck = new Deck(Suit.values(), Rank.values(), "cover");

    private int delayTime = 600; // General UI delay

    /**
     * OTHER CLASS ASSOCIATIONS
     */
    private final GameContext gameContext;
    static final Logger logger = new Logger(); // Made static for potential use in Player classes if needed directly
    private final PinochleUI pinochleUI = new PinochleUI(this);
    // Assuming BidManager and MeldManager are already refactored and don't directly
    // depend on HUMAN_PLAYER_INDEX checks internally for these actions
    private final BidManager bidManager = new BidManager(pinochleUI); // Pass Pinochle reference if BidManager needs
                                                                      // game context (like properties, delay, isAuto)
    private final MeldManager meldManager;
    private final TrickManager trickManager = new TrickManager(this.pinochleUI, nbPlayers, deck);
    private final GameMode gameMode;
    private boolean isCutthroat;

    public Pinochle(Properties properties) {
        super(700, 700, 30);

        // Read configuration
        boolean isAuto = Boolean.parseBoolean(properties.getProperty("isAuto"));
        int thinkingTime = Integer.parseInt(properties.getProperty("thinkingTime", "2000"));
        delayTime = Integer.parseInt(properties.getProperty("delayTime", "600"));

        // Create player list
        Player[] players = new Player[nbPlayers];

        // Create empty dummy hand and list of movements (will be replaced in
        // initialiseGameHandsAndDeal)
        Hand playingArea = new Hand(deck);
        int[] autoIndexHands = new int[nbPlayers];
        List<List<String>> playerAutoMovements = new ArrayList<>();

        // Initialise GameContext first so it can be passed to players
        this.gameContext = new GameContext(
                playingArea,
                players,
                properties,
                playerAutoMovements,
                autoIndexHands,
                isAuto);

        // check meld manager
        boolean isAdditionalMelds = Boolean.parseBoolean(properties.getProperty("melds.additional"));
        meldManager = new MeldManager(isAdditionalMelds);

        // Initialise players with GameContext
        // CHANGE HOW THE COMPUTER PLAYS WITH ITS INITALISATION
        boolean isSmartTrick = Boolean.parseBoolean(properties.getProperty("mode.smarttrick"));
        boolean isSmartBid = Boolean.parseBoolean(properties.getProperty("players.0.smartbids"));
        if(isSmartTrick && isSmartBid) {
            gameContext.getPlayers()[COMPUTER_PLAYER_INDEX] = (new ComputerPlayer(
                    gameContext,
                    new SmartComputerBiddingStrategy(),
                    new SmartTrickTakingStrategy(new RootRule()),
                    thinkingTime)); // Index 0
        } else if (isSmartTrick && !isSmartBid) {
            gameContext.getPlayers()[COMPUTER_PLAYER_INDEX] = (new ComputerPlayer(
                    gameContext,
                    new RandomComputerBiddingStrategy(),
                    new SmartTrickTakingStrategy(new RootRule()),
                    thinkingTime)); // Index 0
        } else if (!isSmartTrick && isSmartBid) {
            gameContext.getPlayers()[COMPUTER_PLAYER_INDEX] = (new ComputerPlayer(
                    gameContext,
                    new SmartComputerBiddingStrategy(),
                    new DefaultTrickTakingStrategy(),
                    thinkingTime)); // Index 0
        } else {
            gameContext.getPlayers()[COMPUTER_PLAYER_INDEX] = (new ComputerPlayer(
                    gameContext,
                    new RandomComputerBiddingStrategy(),
                    new DefaultTrickTakingStrategy(),
                    thinkingTime)); // Index 0
        }

        gameContext.getPlayers()[HUMAN_PLAYER_INDEX] = (new HumanPlayer(gameContext, this.pinochleUI)); // Index 1


        this.isCutthroat = Boolean.parseBoolean(properties.getProperty("mode.cutthroat"));
        if (this.isCutthroat) {
            this.gameMode = new CutthroatMode(deck, gameContext, this, pinochleUI);
        } else {
            this.gameMode = new StandardMode(deck, gameContext, this, pinochleUI);
        }


    }

    /**
     * INITIALISING AND RUNNING
     */

    private void initScores() {
        Arrays.fill(scores, 0);
    }

    private void initGame() {
        initialiseGameHandsAndDeal();
        // Pass Player array for UI to get hands

        pinochleUI.displayInitialGameUI(gameContext.getPlayers(), trickManager.getTrickWinningHands(),
                gameContext.getPlayingArea());
        pinochleUI.enableHumanHandInteraction(gameContext.getPlayer(HUMAN_PLAYER_INDEX).getHand(),
                gameContext.getTrumpSuit()); // Enable interaction for human player
    }

    private void initialiseGameHandsAndDeal() {
        Player[] players = gameContext.getPlayers();
        for (int i = 0; i < nbPlayers; i++) {
            Hand newHand = new Hand(deck);
            players[i].setHand(newHand); // Assign a new hand to each player
        }

        gameMode.dealingOut(players, nbPlayers, nbStartCards); // Pass Player array

        for (int i = 0; i < nbPlayers; i++) {
            players[i].getHand().sort(Hand.SortType.SUITPRIORITY, false);
        }
    }

    public String runApp() {
        setTitle("Pinochle (V" + version + ") - Player Abstraction");
        setStatusText("Initialising...");
        initScores(); // Scores are 0
        pinochleUI.initScore(nbPlayers, scores); // UI shows 0
        gameContext.setupPlayerAutoMovements(nbPlayers);
        initGame(); // Deals cards to players, UI displays hands
        gameMode.playGame(); // Bidding, Trump, Melding (scores updated with meld), Tricks (scores updated
                             // with tricks)

        // Final check for bid winner
        int bidWinnerIdx = bidManager.getBidWinPlayerIndex();
        if (bidWinnerIdx != -1) { // If there was a bid winner
            if (scores[bidWinnerIdx] < bidManager.getCurrentBid()) {
                pinochleUI.setStatus("Player " + bidWinnerIdx + " failed to make the bid of "
                        + bidManager.getCurrentBid() + "! Score for round is 0.");
                scores[bidWinnerIdx] = 0; // Or negative points for the bid amount
            } else {
                pinochleUI.setStatus("Player " + bidWinnerIdx + " made the bid of " + bidManager.getCurrentBid() + "!");
            }
        }

        for (int i = 0; i < nbPlayers; i++)
            pinochleUI.updateScore(i, scores); // Final UI score update

        int maxScore = 0;
        boolean hasWinner = false;
        for (int i = 0; i < nbPlayers; i++) {
            if (scores[i] > maxScore) {
                maxScore = scores[i];
                hasWinner = true;
            }
        }
        if (!hasWinner && bidManager.getCurrentBid() == 0) { // e.g. both passed initially
            maxScore = -1; // Indicate no real winner if all scores are 0 and no bid
        }

        List<Integer> winners = new ArrayList<Integer>();
        if (maxScore > 0 || (maxScore == 0 && bidManager.getCurrentBid() > 0)) { // Bid was made, someone could win with
                                                                                 // 0 if opponent went negative/failed
                                                                                 // bid
            for (int i = 0; i < nbPlayers; i++)
                if (scores[i] == maxScore)
                    winners.add(i);
        }

        // Set the Game over text
        pinochleUI.displayGameOver(winners);

        // Prepare hands for logger
        Hand[] finalHandsForLog = new Hand[nbPlayers];
        for (int i = 0; i < nbPlayers; i++)
            finalHandsForLog[i] = gameContext.getPlayer(i).getHand(); // Hands should be empty now
        logger.addEndOfGameToLog(nbPlayers, trickManager.getTrickWinningHands(), scores, winners);

        return logger.getLogResult();
    }

    /**
     * HANDLERS FOR HUMAN INTERACTION
     */

    public void handleHumanTrumpSelection(String selectedSuit) {
        gameContext.setSelectedTrumpForHuman(selectedSuit);
    }

    public void handleHumanCardSelection(Card card) {
        Hand humanHand = gameContext.getPlayer(HUMAN_PLAYER_INDEX).getHand();
        if (!TrickManager.checkValidTrick(card, humanHand.getCardList(),
                gameContext.getPlayingArea().getCardList(),
                gameContext.getTrumpSuit())) {
            pinochleUI.setStatus(
                    "Card is not valid. Player needs to choose higher card of the same suit, or trump if out of suit, or any card if out of trump too.");
            return; // Indicate to UI or HumanPlayer that selection was invalid
        }
        gameContext.setSelectedCardForHuman(card);
        pinochleUI.disableHumanHandInteraction(humanHand); // Disabling interaction might be premature if selection is
                                                           // part of a loop
    }

    /**
     * GET FUNCTIONS
     */

    // Make BidManager accessible if it needs to be called from UI actions directly
    // (not ideal)
    public BidManager getBidManager() {
        return bidManager;
    }

    public int[] getScores() {
        return scores;
    }

    public PinochleUI getPinochleUI() {
        return pinochleUI;
    }

    public MeldManager getMeldManager() {
        return meldManager;
    }

    public TrickManager getTrickManager() {
        return trickManager;
    }

    public int getUIDelayTime() {
        return delayTime;
    } // Used by HumanPlayer for polling

}