package com.company.ssl;

public class AudioCapture {
    static {
        System.loadLibrary("tinycap");
    }
    public static native int tinycap();
    public static native int open();
    public static native int read(double[] specData,int number);
    public static native void close();
}
