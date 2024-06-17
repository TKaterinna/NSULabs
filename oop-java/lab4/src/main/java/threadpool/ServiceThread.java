//package threadpool;
//
//import java.util.ArrayList;
//import java.util.List;
//import java.util.concurrent.BlockingQueue;
//
//public class ServiceThread extends Thread {
//    private int maxThreadCount;
//    private int timeSavingThread;
//    private BlockingQueue<Runnable> queue;
//    private final List<Thread> threads;
//    private volatile boolean isRunning;
////    private ThreadClock clock;
//
//    public ServiceThread(BlockingQueue<Runnable> queue, int maxThreadCount, int timeSavingThread) {
//        this.queue = queue;
//        this.maxThreadCount = maxThreadCount;
//        threads = new ArrayList<>();
//        this.timeSavingThread = timeSavingThread;
//        isRunning = true;
////        clock = new ThreadClock();
//    }
//
//    public void shutdownThreads() {
//        for (Thread worker : threads) {
//            worker.interrupt();
//        }
//        //threads.clear();
//    }
//    public void setStop() {
//        isRunning = false;
//    }
//
//    @Override
//    public void run() {
//        synchronized (threads) {
//            while (true) {
////                System.out.println(threads.size());
//                if (!queue.isEmpty()) {
//                    if (threads.size() < maxThreadCount) {
//                        Thread t = new Worker();
//                        t.start();
//                        threads.add(t);
//                    }
//                }
//                if (isTerminated() && !isRunning) {
//                    break;
//                }
//            }
//        }
//        interrupt();
//    }
//
//    public boolean isTerminated() {
//        synchronized (threads) {
//            for (Thread thread : threads) {
//                if (thread != null && thread.isAlive()) {
//                    return false;
//                }
//            }
//        }
//        return true;
//    }
//
//    private class Worker extends Thread {
//        private long startFreeTime;
//        public Worker() {
//        }
//
//        public void run() {
//            while (isRunning) {
////                if (clock.getTime() - startFreeTime >= timeSavingThread) {
////                    synchronized (threads) {
////                        threads.remove(this);
////                    }
////                    break;
////                }
//                try {
//                    Runnable task = queue.take();
//                    task.run();
////                    startFreeTime = clock.getTime();
//                } catch (InterruptedException e) {
//                    return;
////                    if (!isRunning) {
//////                        synchronized (threads) {
//////                            threads.remove(this);
//////                        }
////                        break;
////                    }
//                }
//            }
//        }
//    }
//}
