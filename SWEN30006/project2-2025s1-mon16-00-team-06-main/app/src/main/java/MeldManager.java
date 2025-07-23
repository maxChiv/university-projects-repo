import ch.aplu.jcardgame.Card;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MeldManager {

    boolean isAdditionalMelds;

    public MeldManager(boolean isAdditionalMelds) {
        this.isAdditionalMelds = isAdditionalMelds;
    }

    public int calculateMeldingScore(List<Card> list, String trumpSuit) {
        ArrayList<Meld> allMelds;
        if (isAdditionalMelds) {
            allMelds = allMeldsList(trumpSuit);
        } else {
            allMelds = originalMeldsList(trumpSuit);
        }


        int score = 0;
        for (Meld m : allMelds) {
            List<String> cardsToRemove = m.cardsToRemove(list);
            if (cardsToRemove != null) {
                score += m.getScore();
                list = CardUtil.removeCardFromList(list, cardsToRemove);
            }
        }

        return score;
    }

    public static ArrayList<Meld> originalMeldsList(String trumpSuit) {
        ArrayList<Meld> allMelds = new ArrayList<>(Arrays.asList(
                new AceRunExtraKingMeld(trumpSuit),
                new AceRunExtraQueenMeld(trumpSuit),
                new TenToAceMeld(trumpSuit),
                new RoyalMarriageMeld(trumpSuit)
        ));
        return allMelds;
    }

    public static ArrayList<Meld> allMeldsList(String trumpSuit) {
        ArrayList<Meld> allMelds = new ArrayList<>(Arrays.asList(
                new DoubleRunMeld(trumpSuit),
                new JacksAroundMeld(trumpSuit),
                new DoublePinochleMeld(trumpSuit),
                new AceRunRoyalMarriageMeld(trumpSuit),
                new AceRunExtraKingMeld(trumpSuit),
                new AceRunExtraQueenMeld(trumpSuit),
                new TenToAceMeld(trumpSuit),
                new AcesAroundMeld(trumpSuit),
                new RoyalMarriageMeld(trumpSuit),
                new PinochleMeld(trumpSuit),
                new CommonMarriageMeld(trumpSuit),
                new DixMeld(trumpSuit)
        ));
        return allMelds;
    }


}