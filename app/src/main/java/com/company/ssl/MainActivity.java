package com.company.ssl;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.graphics.drawable.BitmapDrawable;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Toast;

import com.company.constants.MyConstants;
import com.company.module.UsbCameraModule;
import com.company.view.AutoFitTextureView;

import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;

import static java.lang.Math.PI;
import static java.lang.Math.cos;
import static java.lang.Math.pow;
import static java.lang.Math.rint;
import static java.lang.Math.round;
import static java.lang.Math.sin;
import static java.lang.Math.sqrt;
import static java.lang.Math.tan;

public class MainActivity extends AppCompatActivity {
    private final String TAG = MainActivity.class.getSimpleName();
    private final int[] colorMap = {0x800000,0x900000,0xA00000,0xB00000,0xC00000,0xD00000,0xE00000,0xF00000,
            0xFF0000,0xFF1000,0xFF2000,0xFF3000,0xFF4000,0xFF5000,0xFF6000,0xFF7000,
            0xFF8000,0xFF9000,0xFFA000,0xFFB000,0xFFC000,0xFFD000,0xFFE000,0xFFF000,
            0xFFFF00,0xF0FF10,0xE0FF20,0xD0FF30,0xC0FF40,0xB0FF50,0xA0FF60,0x90FF70,
            0x80FF80,0x70FF90,0x60FFA0,0x50FFB0,0x40FFC0,0x30FFD0,0x20FFE0,0x10FFF0,
            0x00FFFF,0x00F0FF,0x00E0FF,0x00D0FF,0x00C0FF,0x00B0FF,0x00A0FF,0x0090FF,
            0x0080FF,0x0070FF,0x0060FF,0x0050FF,0x0040FF,0x0030FF,0x0020FF,0x0010FF,
            0x0000FF,0x0000F0,0x0000E0,0x0000D0,0x0000C0,0x0000B0,0x0000A0,0x000090,0x000080};
    private final double[] collectMatrix = {415.962094212636,320.847062285448,399.466642821620,0.934791158729619,
            406.677394741051,239.603680350901,-406.940826536906,5.05088524890112};

    private ImageView mImageView;
    private Bitmap soundbitmap;
    private BitmapDrawable bmpDraw;
    private Bitmap mBitmap;
    private int xCoordinate,yCoordinate;

    private UsbCameraModule mUsbCameraModule;
    private Surface mPreviewSurface;
    private AutoFitTextureView mTextureView;
    private ImageButton mCameraButton;
    private static final int REQUEST_PERMISSIONS = 1;

    public Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what){
                case 0x00:
                    mImageView.setImageBitmap(mBitmap);
                    break;

                default:
                    break;
            }
        }
    };

    public void SendMessage(final int msg)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {Message message = new Message();
                message.what = msg;
                handler.sendMessage(message);
            }
        }).start();
    }


    /**
     * 应用程序运行命令获取 Root权限，设备必须已破解(获得ROOT权限)
     *
     * @return 应用程序是/否获取Root权限
     */
    public static boolean upgradeRootPermission(String pkgCodePath) {
        Process process = null;
        DataOutputStream os = null;
        try {
            String cmd="chmod 777 " + pkgCodePath;
            process = Runtime.getRuntime().exec("su"); //切换到root帐号
            os = new DataOutputStream(process.getOutputStream());
            os.writeBytes(cmd + "\n");
            os.writeBytes("exit\n");
            os.flush();
            process.waitFor();
        } catch (Exception e) {
            return false;
        } finally {
            try {
                if (os != null) {
                    os.close();
                }
                process.destroy();
            } catch (Exception e) {
            }
        }
        return true;
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mImageView = (ImageView)findViewById(R.id.image);

        mCameraButton = (ImageButton)findViewById(R.id.camera_button);
        mCameraButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                capture();
            }
        });

        mTextureView = (AutoFitTextureView) findViewById(R.id.TextureView1);
        mTextureView.setSurfaceTextureListener(mSurfaceTextureListener);

        initUsbCameraModule();
        startRequestRealStream();

        if (!hasPermissionsGranted(new String[]{Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO})) {
            requestPermissions(new String[]{Manifest.permission.CAMERA, Manifest.permission.MOUNT_UNMOUNT_FILESYSTEMS,
                    Manifest.permission.SYSTEM_ALERT_WINDOW,Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE}, REQUEST_PERMISSIONS);
        }

//        boolean flag = upgradeRootPermission(getPackageCodePath());
//        Log.d(TAG,"Upgrade Root Permission: "+flag);
//        AudioCapture.tinycap();
//        new Thread() {
//            public void run() {
//                while(true) {
//                    AudioCapture.open();
//                    double[] expectedData = new double[2];
//                    double x,y,z;
//                    while(true) {
//                        if (AudioCapture.read(expectedData,0) > 0) {
//                            break;
//                        }
//                        x = cos(PI * expectedData[0] /180) * cos(PI * expectedData[1] /180);
//                        y = -sin(PI * expectedData[0] /180) * cos(PI * expectedData[1] /180);
//                        z = sin(PI * expectedData[1] /180);
//                        xCoordinate = (int)round((x*collectMatrix[0]+y*collectMatrix[1]+z*collectMatrix[2]+collectMatrix[3])/y);
//                        yCoordinate = (int)round((x*collectMatrix[4]+y*collectMatrix[5]+z*collectMatrix[6]+collectMatrix[7])/y);
////                        double max = -10000, min = 10000;
////                        for (int i = 0; i < 32761; ++i) {
////                            if (specData[i] > max) max = specData[i];
////                            if (specData[i] < min) min = specData[i];
////                        }
////                        Log.d(TAG, "max:" + max + " min:" + min);
////                        int[] picture = new int[32761];
////                        int res;
////                        for (int i = 0; i < 32761; ++i) {
////                            res = (int) ((max - specData[32760 - i]) / (max - min) * 64);
////                            if(res < 12) picture[i] = (0xFF << 24) | colorMap[res];
////                            else picture[i] = (0x00 << 24) | colorMap[res];
////                        }
////                        soundbitmap = Bitmap.createBitmap(picture, 181, 181, Bitmap.Config.ARGB_8888);
////                        Matrix mx = new Matrix();
////                        mx.postScale(-1, 1);
////                        soundbitmap = Bitmap.createBitmap(soundbitmap, 0, 0, soundbitmap.getWidth(), soundbitmap.getHeight(), mx, false);
////                        soundbitmap = Bitmap.createScaledBitmap(soundbitmap,imageWidth,imageWidth,false);
//                    }
//                    AudioCapture.close();
//                }
//            }
//        }.start();

        new Thread() {
            public void run() {
                while(true) {
                    AudioCapture.open(0);
                    int[] expectedData = new int[2];
                    while(true) {
                        if (AudioCapture.read(expectedData, 0) > 0) {
                            break;
                        }
                    }
                    AudioCapture.close(0);
                }
            }
        }.start();
//        new Thread() {
//            public void run() {
//                while(true) {
//                    double[] expectedData = new double[2];
//                    while(true) {
//                        iq2;f (AudioCapture.read(expectedData, 1) > 0) {
//                            break;
//                        }
//                    }
//                }
//            }
//        }.start();

    }

    private boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(getApplicationContext(), permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        Log.d(TAG, "onRequestPermissionsResult");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG,"--- onResume();");
        registerUsbMonitot(true);
        if(mUsbCameraModule != null){
            if(mPreviewSurface != null && mPreviewSurface.isValid()){
                startPreview(mPreviewSurface);
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        stopPreview();
        registerUsbMonitot(false);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        disConncetUsbCam();
    }

    private void initUsbCameraModule(){
        Log.v(TAG,"---- initUsbCameraModule();mUsbCameraModule:"+mUsbCameraModule);
        if(mUsbCameraModule == null) {
            mUsbCameraModule = UsbCameraModule.getInstance(getApplicationContext());
            mUsbCameraModule.init(MyConstants.DEFAULT_USB_CAM_WIDTH, MyConstants.DEFAULT_USB_CAM_HEIGHT);
        }
    }

    private void registerUsbMonitot(boolean register){
        if(mUsbCameraModule != null){
            if(register){
                mUsbCameraModule.registerUsbMonitor(null,true);
            }else{
                mUsbCameraModule.registerUsbMonitor(null,false);
            }
        }
    }
    /**
     * Start Preview
     * @param surface
     */
    private void startPreview(Surface surface){
        Log.v(TAG,"--- startPreview();");
        if(mUsbCameraModule != null){
            mUsbCameraModule.startPreview(surface);
        }
    }

    /**
     * Stop Preview
     */
    private void stopPreview(){
        if(mUsbCameraModule != null){
            mUsbCameraModule.stopPreview();
        }
    }

    /**
     * Start Record
     */
    private void startRecord() {
        if(mUsbCameraModule != null){
            mUsbCameraModule.startUsbCamRecording();
            showToast(R.string.start_record);
        }
    }

    /**
     * Stop Record
     */
    private void stopRecord() {
        if(mUsbCameraModule != null){
            mUsbCameraModule.stopUsbCamRecording();
            showToast(R.string.stop_record);
        }
    }

    /**
     * Take Picture
     */
    private void capture() {
        if(mUsbCameraModule != null){
            mUsbCameraModule.capture();
            showToast(R.string.capture);
        }
    }

    /**
     * Set RealStream CallBack
     */
    private void startRequestRealStream() {
        if(mUsbCameraModule != null){
            mUsbCameraModule.setUsbCameraFrameCallback(new UsbCameraModule.IUsbFrameCallback() {
                @Override
                public void onStreamUpdate(byte[] frameByte, int format, int width, int height) {
//                    Log.v(TAG,"----onStreamUpdate(); width:"+width + ";height:"+height);
                    mBitmap = drawSoundLocation();
                    SendMessage(0x00);
                }
            });
        }
    }

    private void stopRequestRealStream() {
        if(mUsbCameraModule != null){
            mUsbCameraModule.setUsbCameraFrameCallback(null);
        }
    }

    private void disConncetUsbCam(){
        if(mUsbCameraModule != null){
            mUsbCameraModule.destory();
        }
    }

    TextureView.SurfaceTextureListener mSurfaceTextureListener = new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            mPreviewSurface = new Surface(surface);
            startPreview(mPreviewSurface);
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            return true;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {

        }
    };

    private void showToast(int textId){
        Toast.makeText(getApplicationContext(),getResources().getString(textId),Toast.LENGTH_SHORT).show();
    }

    public Bitmap drawSoundLocation()
    {
        Bitmap newmap = Bitmap.createBitmap(MyConstants.DEFAULT_USB_CAM_WIDTH, MyConstants.DEFAULT_USB_CAM_HEIGHT, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(newmap);
        Paint paint = new Paint();
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(1);
        canvas.drawBitmap(mUsbCameraModule.mBitmap,0,0,null);
        for(int i = 15 ; i > 0 ; --i) {
            paint.setColor((0xFF << 24) | colorMap[i+2]);
            canvas.drawCircle(xCoordinate, yCoordinate, i, paint);
        }
        return newmap;
    }

    public static Bitmap combineBitmap(Bitmap background, Bitmap foreground)
    {
        if (background == null)
        {
            return null;
        }
        int bgWidth = background.getWidth();
        int bgHeight = background.getHeight();
        int fgWidth = foreground.getWidth();
        int fgHeight = foreground.getHeight();
        Bitmap newmap = Bitmap.createBitmap(bgWidth, bgHeight, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(newmap);
        canvas.drawBitmap(background, 0, 0, null);
        canvas.drawBitmap(foreground, (bgWidth - fgWidth) / 2, (bgHeight - fgHeight) / 2, null);
        canvas.save();
        canvas.restore();
        return newmap;
    }

    /**
     * 旋转变换
     *
     * @param origin 原图
     * @param alpha  旋转角度，可正可负
     * @return 旋转后的图片
     */
    private Bitmap rotateBitmap(Bitmap origin, float alpha) {
        if (origin == null) {
            return null;
        }
        int width = origin.getWidth();
        int height = origin.getHeight();
        Matrix matrix = new Matrix();
        matrix.setRotate(alpha);
        // 围绕原地进行旋转
        Bitmap newBM = Bitmap.createBitmap(origin, 0, 0, width, height, matrix, false);
        if (newBM.equals(origin)) {
            return newBM;
        }
        origin.recycle();
        return newBM;
    }
}