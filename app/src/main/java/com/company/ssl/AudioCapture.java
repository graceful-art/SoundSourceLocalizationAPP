package com.company.ssl;

public class AudioCapture {
    static {
        System.loadLibrary("tinycap");
    }
    public static native int tinycap();
    public static native int open(int number);
    public static native int read(int[] specData,int number);
    public static native void close(int number);
}
