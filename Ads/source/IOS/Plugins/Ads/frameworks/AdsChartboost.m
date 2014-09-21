//
//  AdsChartboost.m
//  Ads
//
//  Created by Arturs Sosins on 6/25/13.
//  Copyright (c) 2013 Gideros Mobile. All rights reserved.
//
#include "gideros.h"
#import "AdsChartboost.h"
#import "AdsClass.h"

@implementation AdsChartboost
-(id)init{
    self.cb = [Chartboost sharedChartboost];
    self.mngr = [[AdsManager alloc] init];
    return self;
}

-(void)destroy{
    [self.mngr destroy];
    [self.mngr release];
    self.mngr = nil;
}

-(void)setKey:(NSMutableArray*)parameters{
    self.cb.appId = [parameters objectAtIndex:0];
    self.cb.appSignature = [parameters objectAtIndex:1];
    self.cb.delegate = self;
    [Chartboost startWithAppId:[parameters objectAtIndex:0] appSignature:[parameters objectAtIndex:1] delegate:self];
}

-(void)loadAd:(NSMutableArray*)parameters{
    NSString *type = [parameters objectAtIndex:0];
    NSString *tag = nil;
    if([parameters count] > 1)
        tag = [parameters objectAtIndex:1];
    if ([type isEqualToString:@"interstitial"] || [type isEqualToString:@"auto"]) {
        AdsStateChangeListener *listener = [[AdsStateChangeListener alloc] init];
        [listener setShow:^(){
            [AdsClass adDisplayed:[self class] forType:type];
            if(tag != nil)
                [[Chartboost sharedChartboost] showInterstitial:tag];
            else
                [[Chartboost sharedChartboost] showInterstitial:CBLocationHomeScreen];
        }];
        [listener setDestroy:^(){}];
        [listener setHide:^(){}];
        [self.mngr set:self.cb forType:type withListener:listener];
        if(tag != nil)
            [self.cb cacheInterstitial:tag];
        else
            [self.cb cacheInterstitial:CBLocationHomeScreen];
    }
    else if ([type isEqualToString:@"moreapps"]) {
        AdsStateChangeListener *listener = [[AdsStateChangeListener alloc] init];
        [listener setShow:^(){
            [AdsClass adDisplayed:[self class] forType:type];
            [self.cb showMoreApps:CBLocationHomeScreen];
        }];
        [listener setDestroy:^(){}];
        [listener setHide:^(){}];
        [self.mngr set:self.cb forType:type withListener:listener];
        [self.cb cacheMoreApps:CBLocationHomeScreen];
    }
    else
    {
         [AdsClass adError:[self class] with:[NSString stringWithFormat:@"Unknown type: %@", type]];
    }
}

-(void)showAd:(NSMutableArray*)parameters{
    NSString *type = [parameters objectAtIndex:0];
    if([self.mngr get:type] == nil)
        [self loadAd:parameters];
    [self.mngr show:type];
}

-(void)hideAd:(NSString*)type{
    [self.mngr hide:type];
}

-(void)enableTesting{
}

-(UIView*)getView{
    return nil;
}

- (BOOL)shouldRequestInterstitial:(NSString *)location{
    return YES;
}

- (void)didCacheInterstitial:(NSString *)location{
    [AdsClass adReceived:[self class] forType:@"interstitial"];
    [self.mngr load:@"interstitial"];
}

- (BOOL)shouldDisplayInterstitial:(NSString *)location{
    return YES;
}

- (void)didFailToLoadInterstitial:(NSString *)location{
    [AdsClass adFailed:[self class] with:@"Failed to receive ads" forType:@"interstitial"];
    [self.mngr reset:@"interstitial"];
}

- (void)didDismissInterstitial:(NSString *)location{
    [AdsClass adDismissed:[self class] forType:@"interstitial"];
    [self.cb cacheInterstitial:CBLocationHomeScreen];
}

- (void)didCloseInterstitial:(NSString *)location{
    [AdsClass adActionEnd:[self class] forType:@"interstitial"];
}

- (void)didClickInterstitial:(NSString *)location{
    [AdsClass adActionBegin:[self class] forType:@"interstitial"];
}


- (void)didCacheMoreApps{
    [AdsClass adReceived:[self class] forType:@"moreapps"];
    [self.mngr load:@"moreapps"];

}

- (BOOL)shouldDisplayMoreApps{
    return YES;
}

- (void)didFailToLoadMoreApps{
    [AdsClass adFailed:[self class] with:@"Failed to receive ads" forType:@"moreapps"];
    [self.mngr reset:@"moreapps"];
}

- (void)didDismissMoreApps{
    [AdsClass adDismissed:[self class] forType:@"moreapps"];
    [self.cb cacheMoreApps:CBLocationHomeScreen];
}

- (void)didCloseMoreApps{
    [AdsClass adActionEnd:[self class] forType:@"moreapps"];
}

- (void)didClickMoreApps{
    [AdsClass adActionBegin:[self class] forType:@"moreapps"];
}

@end
