package threadpool;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.concurrent.*;

public class MThreadPool implements ExecutorService {
    private int maxThreadCount;
    private int timeSavingThread;
    private BlockingQueue<Runnable> queue;
    private volatile boolean isRunning;
    private final List<Thread> threads;
    private final Object lock = new Object();
    public MThreadPool (int maxThreadCount, BlockingQueue<Runnable> queue) {
        this.maxThreadCount = maxThreadCount;
        this.queue = queue;
        timeSavingThread = 10000;
        isRunning = true;
        threads = new ArrayList<>();
    }
    @Override
    public void shutdown() {
        isRunning = false;
    }

    @Override
    public List<Runnable> shutdownNow() {
        synchronized (threads) {
            isRunning = false;
            for (Thread w : threads) {
                w.interrupt();
            }
            threads.clear();
        }
        List<Runnable> tasks = new ArrayList<>();
        while (!queue.isEmpty()) {
            try {
                tasks.add(queue.take());
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
        return tasks;
    }

    @Override
    public boolean isShutdown() {
        return !isRunning;
    }

    @Override
    public boolean isTerminated() {
        return !isRunning && queue.isEmpty() && threads.isEmpty();
    }

    @Override
    public boolean awaitTermination(long l, TimeUnit timeUnit) {
        if (timeUnit != TimeUnit.MILLISECONDS) {
            l = timeUnit.toMillis(l);
        }
        long startTime = System.currentTimeMillis();
        while (!threads.isEmpty() && (System.currentTimeMillis() - startTime) < l) {
            waitTermination(l - (System.currentTimeMillis() - startTime));
        }
        return threads.isEmpty();
    }

    private void waitTermination(long l) {
        synchronized (lock) {
            try {
                lock.wait(l);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void waitTermination() {
        synchronized (lock) {
            try {
                lock.wait();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }
    private void notifyAwait() {
        synchronized (lock) {
            lock.notifyAll();
        }
    }

    @Override
    public <T> Future<T> submit(Callable<T> callable) {
        if (callable == null) {
            throw new NullPointerException();
        }
//        if (isRunning) {
            FutureTask<T> future = new FutureTask<>(callable);
            boolean ret = queue.offer(future);
            if (ret) {
                makeThreadsIfNeeded();
                return future;
            }
            return submit(callable);
//        }
//        throw new RejectedExecutionException();
    }

    @Override
    public <T> Future<T> submit(Runnable runnable, T t) {
        if (runnable == null) {
            throw new NullPointerException();
        }
//        if (isRunning) {
            boolean ret = queue.offer(runnable);
            if (ret) {
                makeThreadsIfNeeded();
                Future<T> future = new FutureTask<>(runnable, t);
                return future;
            }
            return submit(runnable, t);
//        }
//        throw new RejectedExecutionException();
    }

    @Override
    public Future<?> submit(Runnable runnable) {
        if (runnable == null) {
            throw new NullPointerException();
        }
        Future<?> future = new FutureTask<>(runnable, null);
//        if (isRunning) {
            boolean ret = queue.offer(runnable);
            if (ret) {
                makeThreadsIfNeeded();
            }
        return future;
//            throw new RejectedExecutionException();
//            return submit(runnable);
//        }
//        throw new RejectedExecutionException();
    }

    @Override
    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> collection) throws InterruptedException {
        List<Future<T>> listFuture = new ArrayList<>();
        for (Callable<T> callable : collection) {
            if (callable == null) {
                throw new NullPointerException();
            }
            listFuture.add(submit(callable));
        }
        //while (true) {
            for (Future<T> f : listFuture) {
                try {
                    f.get();
                } catch (ExecutionException e) {
                    throw new RuntimeException();
                }
            }
        //}
        return listFuture;
    }

    @Override
    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> collection, long l, TimeUnit timeUnit) throws InterruptedException {
        List<Future<T>> listFuture = new ArrayList<>();
        for (Callable<T> callable : collection) {
            if (callable == null) {
                throw new NullPointerException();
            }
            listFuture.add(submit(callable));
        }
        if (timeUnit != TimeUnit.MILLISECONDS) {
            l = timeUnit.toMillis(l);
        }
        long startTime = System.currentTimeMillis();
        boolean flag_stop = true;
        while ((System.currentTimeMillis() - startTime) < l) {
            waitTermination(l - (System.currentTimeMillis() - startTime));
            for (Future<T> f : listFuture) {
                if (!f.isDone()) {
                    flag_stop = false;
                    break;
                }
            }
            if (flag_stop) {
                return listFuture;
            }
            else {
                flag_stop = true;
            }
        }

        for (Future<T> f : listFuture) {
            if (!f.isDone()) {
                f.cancel(true);
//                listFuture.remove(f); //?
            }
        }
        return listFuture;
    }

    @Override
    public <T> T invokeAny(Collection<? extends Callable<T>> collection) throws InterruptedException, ExecutionException {
        List<Future<T>> listFuture = new ArrayList<>();
        if (collection.isEmpty()) {
            throw new IllegalArgumentException();
        }
        for (Callable<T> callable : collection) {
            if (callable == null) {
                throw new NullPointerException();
            }
            listFuture.add(submit(callable));
        }
        while (true) {
            waitTermination();
            for (Future<T> f : listFuture) {
                try {
                    if (f.isDone()) {
                        T res = f.get();
                        for (Future<T> t : listFuture) {
                            t.cancel(true);
                        }
                        return res;

                    }
                }
                catch(ExecutionException e) {
                    listFuture.remove(f);
                    if (listFuture.isEmpty()) {
                        throw e;
                    }
                }
            }
        }
    }

    @Override
    public <T> T invokeAny(Collection<? extends Callable<T>> collection, long l, TimeUnit timeUnit) throws InterruptedException, ExecutionException, TimeoutException {
        List<Future<T>> listFuture = new ArrayList<>();
        if (collection.isEmpty()) {
            throw new IllegalArgumentException();
        }
        for (Callable<T> callable : collection) {
            if (callable == null) {
                throw new NullPointerException();
            }
            listFuture.add(submit(callable));
        }
        if (timeUnit != TimeUnit.MILLISECONDS) {
            l = timeUnit.toMillis(l);
        }
        long startTime = System.currentTimeMillis();
        while ((System.currentTimeMillis() - startTime) < l) {
            waitTermination(l - (System.currentTimeMillis() - startTime));
            for (Future<T> f : listFuture) {
                try {
                    if (f.isDone()) {
                        T res = f.get();
                        for (Future<T> t : listFuture) {
                            t.cancel(true);
                        }
                        return res;
                    }
                }
                catch(ExecutionException e) {
                    listFuture.remove(f);
                    if (listFuture.isEmpty()) {
                        throw e;
                    }
                }
            }
        }

        for (Future<T> t : listFuture) {
            t.cancel(true);
        }
        throw new TimeoutException();
    }

    @Override
    public void execute(Runnable runnable) {
        if (runnable == null) {
            throw new NullPointerException();
        }
//        if (isRunning) {
            boolean ret = queue.offer(runnable);
            if (ret) {
                makeThreadsIfNeeded();
                return;
            }
            execute(runnable);
//        }
//        throw new RejectedExecutionException();
        //threw RejectedExecutionException - if this task cannot be accepted for execution
        //      NullPointerException - if command is null
    }

    public void makeThreadsIfNeeded() {
        if (isRunning && !queue.isEmpty() && (threads.size() < maxThreadCount)) {
            Thread w = new Worker();
            synchronized (threads) {
                threads.add(w);
            }
            w.start();
        }
    }

    private class Worker extends Thread {
        public Worker() {
        }

        public void run() {
            while (isRunning) {
                try {
                    Runnable task = queue.poll(timeSavingThread, TimeUnit.MILLISECONDS);//queue.take();

                    if (task == null) {
                        this.interrupt();
                        synchronized (threads) {
                            threads.remove(this);
                        }
                        notifyAwait();
                        return;
                    }
                    task.run();
                    notifyAwait();
                } catch (InterruptedException e) {
                    this.interrupt();
                    synchronized (threads) {
                        threads.remove(this);
                    }
                    return;
                }
            }
            synchronized (threads) {
                threads.remove(this);
            }
        }
    }
}
