package ru.nsu.testova;

import ru.nsu.testova.commands.*;
import org.junit.Assert;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

public class MainTest {
    @Test
    public void testPush() {
        Context context = new Context();
        List<String> params = new ArrayList<>();
        params.add("PUSH");
        params.add("1");
        Command push = new Push();
        push.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(1, result, 0.01);
    }

    @Test
    public void testPop() {
        Context context = new Context();
        context.getStack().push(1.0);
        context.getStack().push(2.0);
        List<String> params = new ArrayList<>();
        params.add("POP");
        Command pop = new Pop();
        pop.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(1, result, 0.01);
    }

    @Test
    public void testComment() {
        Context context = new Context();
        context.getStack().push(1.0);
        List<String> params = new ArrayList<>();
        params.add("#");
        params.add("djvhngsujdwh");
        Command comment = new Comment();
        comment.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(1, result, 0.01);
    }

    @Test
    public void testDivision() {
        Context context = new Context();
        context.getStack().push(6.0);
        context.getStack().push(2.0);
        List<String> params = new ArrayList<>();
        params.add("/");
        Command division = new Division();
        division.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(3, result, 0.01);
    }

    @Test
    public void testMinus() {
        Context context = new Context();
        context.getStack().push(3.0);
        context.getStack().push(1.0);
        List<String> params = new ArrayList<>();
        params.add("-");
        Command minus = new Minus();
        minus.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(2, result, 0.01);
    }

    @Test
    public void testMultiplication() {
        Context context = new Context();
        context.getStack().push(3.0);
        context.getStack().push(2.0);
        List<String> params = new ArrayList<>();
        params.add("*");
        Command multiplication = new Multiplication();
        multiplication.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(6, result, 0.01);
    }

    @Test
    public void testPlus() {
        Context context = new Context();
        context.getStack().push(3.0);
        context.getStack().push(2.0);
        List<String> params = new ArrayList<>();
        params.add("+");
        Command plus = new Plus();
        plus.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(5, result, 0.01);
    }

    @Test
    public void testSqrt() {
        Context context = new Context();
        context.getStack().push(9.0);
        List<String> params = new ArrayList<>();
        params.add("SQRT");
        Command sqrt = new Sqrt();
        sqrt.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(3, result, 0.01);
    }

    @Test
    public void testDefine() {
        Context context = new Context();
        List<String> params = new ArrayList<>();
        params.add("DEFINE");
        params.add("a");
        params.add("3.76");
        Command define = new Define();
        define.make(params, context);
        double result = context.getVar().get("a");

        Assert.assertEquals(3.76, result, 0.01);
    }

    @Test
    public void testPrint() {
        Context context = new Context();
        context.getStack().push(9.0);
        List<String> params = new ArrayList<>();
        params.add("PRINT");
        Command print = new Print();
        print.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(9, result, 0.01);
    }

    @Test
    public void testParameters() {
        Context context = new Context();
        context.getStack().push(6.0);
        context.getVar().put("a", 3.0);
        List<String> params = new ArrayList<>();
        params.add("PUSH");
        params.add("a");
        Command push = new Push();
        push.make(params, context);

        params.clear();
        params.add("/");
        Command division = new Division();
        division.make(params, context);
        double result = context.getStack().pop();

        Assert.assertEquals(2, result, 0.01);
    }
}
