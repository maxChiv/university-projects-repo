import ch.aplu.jcardgame.*;

import java.util.*;

import static ch.aplu.jgamegrid.GameGrid.delay;

public abstract class GameMode {
    Deck deck;
    GameContext gameContext;

    public GameMode(Deck deck, GameContext gameContext) {
        this.deck = deck;
        this.gameContext = gameContext;
    }

    // All we know for a game mode is that it must deal cards to start, and play a
    // game, other choices like how
    // Pinochle is played can be handled by the managers.
    abstract void dealingOut(Player[] playersToDeal, int numPlayers, int numCardsPerPlayer);

    abstract void playGame();
}

class StandardMode extends GameMode {
    protected Hand pack;
    protected final BidManager bidManager;
    protected final TrickManager trickManager;
    protected final MeldManager meldManager;
    protected final PinochleUI pinochleUI;
    protected final int nbPlayers;
    protected int[] scores;
    protected int delayTime;

    public StandardMode(Deck deck, GameContext gameContext, Pinochle pinochle, PinochleUI pinochleUI) {
        super(deck, gameContext);
        this.bidManager = pinochle.getBidManager();
        this.trickManager = pinochle.getTrickManager();
        this.meldManager = pinochle.getMeldManager();
        this.pinochleUI = pinochleUI;
        this.nbPlayers = gameContext.getPlayers().length;
        this.scores = pinochle.getScores();
        this.delayTime = pinochle.getUIDelayTime();
    }

    void handleBidPhase() {
        // Phase 1: Bidding
        bidManager.askForBid(gameContext, meldManager);
        askForTrumpCard(); // Uses Player interface to choose trump
    }

    void handleMeldPhase() {
        // Easy Refrencing
        Player[] players = gameContext.getPlayers();

        for (int i = 0; i < nbPlayers; i++) {
            scores[i] = meldManager.calculateMeldingScore(players[i].getHand().getCardList(), gameContext.getTrumpSuit());
            pinochleUI.updateScore(i, scores); // Update UI with meld scores
            delay(delayTime);
        }
        Pinochle.logger.addTrumpInfoToLog(gameContext.getTrumpSuit(), scores); // scores here are meld scores
    }

    void handleTrickPhase() {
        // Easy Refrencing
        Player[] players = gameContext.getPlayers();

        int nextPlayer = bidManager.getBidWinPlayerIndex(); // Winner of bid leads the first trick
        int numberOfTricks = players[Pinochle.COMPUTER_PLAYER_INDEX].getHand().getNumberOfCards();
        // All players start with same no. of cards

        // Log each players hand
        Hand[] currentHandsForLog = new Hand[nbPlayers];
        for (int k = 0; k < nbPlayers; k++)
            currentHandsForLog[k] = players[k].getHand();
        Pinochle.logger.addPlayerCardsToLog(nbPlayers, currentHandsForLog);

        for (int i = 0; i < numberOfTricks; i++) { // For each trick
            Pinochle.logger.addRoundInfoToLog(i);
            // Play a round
            nextPlayer = trickManager.playSingleTrick(gameContext, nextPlayer);
        } // End of all tricks (numberOfTricks loop)

        for (int j = 0; j < nbPlayers; j++) { // For each player in the current trick
            scores[j] += trickManager.updateTrickScore(j, gameContext.getTrumpSuit()); // Calculate total trick scores
            // for each player from their trickWinningHands and add to main scores.
        }

        validateFinalScoreAgainstBid(); // Check if the players scores should be 0
    }

    @Override
    void dealingOut(Player[] playersToDeal, int numPlayers, int numCardsPerPlayer) {
        pack = deck.toHand(false);

        for (int i = 0; i < numPlayers; i++) {
            String initialCardsKey = "players." + i + ".initialcards";
            String initialCardsValue = gameContext.getProperties().getProperty(initialCardsKey);
            if (initialCardsValue != null) {
                String[] initialCards = initialCardsValue.split(",");
                for (String initialCard : initialCards) {
                    if (initialCard.length() <= 1)
                        continue;
                    Card card = CardUtil.getCardFromList(pack.getCardList(), initialCard);
                    if (card != null) {
                        card.removeFromHand(false);
                        playersToDeal[i].getHand().insert(card, false);
                    }
                }
            }
        }

        for (int i = 0; i < numPlayers; i++) {
            int cardsToDealt = numCardsPerPlayer - playersToDeal[i].getHand().getNumberOfCards();
            for (int j = 0; j < cardsToDealt; j++) {
                if (pack.isEmpty())
                    return;
                Card dealt = CardUtil.randomCard(pack.getCardList());
                dealt.removeFromHand(false);
                playersToDeal[i].getHand().insert(dealt, false);
            }
        }
    }

    private void askForTrumpCard() {
        Player[] players = gameContext.getPlayers();

        gameContext.setTrumpSuit(null); // Clear before asking
        int bidWinnerIdx = bidManager.getBidWinPlayerIndex(); // Assuming BidManager provides this

        // Delegate to the winning player
        pinochleUI.playerChooseTrumpTextDisplay(players[bidWinnerIdx]);
        gameContext.setTrumpSuit(players[bidWinnerIdx].chooseTrump());

        // Update UI once chosen
        if (gameContext.getTrumpSuit() != null) {
            pinochleUI.updateTrumpActorUI(gameContext.getTrumpSuit());
        } else {
            // Handle error or default if player returns null (should not happen with
            // current impl)
            gameContext.setTrumpSuit("S"); // Default to Spades on error
            pinochleUI.updateTrumpActorUI(gameContext.getTrumpSuit());
        }
    }

    // To be called after validating each players tricks.
    private void validateFinalScoreAgainstBid() {
        for (int i = 0; i < nbPlayers; i++) {
            // Immediate check after updating score, as in the original version
            if (i == bidManager.getBidWinPlayerIndex()) {
                if (scores[i] < bidManager.getCurrentBid()) {
                    scores[i] = 0;
                }
            }
        }
    }

    @Override
    void playGame() {
        // Phase 1: Bidding
        handleBidPhase();

        // Phase 2: Melding
        handleMeldPhase();

        // Phase 3: Trick Taking
        handleTrickPhase();
    }

}

class CutthroatMode extends StandardMode {
    protected Player[] playersToDeal;

    // Cut throat gamemode is similar to standard mode except an added phase

    public CutthroatMode(Deck deck, GameContext gameContext, Pinochle pinochle, PinochleUI pinochleUI) {
        super(deck, gameContext, pinochle, pinochleUI);
        playersToDeal = gameContext.getPlayers();
    }

    @Override
    void playGame() {
        // Phase 1: Bidding
        handleBidPhase();

        // Phase 2: 24 to 12 Card Selection
        handlePostBidPhase();

        // Phase 3: Melding
        handleMeldPhase();

        // Phase 4: Trick Taking
        handleTrickPhase();
    }

    int getCardScore(Card c) {
        int score = 0;

        if (c.getSuit().toString().equals(gameContext.getTrumpSuit())) {
            score += 100;
        }

        score += c.getValue();

        return score;
    }

    Card getBestCardForComputer(Card card1, Card card2) {
        return getCardScore(card1) > getCardScore(card2) ? card1 : card2;
    }

    Card[] chooseTwelveBestCards(ArrayList<Card> cards) {
        Card[] cardsToReturn = new Card[12];
        HashMap<Card, Integer> map = new HashMap<>();
        for (Card c : cards) {
            map.put(c, getCardScore(c));
        }

        List<Map.Entry<Card, Integer>> entryList = new ArrayList<>(map.entrySet());

        entryList.sort(new Comparator<Map.Entry<Card, Integer>>() {
            @Override
            public int compare(Map.Entry<Card, Integer> e1, Map.Entry<Card, Integer> e2) {
                return e2.getValue() - e1.getValue(); // descending
            }
        });

        int count = 0;
        for (Map.Entry<Card, Integer> entry : entryList) {
            if (count >= 12) break;
            cardsToReturn[count] = entry.getKey();
            count++;
        }

        return cardsToReturn;
    }

    void handlePostBidPhase() {
        // IMPLEMENT LOGIC HERE AS IT WILL THEN BE CALLED IN THE ABOVE PLAY GAME AND IF
        // EVERYTHING IS CORRECT,
        // THE PLAYERS HAVE 12 CARDS AND THEREFORE THE HANDLEMELDPHASE CAN CONTINUE AS
        // THE SAME FROM THE STANDARD GAME MODE

        // 1. Get the stockpile, it should have 24 cards, and get the first 2 Cards in that pile

        if (gameContext.isAuto()) {
            dealingOutExtra(playersToDeal, playersToDeal.length, 12); // Should be

        } else {
            ArrayList<Card> first2Cards = new ArrayList<>();
            Card firstCard = pack.getFirst();
            first2Cards.add(firstCard);
            pack.remove(0, false);
            Card secondCard = pack.getFirst();
            first2Cards.add(secondCard);
            pack.remove(0, false);

            // 2. Get the winner of the bid (remember that computer index is hardcoded at 0 and human player is 1)
            int bidWinner = bidManager.getBidWinPlayerIndex();
            boolean didHumanWinBid = bidWinner == Pinochle.HUMAN_PLAYER_INDEX;


            // 2.1 If the human won the bid:

            if (didHumanWinBid) {
                // - Show the 2 cards on the screen & Show the message prompting them to take a card
                Card humanChosenCard = CardSelectionWindow.selectCards(first2Cards, 1).get(0);
                Card computerRemainingCard = firstCard.toString().equals((humanChosenCard.toString())) ? secondCard : firstCard;

                // - Add the other card to the computers hand
                playersToDeal[Pinochle.COMPUTER_PLAYER_INDEX].getHand().insert(computerRemainingCard, false);

                // - Add the other card to the human players hand
                playersToDeal[Pinochle.HUMAN_PLAYER_INDEX].getHand().insert(humanChosenCard, false);

            }

            // 2.2 If the computer won the bid:
            else {
                // - Define which card is the best in a function called getBestCard(card1, card2) -> Card
                Card computerChosenCard = getBestCardForComputer(firstCard, secondCard);
                Card humanRemainingCard = firstCard.toString().equals((computerChosenCard.toString())) ? secondCard : firstCard;

                // - Add that card to the computers hand (should have 13 cards now)
                playersToDeal[Pinochle.COMPUTER_PLAYER_INDEX].getHand().insert(computerChosenCard, false);

                // - Add the other card to the human players hand
                playersToDeal[Pinochle.HUMAN_PLAYER_INDEX].getHand().insert(humanRemainingCard, false);

            }

            // 3 There should be 22 cards left in the stockpile, give 11 to each player alternating
            for (int i = 0; i < pack.getCardList().size(); i++) {
                int nonBidWinner = (bidWinner + 1) % 2;
                if (i % 2 == 0) playersToDeal[bidWinner].getHand().insert(pack.getCardList().get(i), false);
                else playersToDeal[nonBidWinner].getHand().insert(pack.getCardList().get(i), false);
            }
        }

        // 4 Each player should now have 24 cards and the stockpile should be empty
        // - Show a message prompting the human player to choose 12 cards as well as a counter x/12

        // - Allow the human player to click/unClick on cards to add/remove them from their final hand
        ArrayList<Card> humanChosenCards;

        // - Define a function for the computer to pick the 12 best cards and set those to be their hand
        ArrayList<Card> computerCurrentHand = playersToDeal[Pinochle.COMPUTER_PLAYER_INDEX].getHand().getCardList();
        Card[] computerChosenCards = chooseTwelveBestCards(computerCurrentHand);

        if (gameContext.isAuto()) {
            humanChosenCards = getPlayer1FinalCards(gameContext);
        } else {
            humanChosenCards = CardSelectionWindow.selectCards(playersToDeal[Pinochle.HUMAN_PLAYER_INDEX].getHand().getCardList(), 12);
        }

        pack.getCardList().clear();

        Card[] humanChosenCardsArray = humanChosenCards.toArray(new Card[humanChosenCards.size()]);  // Have to convert type
        pinochleUI.setNewPlayerHandsUI(playersToDeal, computerChosenCards, humanChosenCardsArray);

    }

    void dealingOutExtra(Player[] playersToDeal, int numPlayers, int numCardsPerPlayer) {
        pack = deck.toHand(false);

        for (int i = 0; i < numPlayers; i++) {
            String extraCardsKey = "players." + i + ".extra_cards";
            String extraCardsValue = gameContext.getProperties().getProperty(extraCardsKey);
            if (extraCardsValue != null) {
                String[] initialCards = extraCardsValue.split(",");
                for (String extraCard : initialCards) {
                    if (extraCard.length() <= 1)
                        continue;
                    Card card = CardUtil.getCardFromList(pack.getCardList(), extraCard);
                    if (card != null) {
                        card.removeFromHand(false);
                        playersToDeal[i].getHand().insert(card, false);
                    }
                }
            }
        }

        for (int i = 0; i < numPlayers; i++) {
            int cardsToDealt = numCardsPerPlayer - playersToDeal[i].getHand().getNumberOfCards();
            for (int j = 0; j < cardsToDealt; j++) {
                if (pack.isEmpty())
                    return;
                Card dealt = CardUtil.randomCard(pack.getCardList());
                dealt.removeFromHand(false);
                playersToDeal[i].getHand().insert(dealt, false);
            }
        }
    }
    ArrayList<Card> getPlayer1FinalCards(GameContext gameContext) {
        pack = deck.toHand(false);

        ArrayList<Card> player1FinalCards = new ArrayList<>();
        String finalCardsKey = "players.1.final_cards";
        String finalCardsValue = gameContext.getProperties().getProperty(finalCardsKey);

        if (finalCardsValue != null) {
            String[] cardShortHands = finalCardsValue.split(",");
            for (String cardShortHand : cardShortHands) {
                if (cardShortHand.length() <= 1) {
                    continue;
                }
                ch.aplu.jcardgame.Card card = CardUtil.getCardFromList(pack.getCardList(), cardShortHand);
                if (card != null) {
                    card.removeFromHand(false);
                    player1FinalCards.add(card);
                }
            }
        }
        return player1FinalCards;
    }
}