//package threadpool;
//
//import java.util.ArrayList;
//import java.util.Collection;
//import java.util.List;
//import java.util.concurrent.*;
//
//public class MExecutorService implements ExecutorService {
//    private int maxThreadCount;
//    private int timeSavingThread;
//    private BlockingQueue<Runnable> queue;
//    private volatile boolean isRunning;
//    private ServiceThread serviceThread;
////    private ThreadClock clock = new ThreadClock();
//    public MExecutorService(int maxThreadCount, BlockingQueue<Runnable> queue) {
//        this.maxThreadCount = maxThreadCount;
//        this.queue = queue;
//        timeSavingThread = 30000;
//        isRunning = true;
//        serviceThread = new ServiceThread(queue, maxThreadCount, timeSavingThread);
//        serviceThread.start();
//    }
//
//    @Override
//    public void shutdown() {
//        isRunning = false;
//        serviceThread.setStop();
//    }
//
//    @Override
//    public List<Runnable> shutdownNow() {
//        shutdown();
//        serviceThread.shutdownThreads();
//        List<Runnable> tasks = new ArrayList<>();
//        while (!queue.isEmpty()) {
//            try {
//                tasks.add(queue.take());
//            } catch (InterruptedException e) {
//                throw new RuntimeException(e);
//            }
//        }
//        return tasks;
//    }
//
//    @Override
//    public boolean isShutdown() {
//        return !isRunning;
//    }
//
//    @Override
//    public boolean isTerminated() {
//        return (queue.isEmpty() && !isRunning && !serviceThread.isAlive());
//    }
//
//    @Override
//    public boolean awaitTermination(long l, TimeUnit timeUnit) throws InterruptedException {
////        if (timeUnit != TimeUnit.MILLISECONDS) {
////            l = timeUnit.toMillis(l);
////        }
////        long startTime = clock.getTime();
////        while ((clock.getTime() - startTime < l) && !isTerminated()) {
////
////        }
//        if (isTerminated()) {
//            return true;
//        }
//        else {
//            return false;
//        }
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public <T> Future<T> submit(Callable<T> callable) {
//        return null;
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public <T> Future<T> submit(Runnable runnable, T t) {
//        return null;
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public Future<?> submit(Runnable runnable) {
//        return null;
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> collection) throws InterruptedException {
//        return null;
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> collection, long l, TimeUnit timeUnit) throws InterruptedException {
//        return null;
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public <T> T invokeAny(Collection<? extends Callable<T>> collection) throws InterruptedException, ExecutionException {
//        return null;
//    }
//
//    @Override////////////////////////////////////////////////////////////////////////////////
//    public <T> T invokeAny(Collection<? extends Callable<T>> collection, long l, TimeUnit timeUnit) throws InterruptedException, ExecutionException, TimeoutException {
//        return null;
//    }
//
//    @Override
//    public void execute(Runnable runnable) {
//        while (true) {
//            if (isRunning) {
//                boolean ret = queue.offer(runnable);
//                if (ret) {
//                    break;
//                }
//            }
//        }
//
////        try {
////            if (isRunning) {
////                queue.put(runnable);
////            }
////        } catch (InterruptedException e) {
////            //throw new RuntimeException(e);
////        }
//
////            if (isRunning) {
////                while (!queue.offer(runnable)) {
////                    try {
////                        wait(1000);
////                    } catch (InterruptedException e) {
////                        throw new RuntimeException(e);
////                    }
////                }
////            }
//    }
//
//}
