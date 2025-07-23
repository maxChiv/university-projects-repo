import ch.aplu.jcardgame.*;


import java.util.*;
import java.util.List;
import java.util.stream.Collectors;


/**
 * Interface for computer bidding strategies.
 */
public interface ComputerBiddingStrategy {
    /**
     * Calculates the bid for the computer player.
     */
    public int calculateBid(Hand hand, GameContext gameContext, BidManager bidManager, MeldManager meldManager);
}

class RandomComputerBiddingStrategy implements ComputerBiddingStrategy {
    private static final Random random = new Random();

    @Override
    public int calculateBid(Hand hand, GameContext gameContext, BidManager bidManager, MeldManager meldManager) {
        // This is essentially your original random bidding logic
        int randomBidBase = random.nextInt(3); // 0, 1, or 2
        return randomBidBase * BidManager.BID_SELECTION_VALUE; // Assuming BID_SELECTION_VALUE is 10
    }
}

/**
 * Implements the smart bidding strategy for the computer player.
 */
class SmartComputerBiddingStrategy implements ComputerBiddingStrategy {
    private final int BID_INCREMENT_LARGE = 20;
    private final int BID_INCREMENT_SMALL = 10;

    @Override
    public int calculateBid(Hand hand, GameContext gameContext, BidManager bidManager, MeldManager meldManager) {
        // Caclulate its best meld Score
        int potentialMeldScore = caclulatePotentialMeldScore(hand, meldManager);

        // Rule 1: If the computer player is the first bidder, its opening bid will be equal to the total meld score of its hand.
        if (bidManager.isFirstBidderComputer() && bidManager.getCurrentBid() == 0) {
            return potentialMeldScore;
        }

        // Rule 2 & 3: For subsequent bids (or if the computer player is the second bidder)
        int plannedIncrease = BID_INCREMENT_SMALL;

        // Check for 6 or more cards in the same suit
        if (hasSixOrMoreOfSameSuit(hand)) {
            plannedIncrease = BID_INCREMENT_LARGE;
        }

        // Rule 4: The bid will only be made if the new bid is less than the maximum of the following two values. Otherwise, it will pass.
        int newBid = bidManager.getCurrentBid() + plannedIncrease;
        int maxAllowableBid = calculateMaxAllowableBid(hand);

        if (newBid < potentialMeldScore + maxAllowableBid) {
            return plannedIncrease;
        } else {
            return 0; // Pass
        }
    }

    // Helper method to calculate the maximum allowable bid based on hand strength
    private int calculateMaxAllowableBid(Hand hand) {
        // seperate all the suits from the hand
        Map<Suit, List<Card>> cardsBySuit = hand.getCardList().stream()
                .collect(Collectors.groupingBy(card -> (Suit) card.getSuit()));

        // Find the majority suit and then count how much the total of that suit would be
        int maxMajoritySuitValue = 0;
        Suit majoritySuit = null; // Will store the actual majority Suit
        int maxCardsInSuit = -1; // To track the highest count of cards in a suit

        // Iterate through the map entries to find the majority suit
        for (Map.Entry<Suit, List<Card>> entry : cardsBySuit.entrySet()) {
            if (entry.getValue().size() > maxCardsInSuit) {
                maxCardsInSuit = entry.getValue().size();
                majoritySuit = entry.getKey();
            }
        }

        if (majoritySuit != null) { // Sum up all the values of the max suit
            maxMajoritySuitValue = cardsBySuit.get(majoritySuit).stream()
                    .mapToInt(card -> ((Rank) card.getRank()).getScoreValue())
                    .sum();
        }

        // Calculate the total card value of the suit that contains the most Aces, 10s, and Kings.
        int maxAcesTensKingsValue = 0;
        for (Suit suit : Suit.values()) {
            // Get all the cards of the current suit from the player's hand.
            List<Card> suitCards = cardsBySuit.getOrDefault(suit, Collections.emptyList());

            int currentSuitAcesTensKingsValue = 0;

            // go through each card in THIS specific suit
            for (Card card : suitCards) {
                Rank rank = (Rank) card.getRank(); // Get the rank of the card (e.g., ACE, TEN, NINE)

                // Check if the card is an Ace, Ten, or King
                if (rank == Rank.ACE || rank == Rank.TEN || rank == Rank.KING) {
                    // If it is, add its score value to our total for this suit
                    currentSuitAcesTensKingsValue += rank.getScoreValue();
                }
            }

            // After checking all cards in the current suit, see if itsvalue
            // is higher than any other suit we've seen so far.
            if (currentSuitAcesTensKingsValue > maxAcesTensKingsValue) {
                maxAcesTensKingsValue = currentSuitAcesTensKingsValue;
            }
        }

        return Math.max(maxMajoritySuitValue, maxAcesTensKingsValue);
    }

    public int caclulatePotentialMeldScore(Hand hand, MeldManager meldManager) {
        int bestOverallMeldScore = 0; // Start with 0, as scores are non-negative

        // Go through each of the four possible suits
        for (Suit suit : Suit.values()) {
            // Calculate the meld score for the hand, pretending 'suit' is the trump.
            int currentSuitMeldScore = meldManager.calculateMeldingScore(hand.getCardList(), suit.getSuitShortHand());

            // If the score for this suit as trump is better than what  found so faupdate our best score.
            if (currentSuitMeldScore > bestOverallMeldScore) {
                bestOverallMeldScore = currentSuitMeldScore;
            }
        }
        return bestOverallMeldScore;
    }

    public boolean hasSixOrMoreOfSameSuit(Hand hand) {
        // Step 1: Count how many cards you have for each suit.
        Map<String, Integer> suitCounts = new HashMap<>();

        // Go through every card in your hand
        for (Card card : hand.getCardList()) {
            Suit suit = (Suit) card.getSuit(); // Get the suit of the current card

            // Increase the count for this suit.
            // If the suit isn't in the map yet, it starts at 0 before adding 1.
            suitCounts.put(suit.getSuitShortHand(), suitCounts.getOrDefault(suit.getSuitShortHand(), 0) + 1);
        }

        // Step 2: Now, check if any suit has 6 or more cards.
        for (Integer count : suitCounts.values()) {
            if (count >= 6) {
                return true;
            }
        }

        return false;
    }
}