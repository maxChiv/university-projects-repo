import ch.aplu.jcardgame.*;
import ch.aplu.jgamegrid.*;

import java.awt.*;
import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

public class CardUtil {

    static final Random random = new Random(Pinochle.seed);

    public static String getCardName(Card card) {
        Suit suit = (Suit) card.getSuit();
        Rank rank = (Rank) card.getRank();
        return rank.getRankCardValue() + suit.getSuitShortHand();
    }

    public static boolean checkCardInList(List<Card> cardList, List<String> cardsToCheck) {
        ArrayList<String> cardsToRemove = new ArrayList<>(cardsToCheck);
        for (Card card : cardList) {
            String cardName = getCardName(card);
            cardsToRemove.remove(cardName);
        }
        return cardsToRemove.isEmpty();
    }

    public static List<Card> removeCardFromList(List<Card> cardList, List<String> cardsToRemove) {
        List<Card> newCardList = new ArrayList<>();
        List<String> newCardsToRemove = new ArrayList<>(cardsToRemove);
        for (Card card : cardList) {
            String cardName = getCardName(card);
            if (newCardsToRemove.contains(cardName)) {
                newCardsToRemove.remove(cardName);
            } else {
                newCardList.add(card);
            }
        }
        return newCardList;
    }

    private static Rank getRankFromString(String cardName) {
        String rankString = cardName.substring(0, cardName.length() - 1);
        Integer rankValue = Integer.parseInt(rankString);

        for (Rank rank : Rank.values()) {
            if (rank.getShortHandValue() == rankValue) {
                return rank;
            }
        }

        return Rank.ACE;
    }

    private static Suit getSuitFromString(String cardName) {
        String rankString = cardName.substring(0, cardName.length() - 1);
        String suitString = cardName.substring(cardName.length() - 1, cardName.length());
        Integer rankValue = Integer.parseInt(rankString);

        for (Suit suit : Suit.values()) {
            if (suit.getSuitShortHand().equals(suitString)) {
                return suit;
            }
        }
        return Suit.CLUBS;
    }

    public static Card getCardFromList(List<Card> cards, String cardName) {
        Rank existingRank = getRankFromString(cardName);
        Suit existingSuit = getSuitFromString(cardName);
        for (Card card : cards) {
            Suit suit = (Suit) card.getSuit();
            Rank rank = (Rank) card.getRank();
            if (suit.getSuitShortHand().equals(existingSuit.getSuitShortHand())
                    && rank.getRankCardValue() == existingRank.getRankCardValue()) {
                return card;
            }
        }

        return null;
    }

    public static Card applyAutoMovement(Hand hand, String nextMovement) {
        if (hand.isEmpty()) return null;
        String[] cardStrings = nextMovement.split("-");
        String cardDealtString = cardStrings[0];
        if (nextMovement.isEmpty()) {
            return null;
        }
        Card dealt = getCardFromList(hand.getCardList(), cardDealtString);
        if (dealt == null) {
            System.err.println("cannot draw card: " + cardDealtString + " - hand: " + hand.getCardList());
        }

        return dealt;
    }

    // return random Card from ArrayList
    public static Card randomCard(ArrayList<Card> list) {
        int x = random.nextInt(list.size());
        return list.get(x);
    }

    public static int cmpCard(Card card1, Card card2, String trumpSuit) {
        Rank rank1 = (Rank) card1.getRank();
        Suit suit1 = (Suit) card1.getSuit();
        Rank rank2 = (Rank) card2.getRank();
        Suit suit2 = (Suit) card2.getSuit();

        boolean isCard1Trump = suit1.getSuitShortHand().equals(trumpSuit);
        boolean isCard2Trump = suit2.getSuitShortHand().equals(trumpSuit);

        // Prioritize Trump Cards
        if (isCard1Trump && !isCard2Trump) {
            return 1; // card1 is trump, card2 is not -> card1 is stronger
        }
        if (!isCard1Trump && isCard2Trump) {
            return -1; // card2 is trump, card1 is not -> card2 is stronger
        }

        // If both are trump OR both are non-trump and of the same suit
        if (suit1.getSuitShortHand().equals(suit2.getSuitShortHand())) {
            // Compare by rankCardValue (Pinochle's specific rank hierarchy)
            return Integer.compare(rank1.getRankCardValue(), rank2.getRankCardValue());
        }

        // If both are non-trump AND of different suits
        // In Pinochle, these cards don't inherently beat each other in a pairwise comparison.
        // They are considered of "equal strength" in this context.
        return 0;
    }
}
