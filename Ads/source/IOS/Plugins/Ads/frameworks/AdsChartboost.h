//
//  AdsChartboost.h
//  Ads
//
//  Created by Arturs Sosins on 6/25/13.
//  Copyright (c) 2013 Gideros Mobile. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AdsProtocol.h"
#import "Chartboost.h"
#import "AdsManager.h"

@interface AdsChartboost : NSObject <AdsProtocol, ChartboostDelegate>
@property (nonatomic, retain) Chartboost *cb;
@property (nonatomic, retain) AdsManager *mngr;
@end
