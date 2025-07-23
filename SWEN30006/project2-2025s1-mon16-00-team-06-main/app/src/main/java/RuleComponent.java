import ch.aplu.jcardgame.Card;
import ch.aplu.jcardgame.Hand;

import java.util.ArrayList;
import java.util.List;

/**
 * Defines the common interface for rules within the computer player's trick-taking strategy.
 */
public interface RuleComponent {

    int NEUTRAL = 0;
    int LOW_POSITIVE = 10;
    int MEDIUM_POSITIVE = 50;
    int HIGH_POSITIVE = 100;

    int BONUS_WIN_TRICK_BASE = 100;
    int BONUS_FORCED_PLAY = 500;

    int LOW_NEGATIVE = -10;
    int MEDIUM_NEGATIVE = -50;
    int HIGH_NEGATIVE = -100;
    int VERY_HIGH_NEGATIVE = -500;

    int VETO_PLAY = -999999;

    int evaluate(Card evaluateCard, GameContext gameContext);
}

/* Composite rule base class with addChild */
abstract class CompositeRule implements RuleComponent {
    protected List<RuleComponent> children = new ArrayList<>();

    public void addChild(RuleComponent child) {
        children.add(child);
    }
}

/**
 * RootRule (Composite - Sum of children)
 */
class RootRule extends CompositeRule {
    /** Consructs the following Tree by default
     * RootRule (Composite - Sum of children)
     *   PrioritiseWinningRule (Simple)
     *   AvoidLosingValuableCardsRule (Composite - Sum of children)
     *     SaveHighCardRule (Simple - Negative score)
     *     AvoidLeadingIntoStrongTrump (Simple - Negative score based on GameHistory)
     *   StrategicPlayRule (Composite - Weighted Sum)
     *     LeadTrumpIfStrongRule (Simple - Positive weight)
     *     ExploitOpponentVoidRule (Simple - High positive weight)
     *     PlayLowInWeakSuitRule (Simple - Medium positive weight)
     */
    public RootRule() {

        StrategicPlayRule strategicPlayRule = new StrategicPlayRule();
        strategicPlayRule.addChild(new PlayLowInWeakSuitRule());
        strategicPlayRule.addChild(new ExploitOpponentVoidRule());
        strategicPlayRule.addChild(new LeadTrumpIfStrongRule());
        this.addChild(strategicPlayRule);

        AvoidLosingValuableCardsRule avoidLosingValuableCardsRule = new AvoidLosingValuableCardsRule();
        avoidLosingValuableCardsRule.addChild(new SaveHighCardRule());
        avoidLosingValuableCardsRule.addChild(new AvoidLeadingIntoStrongSuit());
        this.addChild(avoidLosingValuableCardsRule);

        this.addChild(new PrioritiseWinningRule());
    }

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        int totalScore = 0;
        for (RuleComponent rule : children) {
            totalScore += rule.evaluate(evaluateCard, gameContext);
        }
        return totalScore;
    }
}

/**
 * AvoidLosingValuableCardsRule (Composite - Sum of children)
 * This rule combines negative scores from its children to discourage losing valuable cards.
 */
class AvoidLosingValuableCardsRule extends CompositeRule {

    public AvoidLosingValuableCardsRule() {
        // Empty constructor for adding children later
    }

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        int totalPenalty = 0;
        for (RuleComponent rule : children) {
            totalPenalty += rule.evaluate(evaluateCard, gameContext);
        }
        return totalPenalty;
    }
}

/**
 * StrategicPlayRule (Composite - Weighted Sum)
 * This rule combines scores from various strategic plays.
 */
class StrategicPlayRule extends CompositeRule {

    public StrategicPlayRule() {
        // Empty constructor for adding children later
    }

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        int totalScore = 0;
        for (RuleComponent rule : children) {
            totalScore += rule.evaluate(evaluateCard, gameContext);
        }
        return totalScore;
    }
}

/**
 * PrioritiseWinningRule (Simple)
 * This rule focuses on evaluating a card's ability to win the current trick.
 */

class PrioritiseWinningRule implements RuleComponent {
    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        String trumpSuit = gameContext.getTrumpSuit();

        // If the player is leading the trick, any card they play will be the "winning" card initially.
        if (gameContext.getPlayingArea().isEmpty()) {
            // For this card, is there a card that can beat it in the players hand.
            for (Card card : gameContext.getPlayerHand(Pinochle.HUMAN_PLAYER_INDEX).getCardList()) {
                if (CardUtil.cmpCard(evaluateCard, card, trumpSuit) == -1) {
                    // There's a card that can beat evaluateCard, so no winning bonus.
                    return NEUTRAL;
                }
            }
            // No cards that beat evaluateCard
            return BONUS_WIN_TRICK_BASE;
        }

        // If not leading, we need to compare evaluateCard against the current highest card on the table.
        Card currentHighestCardOnTable = gameContext.getPlayingArea().get(0);

        // cmpCard returns 1 if evaluateCard is stronger.
        if (CardUtil.cmpCard(evaluateCard, currentHighestCardOnTable, trumpSuit) == 1) {
            return BONUS_WIN_TRICK_BASE; // High positive weight for winning the trick
        }

        // If the card doesn't win the trick, this rule doesn't apply a score.
        // Other rules will handle penalties for wasted cards etc.
        return NEUTRAL;
    }
}

/**
 * SaveHighCardRule (Simple - Very High Negative score)
 * This rule discourages playing high-value cards if they are not needed to win or avoid a penalty.
 * This makes sure there are more ideal cards to play to apply such a high penalty.
 */
class SaveHighCardRule implements RuleComponent {
    private static final int HIGH_CARD_RANK_VALUE_THRESHOLD = 3;
    private static final int WASTE_PENALTY = VERY_HIGH_NEGATIVE;

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        if (((Rank) evaluateCard.getRank()).getRankCardValue() <= HIGH_CARD_RANK_VALUE_THRESHOLD) {
            return NEUTRAL;
        }

        // This rule generally doesn't discourage leading with a high card.
        if (gameContext.getPlayingArea().isEmpty()) {
            return NEUTRAL;
        }

        Card currentHighestCardOnTable = gameContext.getPlayingArea().get(0);

        // Determine if 'evaluateCard' would win the current trick.
        boolean isWinningTrick = CardUtil.cmpCard(evaluateCard, currentHighestCardOnTable, gameContext.getTrumpSuit()) == 1;

        // Check if there are other lower-ranked cards of the same suit.
        long lowerLegalSameSuitCardsCount = 0;
        for (Card card : gameContext.getPlayerHand(Pinochle.COMPUTER_PLAYER_INDEX).getCardList()) {
            if (card.equals(evaluateCard)) {
                continue; // Skip the card we're currently evaluating
            }

            Suit legalCardSuit = (Suit) card.getSuit();
            if (legalCardSuit.equals((Suit) evaluateCard.getSuit())) { // Same suit
                if (CardUtil.cmpCard(card, currentHighestCardOnTable, gameContext.getTrumpSuit()) == -1) {
                    // Card would lose so it cannot be played.
                    continue;
                }
                if (CardUtil.cmpCard(card, evaluateCard, gameContext.getTrumpSuit()) == -1) { // legalCard is weaker than evaluateCard
                    lowerLegalSameSuitCardsCount++;
                }
            }
        }
        // If 'evaluateCard' is a high card, it's not winning, AND we have a cheaper alternative of the same suit.
        if (!isWinningTrick && lowerLegalSameSuitCardsCount > 0) {
            return WASTE_PENALTY;
        }
        return NEUTRAL;
    }
}

/**
 * AvoidLeadingIntoStrongSuit (Simple - Negative score)
 * This rule discourages leading with a suit that opponents might have strong suited cards in.
 */
class AvoidLeadingIntoStrongSuit implements RuleComponent {
    private final int OPPONENT_HIGH_RANK_THRESHOLD = 10;
    private final int LEAD_PENALTY = MEDIUM_NEGATIVE;

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        // This rule only applies when the current player is leading the trick.
        if (!gameContext.getPlayingArea().isEmpty()) {
            return NEUTRAL;
        }

        for (Card card : gameContext.getPlayerHand(Pinochle.HUMAN_PLAYER_INDEX).getCardList()) {
            if (((Suit)card.getSuit()).equals((Suit)evaluateCard.getSuit()) &&
                    ((Rank)card.getRank()).getRankCardValue() >= OPPONENT_HIGH_RANK_THRESHOLD) {
                return LEAD_PENALTY;
            }
        }

        return NEUTRAL; // No opponent is deemed strong in this non-trump suit, or rule conditions not met.
    }
}

/**
 * LeadTrumpIfStrongRule (Simple - Positive Medium weight)
 * This rule encourages leading with trump if the hand has a strong trump suit.
 */
class LeadTrumpIfStrongRule implements RuleComponent {
    // Thresholds to define what's considered a "strong" trump suit.
    private static final int STRONG_TRUMP_COUNT_THRESHOLD = 4; // e.g., 5 or more trump cards
    private static final int HIGH_TRUMP_SCORE_THRESHOLD = 30; // e.g., combined score of Aces/Tens/Kings in trump suit

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        // This rule only applies if the player is leading the trick.
        if (!gameContext.getPlayingArea().isEmpty()) { // If playingArea is not empty, player is not leading
            return NEUTRAL;
        }


        // Check if the card being evaluated is a trump card.
        Suit evaluatedCardSuit = (Suit) evaluateCard.getSuit();
        boolean isEvaluateCardTrump = evaluatedCardSuit.getSuitShortHand().equals(gameContext.getTrumpSuit());

        // This rule only applies if the card being considered to lead is a trump card.
        if (!isEvaluateCardTrump) {
            return NEUTRAL;
        }

        // Now, assess if the player's overall trump suit is "strong."
        Hand playerHand = gameContext.getPlayerHand(Pinochle.COMPUTER_PLAYER_INDEX); // Get current player's hand

        // Count trump cards in hand
        long trumpCardCount = 0;
        for (Card card : playerHand.getCardList()) {
            Suit cardSuit = (Suit) card.getSuit();
            if (cardSuit.getSuitShortHand().equals(gameContext.getTrumpSuit())) {
                trumpCardCount++;
            }
        }

        // Calculate score of high trump cards (Aces, Tens, Kings) in hand
        int highTrumpScore = 0;
        for (Card card : playerHand.getCardList()) {
            Suit cardSuit = (Suit) card.getSuit();
            // Check if the card is a trump card
            if (cardSuit.getSuitShortHand().equals(gameContext.getTrumpSuit())) {
                Rank rank = (Rank) card.getRank();
                // Check if the trump card is an Ace, Ten, or King
                if (rank == Rank.ACE || rank == Rank.TEN || rank == Rank.KING) {
                    highTrumpScore += rank.getScoreValue();
                }
            }
        }

        // If the hand has a strong count of trumps AND a good score from high trumps
        if (trumpCardCount >= STRONG_TRUMP_COUNT_THRESHOLD && highTrumpScore >= HIGH_TRUMP_SCORE_THRESHOLD) {
            return MEDIUM_POSITIVE; // Encourage leading with this trump
        }

        return NEUTRAL;
    }
}

/**
 * ExploitOpponentVoidRule (Simple - High positive weight)
 * This rule highly encourages playing a suit that an opponent is known to be void in,
 * especially if that opponent still has valuable cards.
 */
class ExploitOpponentVoidRule implements RuleComponent {
    private static final int VOID_EXPLOIT_BONUS = HIGH_POSITIVE;

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        // This rule only applies if the current player is leading the trick.
        if (!gameContext.getPlayingArea().isEmpty()) {
            return NEUTRAL;
        }

        Suit cardSuit = (Suit) evaluateCard.getSuit();

        if (playerDoesNotHaveSuit(
                gameContext.getPlayerHand(Pinochle.HUMAN_PLAYER_INDEX),
                (Suit)evaluateCard.getSuit())
        ){
            return VOID_EXPLOIT_BONUS;
        }
        return NEUTRAL;
    }

    private boolean playerDoesNotHaveSuit(Hand hand, Suit suitToCheck) {
        for (Card card : hand.getCardList()) {
            Suit cardSuit = (Suit) card.getSuit();
            if (cardSuit.equals(suitToCheck)) {
                return false; // Found at least one card of the suit, so player HAS the suit.
            }
        }
        return true; // Iterated through all cards, and none matched the suit.
    }
}

/**
 * PlayLowInWeakSuitRule (Simple - Medium positive weight)
 * This rule encourages playing a low card in a suit where the player has few cards
 * and doesn't expect to win, aiming to save higher cards for other purposes.
 */
class PlayLowInWeakSuitRule implements RuleComponent {
    private static final int WEAK_SUIT_CARD_COUNT_THRESHOLD = 2;
    private static final int LOW_CARD_SCORE_THRESHOLD = 5;

    @Override
    public int evaluate(Card evaluateCard, GameContext gameContext) {
        if (gameContext.getPlayingArea().isEmpty()) {
            return NEUTRAL; // This rule doesn't apply when leading
        }

        Card currentHighestCardOnTable = gameContext.getPlayingArea().get(0);
        String trumpSuit = gameContext.getTrumpSuit();

        // Check if `evaluateCard` is unlikely to win the trick
        boolean likelyLosingTrick = false;
        if (currentHighestCardOnTable != null) {

            if (CardUtil.cmpCard(evaluateCard, currentHighestCardOnTable, gameContext.getTrumpSuit()) != 1) {
                likelyLosingTrick = true;
            }
        } else {
            return NEUTRAL; // Should not happen if !isPlayerLeading, but safety.
        }

        boolean isLowCard = ((Rank) evaluateCard.getRank()).getScoreValue() <= LOW_CARD_SCORE_THRESHOLD;

        Hand playerCurrentHand = gameContext.getPlayerHand(Pinochle.HUMAN_PLAYER_INDEX);
        Suit cardSuit = (Suit) evaluateCard.getSuit();

        long cardsInEvaluatedSuit = playerCurrentHand.getCardList().stream()
                .filter(card -> (card.getSuit()).equals(cardSuit))
                .count();

        boolean isWeakSuit = cardsInEvaluatedSuit <= WEAK_SUIT_CARD_COUNT_THRESHOLD;

        if (likelyLosingTrick && isLowCard && isWeakSuit) {
            return MEDIUM_POSITIVE;
        }

        return NEUTRAL;
    }
}
