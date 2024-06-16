package ru.nsu.testova.lab4;

import threadpool.MThreadPool;

import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.*;

public class MainTest {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        BlockingQueue<Runnable> queue = new ArrayBlockingQueue<>(100);
        MThreadPool pool = new MThreadPool(10, queue);

//        Future<Integer> f1 = pool.submit(()-> {
//            try {
//                Thread.sleep(1000);
//            } catch (InterruptedException e) {
//                throw new RuntimeException(e);
//            }
//            return 5;
//        });
//        Future<Integer> f2 = pool.submit(()-> {
//            try {
//                Thread.sleep(2000);
//            } catch (InterruptedException e) {
//                throw new RuntimeException(e);
//            }
//            return 7;
//        });

        Collection<Callable<Integer>> collection = new ArrayList<>();
        collection.add(()-> {
            try {
                Thread.sleep(7000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            return 7;
        });
        collection.add(()-> {
            try {
                Thread.sleep(10000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            return 10;
        });
        collection.add(()-> {
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            return 5;
        });

//        System.out.println(pool.awaitTermination(20000, TimeUnit.MILLISECONDS));

        System.out.println(pool.invokeAny(collection));
        System.out.println(pool.invokeAll(collection, 30000, TimeUnit.MILLISECONDS));


    }
}
