package ru.nsu.testova;

import java.util.Objects;

public class Word implements Comparable<Word>{
    StringBuilder word;
    int count;

    Word() {
        this.word = null;
        this.count = 0;
    }
    Word(StringBuilder word, int count) {
        this.word = word;
        this.count = count;
    }

    public int getCount() {
        return count;
    }

    public StringBuilder getWord() {
        return word;
    }

//    public void setCount(int count) {
//        this.count = count;
//    }

    @Override
    public int compareTo(Word other)
    {
        int countComparison = Integer.compare(this.count, other.count);
        if (countComparison == 0)
        {
            return this.word.compareTo(other.word);
        }
        return countComparison;
    }

//    @Override
//    public boolean equals(Object obj) {
//        if (obj == this) {
//            return true;
//        }
//        if (obj == null || obj.getClass() != this.getClass()) {
//            return false;
//        }
//        Word guest = (Word) obj;
//        return word == guest.word;
//    }
//
//    @Override
//    public int hashCode() {
//        return Objects.hash(word, count);
//    }
}
