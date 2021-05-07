#import <GoogleMobileAds/GoogleMobileAds.h>
#include "Urho3DAll.h"
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

- (void)showVideo {
  if (self.rewardedAd && [self.rewardedAd canPresentFromRootViewController:self.viewController error:nil]) {
    [self.rewardedAd presentFromRootViewController:self.viewController
                          userDidEarnRewardHandler:^{
                            GADAdReward *reward = self.rewardedAd.adReward;

                            NSString *rewardMessage = [NSString
                                stringWithFormat:@"Reward received with currency %@ , amount %lf",
                                                 reward.type, [reward.amount doubleValue]];
                            NSLog(@"%@", rewardMessage);
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
  [self loadRewardedAd];
  NSLog(@"Rewarded ad dismissed.");
  self.admobPlugin->sendEvent("onAdDismissedFullScreenContent");
}

@end


#if __cplusplus
extern "C" {
#endif
UIViewController * SDL_GetUiViewController(SDL_Window *window);
#if __cplusplus
}   // Extern C
#endif

//TBD ELI , this is an hack , need to figure out this one.s
static AdmobPluginIOS * admobIOSPlugin;

void * IOS_AdmobInit(Context * context,AdmobPlugin * plugin)
{
    Graphics* graphics = context->GetSubsystem<Graphics>();
    UIViewController * viewController = SDL_GetUiViewController(graphics->GetWindow());
    
    admobIOSPlugin = [[AdmobPluginIOS alloc] initAdmobController:viewController];
   
    [[GADMobileAds sharedInstance] startWithCompletionHandler:nil];
    
    admobIOSPlugin.admobPlugin = plugin;
    
    return (__bridge void *)admobIOSPlugin;
}

bool AdmobPlugin::PostIOSCommand(const String& method, JSONFile& data)
{
    AdmobPluginIOS * admobIOSPlugin = (__bridge AdmobPluginIOS *)ios_admob_plugin;
    bool res = false;
    if(admobIOSPlugin != NULL)
    {
        if(method == "loadRewardedAd")
        {
            [admobIOSPlugin loadRewardedAd];
            res = true;
        }
        else  if(method == "showRewardedVideo")
        {
            [admobIOSPlugin showVideo];
            res = true;
        }
    }
    return res;
}


