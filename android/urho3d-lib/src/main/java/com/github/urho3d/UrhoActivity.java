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


public class UrhoActivity extends SDLActivity {


    private static final String TAG = "Urho3D";

    private static final String AD_UNIT_ID = "ca-app-pub-3940256099942544/5224354917";

    private RewardedAd rewardedAd;
    boolean isLoading;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected  class UrhoActivityHandler extends Handler {
        public void handleMessage(Message msg) {
            processData((String)msg.obj);
        }
    }

    Handler urhoActivityHandler = new UrhoActivityHandler();

    public static native void nativeUserActivityCallback(String json);


    static void postDataToUI(String data) {
        UrhoActivity urhoActivity = (UrhoActivity)(SDLActivity.mSingleton);
        Message msg = urhoActivity.urhoActivityHandler.obtainMessage();
        msg.obj = data;
        urhoActivity.urhoActivityHandler.sendMessage(msg);
    }

    void notifyPlatform(String source , String event, JSONObject params) {
        try {
            params.put("source", source);
            params.put("event", event);
            nativeUserActivityCallback(params.toString());
        } catch (JSONException e) 
        {
            Log.e(TAG, "JSONException " + e);
        }
    }

    void notifyPlatform(String source, String event) {
        notifyPlatform(source, event, new JSONObject());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        MobileAds.initialize(this, new OnInitializationCompleteListener() {
            @Override
            public void onInitializationComplete(InitializationStatus initializationStatus) {
            }
          });

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

    private void loadRewardedAd() {
        if (rewardedAd == null) {
          isLoading = true;
          AdRequest adRequest = new AdRequest.Builder().build();
          RewardedAd.load(
              this,
              AD_UNIT_ID,
              adRequest,
              new RewardedAdLoadCallback() {
                @Override
                public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                  // Handle the error.
                  Log.d(TAG, loadAdError.getMessage());
                  rewardedAd = null;
                  UrhoActivity.this.isLoading = false;
                  Toast.makeText(UrhoActivity.this, "onAdFailedToLoad", Toast.LENGTH_SHORT).show();

                  notifyPlatform("UrhoActivity", "onAdFailedToLoad");
                }
    
                @Override
                public void onAdLoaded(@NonNull RewardedAd rewardedAd) {
                  UrhoActivity.this.rewardedAd = rewardedAd;
                  Log.d(TAG, "onAdLoaded");
                  UrhoActivity.this.isLoading = false;
                  Toast.makeText(UrhoActivity.this, "onAdLoaded", Toast.LENGTH_SHORT).show();

                  notifyPlatform("UrhoActivity", "onAdLoaded");
                }
              });
        }
    }

    private void showRewardedVideo() 
    {

        if (rewardedAd == null) {
          Log.d(TAG, "The rewarded ad wasn't ready yet.");
          return;
        }

        rewardedAd.setFullScreenContentCallback(
            new FullScreenContentCallback() {
              @Override
              public void onAdShowedFullScreenContent() {
                // Called when ad is shown.
                Log.d(TAG, "onAdShowedFullScreenContent");
                Toast.makeText(UrhoActivity.this, "onAdShowedFullScreenContent", Toast.LENGTH_SHORT)
                    .show();

                notifyPlatform("UrhoActivity", "onAdShowedFullScreenContent");
              }
    
              @Override
              public void onAdFailedToShowFullScreenContent(AdError adError) {
                // Called when ad fails to show.
                Log.d(TAG, "onAdFailedToShowFullScreenContent");
                // Don't forget to set the ad reference to null so you
                // don't show the ad a second time.
                rewardedAd = null;
                Toast.makeText(
                        UrhoActivity.this, "onAdFailedToShowFullScreenContent", Toast.LENGTH_SHORT)
                    .show();

                notifyPlatform("UrhoActivity", "onAdFailedToShowFullScreenContent");
              }
    
              @Override
              public void onAdDismissedFullScreenContent() {
                // Called when ad is dismissed.
                // Don't forget to set the ad reference to null so you
                // don't show the ad a second time.
                rewardedAd = null;
                Log.d(TAG, "onAdDismissedFullScreenContent");
                Toast.makeText(UrhoActivity.this, "onAdDismissedFullScreenContent", Toast.LENGTH_SHORT)
                    .show();
                
                notifyPlatform("UrhoActivity", "onAdDismissedFullScreenContent");

              }
            });
        Activity activityContext = UrhoActivity.this;
        rewardedAd.show(
            activityContext,
            new OnUserEarnedRewardListener() {
              @Override
              public void onUserEarnedReward(@NonNull RewardItem rewardItem) {
                // Handle the reward.
                Log.d(TAG, "The user earned the reward.");
                int rewardAmount = rewardItem.getAmount();
                String rewardType = rewardItem.getType();
                
                try {
                JSONObject params = new JSONObject();
                params.put("rewardType", rewardType);
                params.put("rewardAmount", rewardAmount);
                notifyPlatform("UrhoActivity" , "onUserEarnedReward", params);
                }
                catch (JSONException e) 
                {
                    Log.e(TAG, "JSONException " + e);
                }

              }
            });
      }

      private void  processData(String data) {
        try {
            JSONObject js = new JSONObject(data);
            String methodName = js.getString("method");
            Method  method = UrhoActivity.class.getDeclaredMethod(methodName,  JSONObject.class) ;
            if(method != null)
            {
                method.setAccessible(true);
                method.invoke(this, js);
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

    void showRewardedVideo(JSONObject js)
    {
        showRewardedVideo();
    }

    void loadRewardedAd(JSONObject js)
    {
        loadRewardedAd();
    }

    

}
