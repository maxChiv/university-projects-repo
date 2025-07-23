import ch.aplu.jcardgame.Card;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public abstract class Meld {
    public abstract int getScore();
    public abstract List<String> cardsToRemove(List<Card> list);
    public String trumpSuit = null;

    public Meld(String trumpSuit) {
        this.trumpSuit = trumpSuit;
    }

    public List<String> getTenToAceCards() {
        return Arrays.asList(
                Rank.ACE.getRankCardValue() + trumpSuit,
                Rank.JACK.getRankCardValue() + trumpSuit,
                Rank.QUEEN.getRankCardValue() + trumpSuit,
                Rank.KING.getRankCardValue() + trumpSuit,
                Rank.TEN.getRankCardValue() + trumpSuit);
    }
}

class AceRunExtraKingMeld extends Meld {
    public AceRunExtraKingMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 190;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = new ArrayList<>(getTenToAceCards());
        cardsToCheck.add(Rank.KING.getRankCardValue() + trumpSuit);
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class AceRunExtraQueenMeld extends Meld {
    public AceRunExtraQueenMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 190;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = new ArrayList<>(getTenToAceCards());
        cardsToCheck.add(Rank.QUEEN.getRankCardValue() + trumpSuit);
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class AceRunRoyalMarriageMeld extends Meld {
    public AceRunRoyalMarriageMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 230;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = new ArrayList<>(getTenToAceCards());
        cardsToCheck.add(Rank.KING.getRankCardValue() + trumpSuit);
        cardsToCheck.add(Rank.QUEEN.getRankCardValue() + trumpSuit);
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class AcesAroundMeld extends Meld {
    public AcesAroundMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 100;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = Arrays.asList(
                Rank.ACE.getRankCardValue() + Suit.SPADES.getSuitShortHand(),
                Rank.ACE.getRankCardValue() + Suit.DIAMONDS.getSuitShortHand(),
                Rank.ACE.getRankCardValue() + Suit.HEARTS.getSuitShortHand(),
                Rank.ACE.getRankCardValue() + Suit.CLUBS.getSuitShortHand());
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class CommonMarriageMeld extends Meld {
    public CommonMarriageMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 20;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<Suit> nonTrumpSuits = new ArrayList<>(
                Arrays.asList(Suit.DIAMONDS, Suit.SPADES, Suit.HEARTS, Suit.CLUBS));
        nonTrumpSuits.removeIf(suit -> suit.getSuitShortHand().equals(trumpSuit));

        Suit suit1 = nonTrumpSuits.get(0);
        List<String> cardsToCheck1 = Arrays.asList(
                Rank.KING.getRankCardValue() + suit1.getSuitShortHand(),
                Rank.QUEEN.getRankCardValue() + suit1.getSuitShortHand());

        Suit suit2 = nonTrumpSuits.get(1);
        List<String> cardsToCheck2 = Arrays.asList(
                Rank.KING.getRankCardValue() + suit2.getSuitShortHand(),
                Rank.QUEEN.getRankCardValue() + suit2.getSuitShortHand());

        Suit suit3 = nonTrumpSuits.get(2);
        List<String> cardsToCheck3 = Arrays.asList(
                Rank.KING.getRankCardValue() + suit3.getSuitShortHand(),
                Rank.QUEEN.getRankCardValue() + suit3.getSuitShortHand());

        if (CardUtil.checkCardInList(list, cardsToCheck1)) {
            return cardsToCheck1;
        }

        if (CardUtil.checkCardInList(list, cardsToCheck2)) {
            return cardsToCheck2;
        }

        if (CardUtil.checkCardInList(list, cardsToCheck3)) {
            return cardsToCheck3;
        }

        return null;
    }
}

class DixMeld extends Meld {
    public DixMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 10;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = new ArrayList<>();
        cardsToCheck.add(Rank.NINE.getRankCardValue() + trumpSuit);
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class DoublePinochleMeld extends Meld {
    public DoublePinochleMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 300;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = Arrays.asList(
                Rank.JACK.getRankCardValue() + Suit.DIAMONDS.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.DIAMONDS.getSuitShortHand(),
                Rank.QUEEN.getRankCardValue() + Suit.SPADES.getSuitShortHand(),
                Rank.QUEEN.getRankCardValue() + Suit.SPADES.getSuitShortHand());
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class DoubleRunMeld extends Meld {
    public DoubleRunMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 1500;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = new ArrayList<>(getTenToAceCards());
        cardsToCheck.addAll(getTenToAceCards());
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class JacksAroundMeld extends Meld {
    public JacksAroundMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 400;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = Arrays.asList(
                Rank.JACK.getRankCardValue() + Suit.SPADES.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.SPADES.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.DIAMONDS.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.DIAMONDS.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.HEARTS.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.HEARTS.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.CLUBS.getSuitShortHand(),
                Rank.JACK.getRankCardValue() + Suit.CLUBS.getSuitShortHand());
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class PinochleMeld extends Meld {
    public PinochleMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 40;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = Arrays.asList(
                Rank.JACK.getRankCardValue() + Suit.DIAMONDS.getSuitShortHand(),
                Rank.QUEEN.getRankCardValue() + Suit.SPADES.getSuitShortHand());
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class TenToAceMeld extends Meld {
    public TenToAceMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 150;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = getTenToAceCards();
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}

class RoyalMarriageMeld extends Meld {
    public RoyalMarriageMeld(String trumpSuit) {
        super(trumpSuit);
    }

    public int getScore() {
        return 40;
    }

    public List<String> cardsToRemove(List<Card> list) {
        List<String> cardsToCheck = Arrays.asList(
                Rank.QUEEN.getRankCardValue() + trumpSuit,
                Rank.KING.getRankCardValue() + trumpSuit);
        if (CardUtil.checkCardInList(list, cardsToCheck)) {
            return cardsToCheck;
        }
        return null;
    }
}