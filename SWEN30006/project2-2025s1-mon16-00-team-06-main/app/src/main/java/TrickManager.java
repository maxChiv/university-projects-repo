import ch.aplu.jcardgame.*;
import ch.aplu.jgamegrid.*;

import java.awt.*;
import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

public class TrickManager {
    private final PinochleUI pinochleUI;

    private Hand[] trickWinningHands;

    public TrickManager(PinochleUI pinochleUI, int nbPlayers, Deck deck) {
        this.pinochleUI = pinochleUI;

        trickWinningHands = new Hand[nbPlayers];
        for (int i = 0; i < nbPlayers; i++) {
            trickWinningHands[i] = new Hand(deck);
        }

    }

    public int playSingleTrick(GameContext gameContext, int startingPlayerIndex) {
        // Easy refrencing
        Player[] players = gameContext.getPlayers();
        Hand playingArea = gameContext.getPlayingArea();

        int currentPlayerIndex = startingPlayerIndex;
        int nbPlayers = players.length;

        for (int i = 0; i < nbPlayers; i++) { // For each player in the current trick
            Player currentPlayer = players[currentPlayerIndex];
            // Player makes a move
            Card playedCard = currentPlayer.playCard();

            if (playedCard != null) {
                playedCard.removeFromHand(true);  // Game logic: remove from player's hand model & visually
                Pinochle.logger.addCardPlayedToLog(currentPlayerIndex, playedCard);
                playingArea.insert(playedCard, true);   // Game logic: add to playingArea model & visually
            } else {
                // Fallback: play last card if not null
                if (!currentPlayer.getHand().isEmpty()) {
                    playedCard = currentPlayer.getHand().getLast();
                    if (playedCard != null) {
                        playedCard.removeFromHand(true);
                        playingArea.insert(playedCard, true);
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
            // UI Update: Show the card just played in the playing area
            pinochleUI.updatePlayingAreaUI(playingArea, false, -1, nbPlayers, trickWinningHands);

            boolean isTrickComplete = playingArea.getNumberOfCards() == nbPlayers;

            if (isTrickComplete) {
                int trickWinner = checkWinner(startingPlayerIndex, playingArea, gameContext.getTrumpSuit());
                // UI Update: Trick is now complete. Playing area should appear empty or show cards moving.
                pinochleUI.updatePlayingAreaUI(playingArea, true, trickWinner, nbPlayers, trickWinningHands);
                return trickWinner;
            }

            // Round not finish so continue to next player
            currentPlayerIndex = (currentPlayerIndex + 1) % nbPlayers;
        }

        // Fallback: return original starter if something breaks
        return startingPlayerIndex;
    }

    public int updateTrickScore(int playerIdx, String trumpSuit) {
        List<Card> cardsWon = trickWinningHands[playerIdx].getCardList();
        // Refactored trick score calculation
        return getTrickScoreValue(cardsWon, trumpSuit);
    }

    // Separated method that calculates score from a list of cards
    private int getTrickScoreValue(List<Card> cards, String trumpSuit) {
        int score = 0;
        for (Card card : cards) {
            Rank rank = (Rank) card.getRank();
            Suit suit = (Suit) card.getSuit();
            boolean isNineCard = rank.getRankCardValue() == Rank.NINE.getRankCardValue();
            boolean isTrumpCard = suit.getSuitShortHand().equals(trumpSuit); // Use the actual trump suit in scope

            if (isNineCard && isTrumpCard) {
                score += Rank.NINE_TRUMP;
            } else {
                score += rank.getScoreValue();
            }
        }
        return score;
    }

    /**
     * Util Methods
     */

    // Checks if two cards are of the same suit
    private static boolean isSameSuit(Card card1, Card card2) {
        Suit card1Suit = (Suit) card1.getSuit();
        Suit card2Suit = (Suit) card2.getSuit();
        return card1Suit.getSuitShortHand().equals(card2Suit.getSuitShortHand());
    }

    // Returns true if card1 has a higher rank than card2
    private static boolean isHigherRank(Card card1, Card card2) {
        Rank card2Rank = (Rank) card2.getRank();
        Rank card1Rank = (Rank) card1.getRank();
        return card1Rank.getRankCardValue() > card2Rank.getRankCardValue();
    }

    // Returns a card from the list that is the same suit and higher than the existing card
    static Card getHigherCardFromList(Card existingCard, List<Card> cards) {
        return cards.stream().filter(playerCard -> {
            return isSameSuit(existingCard, playerCard) && isHigherRank(playerCard, existingCard);
        }).findAny().orElse(null);
    }

    // Returns the first trump card found in the player's hand
    static Card getTrumpCard(List<Card> cards, String trumpSuit) {
        return cards.stream().filter(playerCard -> {
            Suit playerCardSuit = (Suit) playerCard.getSuit();
            return playerCardSuit.getSuitShortHand().equals(trumpSuit);
        }).findAny().orElse(null);
    }

    public static List<Card> getLegalCards(Hand playerHand, List<Card> existingCards, String trumpSuit) {
        List<Card> legalMoves = new ArrayList<>();
        List<Card> playerCardsList = playerHand.getCardList(); // Get the actual list of cards

        // Rule 1: If player is leading the trick (no cards played yet), all cards are legal.
        if (existingCards.isEmpty()) {
            return new ArrayList<>(playerCardsList); // Return a copy to avoid external modification issues
        }

        // If not leading, iterate through all cards in the player's hand
        // and check each one using the provided checkValidTrick function.
        for (Card candidateCard : playerCardsList) {
            // Pass the candidate card, the player's full hand, the existing cards, and the trump suit
            if (checkValidTrick(candidateCard, playerCardsList, existingCards, trumpSuit)) {
                legalMoves.add(candidateCard);
            }
        }

        return legalMoves;
    }

    // Validates if the trick play is legal based on current state and trump rules
    public static boolean checkValidTrick(Card playingCard, List<Card> playerCards, List<Card> existingCards, String trumpSuit) {
        if (existingCards.isEmpty()) {
            return true;
        }

        Suit playingSuit = (Suit) playingCard.getSuit();
        Rank playingRank = (Rank) playingCard.getRank();
        Card existingCard = existingCards.get(0);
        Suit existingSuit = (Suit) existingCard.getSuit();
        Rank existingRank = (Rank) existingCard.getRank();

        // Same Suit, Higher Rank, then valid
        if (playingSuit.getSuitShortHand().equals(existingSuit.getSuitShortHand()) &&
                playingRank.getRankCardValue() > existingRank.getRankCardValue()) {
            return true;
        }

        // If the chosen is not the same suit, higher rank and there is one, then not valid
        Card higherCard = getHigherCardFromList(existingCard, playerCards);
        if (higherCard != null) {
            return false;
        }

        boolean isExistingTrump = existingSuit.getSuitShortHand().equals(trumpSuit);
        boolean isPlayingTrump = playingSuit.getSuitShortHand().equals(trumpSuit);
        // If the current is trump, then there is already no trump card with higher rank.
        // Otherwise, the above if should return false.
        if (isExistingTrump) {
            return true;
        }

        // If the current is not trump card, then playing trump card is valid
        if (isPlayingTrump) {
            return true;
        }

        // If the current is not trump card, and we have a trump card,
        // but not having a same suit, higher rank card, then we have to play trump card
        Card trumpCard = getTrumpCard(playerCards, trumpSuit);
        if (trumpCard != null) {
            return false;
        }

        // If we dont have a trump card, any card is valid
        return true;
    }

    // Determines the winner of the trick between two cards
    public int checkWinner(int startingPlayerIndex, Hand playingArea, String trumpSuit) {
        assert (playingArea.getCardList().size() == 2);

        Card card1 = playingArea.getCardList().get(0); // played by startingPlayer
        Card card2 = playingArea.getCardList().get(1); // played by (startingPlayer + 1) % 2

        boolean isHigherRankSameSuit = isSameSuit(card1, card2) && isHigherRank(card2, card1);
        if (isHigherRankSameSuit) {
            return (startingPlayerIndex + 1) % 2;
        }

        Suit card1Suit = (Suit) card1.getSuit();
        Suit card2Suit = (Suit) card2.getSuit();

        if (card1Suit.getSuitShortHand().equals(trumpSuit)) {
            return startingPlayerIndex;
        }

        if (card2Suit.getSuitShortHand().equals(trumpSuit)) {
            return (startingPlayerIndex + 1) % 2;
        }

        return startingPlayerIndex;
    }


    public Hand[] getTrickWinningHands() {
        return trickWinningHands;
    }

}
