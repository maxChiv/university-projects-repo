import ch.aplu.jcardgame.Card;
import ch.aplu.jcardgame.Hand;

import java.util.Arrays;
import java.util.Random;
import java.util.stream.Collectors;
import java.util.List;

public interface ComputerTrickTakingStrategy {
    /**
     * Determines which card the computer player should play in the current trick.
     */
    Card playCard(Hand computerHand, GameContext gameContext);
}

class DefaultTrickTakingStrategy implements ComputerTrickTakingStrategy {
    @Override
    public Card playCard(Hand hand, GameContext gameContext) {

        List<Card> existingCards = gameContext.getPlayingArea().getCardList();
        if (existingCards.isEmpty()) {
            int x = CardUtil.random.nextInt(hand.getCardList().size());
            return hand.getCardList().get(x);
        }

        Card existingCard = existingCards.get(0); // Assuming first card played sets the suit for trick
        // Try to win: play higher card of the same suit
        Card higherCard = TrickManager.getHigherCardFromList(existingCard, hand.getCardList());
        if (higherCard != null) {
            return higherCard;
        }

        // Try to win: play a trump card if no higher card of the same suit
        Card trumpCardToPlay = TrickManager.getTrumpCard(hand.getCardList(), gameContext.getTrumpSuit());
        if (trumpCardToPlay != null) {
            // More advanced: only play trump if it can win, or if forced to play trump.
            // For now, simple: if has trump, and can't follow suit with higher, play trump.
            return trumpCardToPlay;
        }

        // Can't win or follow with higher, play any valid card (preferably lowest of the followed suit, or discard)
        // Try to follow suit with any card
        for(Card card : hand.getCardList()){
            if(card.getSuit().equals(existingCard.getSuit())){
                return card; // Play first available card of the suit
            }
        }

        // If cannot follow suit at all, discard (play any card)
        int x = CardUtil.random.nextInt(hand.getCardList().size());
        return hand.getCardList().get(x);
    }
}

class SmartTrickTakingStrategy implements ComputerTrickTakingStrategy {
    private final RuleComponent rootRule; // The root of our Composite rule tree

    public SmartTrickTakingStrategy(RuleComponent rootRule) {
        this.rootRule = rootRule;
    }

    @Override
    public Card playCard(Hand computerHand, GameContext gameContext) {
        Card bestCard = null;
        int highestScore = Integer.MIN_VALUE;

        // 1. Identify all legal cards in hand.
        List<Card> legalCards = TrickManager.getLegalCards(
                computerHand,
                gameContext.getPlayingArea().getCardList(),
                gameContext.getTrumpSuit());

        // 2. For each legal card determine its score
        for (Card cardToEvaluate : legalCards) {
            int currentCardScore = rootRule.evaluate(cardToEvaluate,gameContext);

            // If a VETO_PLAY score is returned, this card is immediately disqualified.
            if (currentCardScore == RuleComponent.VETO_PLAY) {
                continue; // Skip this card and consider the next one
            }

            // 3. if this card is the highest scored card, it is the new card to play.
            if (currentCardScore > highestScore) {
                highestScore = currentCardScore;
                bestCard = cardToEvaluate;
            }
        }

        // 4. Fallback, return a random card.
        if (bestCard == null) {
            return legalCards.get(new Random().nextInt(legalCards.size()));
        }
        return bestCard;
    }
}

