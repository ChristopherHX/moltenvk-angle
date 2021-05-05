//
// Copyright (c) 2008-2020 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

package com.github.urho3d;

import android.content.Context;
import org.libsdl.app.SDLActivity;
import java.io.File;
import java.util.Arrays;
import android.util.SparseArray;
import java.util.ArrayList;
import java.io.FilenameFilter;
import java.util.Comparator;
import android.os.Bundle;
import android.util.Log;
import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.lang.reflect.*;
import java.lang.reflect.Method;  
import 	java.lang.Class;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.OnUserEarnedRewardListener;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;

import android.widget.Toast;
import androidx.annotation.NonNull;

import com.github.plugin.AdmobPlugin;


public class UrhoActivity extends SDLActivity {


    private static final String TAG = "Urho3D";

    AdmobPlugin admobPlugin = null;

    public static UrhoActivity GetSingelton()
    {
        return (UrhoActivity)(SDLActivity.mSingleton);
    }

    ///////////////////////////////////////////////////////////////////////this;////////////////////////////////////////////////////////////////////
    protected  class UrhoActivityHandler extends Handler {
        public void handleMessage(Message msg) {
            processCommand((String)msg.obj);
        }
    }

    Handler urhoActivityHandler = new UrhoActivityHandler();

    static void postCommand(String data) {
        UrhoActivity urhoActivity = (UrhoActivity)(SDLActivity.mSingleton);
        Message msg = urhoActivity.urhoActivityHandler.obtainMessage();
        msg.obj = data;
        urhoActivity.urhoActivityHandler.sendMessage(msg);
    }

  

    public void notifyPlatform(String source , String event, JSONObject params) {
        try {
            params.put("source", source);
            params.put("event", event);
            OnNativePlatformEvent(params.toString());
        } catch (JSONException e) 
        {
            Log.e(TAG, "JSONException " + e);
        }
    }

    public void notifyPlatform(String source, String event) {
        notifyPlatform(source, event, new JSONObject());
    }


    public static native void OnNativePlatformEvent(String json);


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        admobPlugin = new AdmobPlugin();
    }

    public static ArrayList<String> getLibraryNames(Context context )
    {
        String libraryPath = context.getApplicationInfo().nativeLibraryDir;
        File[] files = new File(libraryPath).listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                // Only list libraries, i.e. exclude gdbserver when it presents
                return filename.matches("^lib.*\\.so$");
            }
        });
        Arrays.sort(files, new Comparator<File>() {
            @Override
            public int compare(File lhs, File rhs) {
                return Long.valueOf(lhs.lastModified()).compareTo(rhs.lastModified());
            }
        });

        ArrayList<String> libraryNames = new ArrayList<String>(files.length);

        for (final File libraryFilename : files) {
            String name = libraryFilename.getName().replaceAll("^lib(.*)\\.so$", "$1");
            libraryNames.add(name);
        }

        return libraryNames;
    }

    Class getClassFromClassName( String className)
    {
        Class<?> cls = null;
        try 
        {
            if(className.equals("Internal"))
            {
                cls =   Class.forName("com.github.urho3d.UrhoActivity");
            }
            else
            {
                cls = Class.forName("com.github.plugin."+className);
            }
        }
        catch (Exception e) 
        {
            Log.e(TAG, "onUnhandledMessage Exception for " + className, e);
        }

         return cls;
    }
   
    private void processCommand(String data) {
        try {
            JSONObject js = new JSONObject(data);
            String className = js.getString("class");
            String methodName = js.getString("method");

            Class<?> cls = getClassFromClassName(className);
            if(cls != null)
            {
                Method  getSingelton = cls.getDeclaredMethod("GetSingelton",  null) ;
                if(getSingelton != null)
                {
                    getSingelton.setAccessible(true);
                    Object inst =  getSingelton.invoke(null);
                    if(inst != null)
                    {
                        Method  method = cls.getDeclaredMethod(methodName,  JSONObject.class) ;
                        if(method != null)
                        {
                            method.setAccessible(true);
                            method.invoke(inst, js);
                        }
                    }
                }
            }
            
        } catch (ClassCastException e) {
            Log.e(TAG, "onUnhandledMessage ClassCastException", e);
        } catch (JSONException e) {
            Log.e(TAG, "onUnhandledMessage JSONException", e);
        } catch (SecurityException e) {
            Log.e(TAG, "onUnhandledMessage SecurityException", e);
        } catch (NoSuchMethodException e) {
            Log.e(TAG, "onUnhandledMessage NoSuchMethodException", e);
        } catch (Exception e) {
            Log.e(TAG, "onUnhandledMessage Exception", e);
        }
    }

    void shareText(JSONObject js) {
        try {
            Log.d(TAG, "shareText title: " + js.getString("title") + "subject: " + js.getString("subject") + "text: " + js.getString("text"));

            Toast.makeText(
                UrhoActivity.this, js.getString("text"), Toast.LENGTH_SHORT)
            .show();

        } 
        catch (Exception e) 
        {
            Log.e(TAG, "onUnhandledMessage Exception", e);
        }
    }


}
