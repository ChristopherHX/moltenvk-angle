// Copyright (c) 2020-2021 Eli Aloni (A.K.A elix22).
// Copyright (c) 2008-2021 the Urho3D project.
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

#import <GoogleMobileAds/GoogleMobileAds.h>
#include "../Urho3DAll.h"
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_video.h>

#include "AdmobPlugin.h"

@interface AdmobPluginIOS  : NSObject

@end


@interface AdmobPluginIOS () <GADFullScreenContentDelegate>

@property(nonatomic, strong) GADRewardedAd *rewardedAd;

@property UIViewController * viewController;

@property AdmobPlugin * admobPlugin;

@property Context * context;

@end

@implementation AdmobPluginIOS

- (instancetype)initAdmobController:(UIViewController *)_viewController
{
    self.viewController = _viewController;
    return self;
}

- (void)loadRewardedAd {
  GADRequest *request = [GADRequest request];
  [GADRewardedAd
       loadWithAdUnitID:@"ca-app-pub-3940256099942544/1712485313"
                request:request
      completionHandler:^(GADRewardedAd *ad, NSError *error) {
        if (error) {
          NSLog(@"Rewarded ad failed to load with error: %@", [error localizedDescription]);
          self.admobPlugin->sendEvent("onAdFailedToLoad");
          return;
        }
        self.rewardedAd = ad;
        NSLog(@"Rewarded ad loaded.");
        self.rewardedAd.fullScreenContentDelegate = self;
        self.admobPlugin->sendEvent("onAdLoaded");
      }];
}

- (void)loadRewardedAd:(nonnull NSString *)adUnitID {
  GADRequest *request = [GADRequest request];
  [GADRewardedAd
       loadWithAdUnitID:adUnitID
                request:request
      completionHandler:^(GADRewardedAd *ad, NSError *error) {
        if (error) {
          NSLog(@"Rewarded ad failed to load with error: %@", [error localizedDescription]);
          self.admobPlugin->sendEvent("onAdFailedToLoad");
          return;
        }
        self.rewardedAd = ad;
        NSLog(@"Rewarded ad loaded.");
        self.rewardedAd.fullScreenContentDelegate = self;
        self.admobPlugin->sendEvent("onAdLoaded");
      }];
}

- (void)showVideo {
  if (self.rewardedAd && [self.rewardedAd canPresentFromRootViewController:self.viewController error:nil]) {
    [self.rewardedAd presentFromRootViewController:self.viewController
                          userDidEarnRewardHandler:^{
                            GADAdReward *reward = self.rewardedAd.adReward;

                            NSString *rewardMessage = [NSString
                                stringWithFormat:@"Reward received with currency %@ , amount %lf",
                                                 reward.type, [reward.amount doubleValue]];
                            NSLog(@"%@", rewardMessage);
        
                            auto jsonBuilder = (MakeShared<JsonBuilder>(self.context));
                            (*jsonBuilder)("source",self.admobPlugin->GetTypeName())
                                          ("event","onUserEarnedReward")
                                          ("rewardType", reward.type.UTF8String)
                                          ("rewardAmount", (int)[reward.amount doubleValue]);
                            self.admobPlugin->OnPluginEvent(jsonBuilder->F());
                          }];
  }
}


#pragma mark GADFullScreenContent implementation

/// Tells the delegate that the rewarded ad was presented.
- (void)adDidPresentFullScreenContent:(id)ad {
  NSLog(@"Rewarded ad presented.");
  self.admobPlugin->sendEvent("onAdShowedFullScreenContent");
}

/// Tells the delegate that the rewarded ad failed to present.
- (void)ad:(id)ad didFailToPresentFullScreenContentWithError:(NSError *)error {
  NSLog(@"Rewarded ad failed to present with error: %@", [error localizedDescription]);
  self.admobPlugin->sendEvent("onAdFailedToShowFullScreenContent");
}

/// Tells the delegate that the rewarded ad was dismissed.
- (void)adDidDismissFullScreenContent:(id)ad {
  NSLog(@"Rewarded ad dismissed.");
  self.admobPlugin->sendEvent("onAdDismissedFullScreenContent");
}

@end


#if __cplusplus
extern "C" {
#endif
UIViewController * SDL_GetUIKitViewController(SDL_Window *window);
#if __cplusplus
}   // Extern C
#endif

//TBD ELI , this is an hack , need to figure out this one.s
static AdmobPluginIOS * admobIOSPlugin = NULL;

void * IOS_AdmobInit(Context * context,AdmobPlugin * plugin)
{
    Graphics* graphics = context->GetSubsystem<Graphics>();
    UIViewController * viewController = SDL_GetUIKitViewController(graphics->GetWindow());
    
    admobIOSPlugin = [[AdmobPluginIOS alloc] initAdmobController:viewController];
   
    [[GADMobileAds sharedInstance] startWithCompletionHandler:nil];
    
    admobIOSPlugin.admobPlugin = plugin;
    admobIOSPlugin.context = context;
    
    return (__bridge void *)admobIOSPlugin;
}

bool AdmobPlugin::PostCommandToIOS(Context * context,const String& method, JSONFile& data)
{
    AdmobPluginIOS * admobIOSPlugin = (__bridge AdmobPluginIOS *)ios_plugin_handle;
    bool res = false;
    if(admobIOSPlugin != NULL)
    {
        if(method == "loadRewardedAd")
        {
            String testDeviceId = data.GetRoot()["testDeviceId"].GetString();
            
            if(testDeviceId != "")
            {
                NSString  * testDeviceIdNSString = [NSString stringWithUTF8String:testDeviceId.CString()] ;
                GADMobileAds.sharedInstance.requestConfiguration.testDeviceIdentifiers =@[ testDeviceIdNSString  ];
            }
            
            String adUnitIdString = data.GetRoot()["adUnitId"].GetString();
            
            if(adUnitIdString != "")
            {
                NSString  * adUnitIdNSString = [NSString stringWithUTF8String:adUnitIdString.CString()] ;
                [admobIOSPlugin loadRewardedAd:adUnitIdNSString];
            }
            else
            {
                [admobIOSPlugin loadRewardedAd];
            }
            res = true;
        }
        else  if(method == "showRewardedVideo")
        {
            [admobIOSPlugin showVideo];
            res = true;
        }
    }
    else if(method == "Start")
    {
        if(ios_plugin_handle == NULL)
        {
            ios_plugin_handle = IOS_AdmobInit(context,this);
        }
        
        if(ios_plugin_handle != NULL)
        {
            auto jsonBuilder = (MakeShared<JsonBuilder>(context));
            (*jsonBuilder)("source",this->GetTypeName())("event","OnStarted");
            this->OnPluginEvent(jsonBuilder->F());
        }
    }
    return res;
}


