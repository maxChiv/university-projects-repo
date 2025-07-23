import ch.aplu.jcardgame.Card;
import ch.aplu.jcardgame.Hand;

import java.util.List;
import java.util.stream.Collectors;

public class Logger {
    private static final StringBuilder logResult = new StringBuilder(); // Stores all log entries

    public Logger() {}

    /**
     * Logging Logic
     */

    public void addCardPlayedToLog(int player, Card card) {
        logResult.append("P" + player + "-"); // Log which player played the card

        Rank cardRank = (Rank) card.getRank();
        Suit cardSuit = (Suit) card.getSuit();
        // Append card notation (rank + suit shorthand)
        logResult.append(cardRank.getCardLog() + cardSuit.getSuitShortHand());

        logResult.append(","); // Separate multiple cards by comma
    }

    public void addBidInfoToLog(int bidWinPlayerIndex, int currentBid) {
        logResult.append("Bid:" + bidWinPlayerIndex + "-" + currentBid + "\n"); // Log winning bid info
    }

    public void addTrumpInfoToLog(String trumpSuit, int[] scores) {
        logResult.append("Trump: " + trumpSuit + "\n"); // Log trump suit chosen
        logResult.append("Melding Scores: " + scores[0] + "-" + scores[1] + "\n"); // Log melding scores for each team/player
    }

    public void addRoundInfoToLog(int roundNumber) {
        logResult.append("\n"); // New line before new round info
        logResult.append("Round" + roundNumber + ":"); // Log round number
    }

    public void addPlayerCardsToLog(int nbPlayers, Hand[] hands) {
        logResult.append("Initial Cards:"); // Log initial dealt cards for each player
        for (int i = 0; i < nbPlayers; i++) {
            logResult.append("P" + i + "-");
            logResult.append(convertCardListoString(hands[i]));
        }
    }

    private String convertCardListoString(Hand hand) {
        StringBuilder sb = new StringBuilder();
        // Convert list of cards to a string like "10H,JS,QD" etc.
        sb.append(hand.getCardList().stream().map(card -> {
            Rank rank = (Rank) card.getRank();
            Suit suit = (Suit) card.getSuit();
            return rank.getCardLog() + suit.getSuitShortHand();
        }).collect(Collectors.joining(",")));
        sb.append("-"); // Delimiter after card list
        return sb.toString();
    }

    public void addEndOfGameToLog(int nbPlayers, Hand[] trickWinningHands, int[] scores, List<Integer> winners) {
        logResult.append("\n");
        logResult.append("Trick Winning: "); // Log trick-winning cards for each player
        for (int i = 0; i < nbPlayers; i++) {
            logResult.append("P" + i + ":");
            logResult.append(convertCardListoString(trickWinningHands[i]));
        }
        logResult.append("\n");
        logResult.append("Final Score: "); // Log final scores
        for (int i = 0; i < scores.length; i++) {
            logResult.append(scores[i] + ",");
        }
        logResult.append("\n");
        // Log winner player indices as comma-separated string
        logResult.append("Winners: " + String.join(", ", winners.stream().map(String::valueOf).collect(Collectors.toList())));
    }

    public String getLogResult() {
        return logResult.toString(); // Return the full accumulated log string
    }
}
