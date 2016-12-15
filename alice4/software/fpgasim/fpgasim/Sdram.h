//
//  Sdram.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 12/14/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

// Simulates an SDRAM module. The module does not store data, but keeps track of
// the amount of time it would take to access each word. It does not even know the
// word size. The simulation does not permit parallel work (e.g., writing to one
// bank while opening a row on another).
@interface Sdram : NSObject

@property (readonly) long cycles;
@property (readonly) int bankCount;
@property (readonly) int rowCount;
@property (readonly) int columnCount;
@property (readonly) int clockFrequency;

- (id)init;

- (void)read:(int)count bank:(int)bank row:(int)row column:(int)column;
- (void)write:(int)count bank:(int)bank row:(int)row column:(int)column;

- (void)resetStats;

@end
