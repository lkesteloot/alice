//
//  Sdram.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 12/14/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "Sdram.h"

static const int BANK_COUNT = 4;
static const int ROW_COUNT = 4096;
static const int COLUMN_COUNT = 256;
static const int CAS_LATENCY = 3;
static const int CLOCK_FREQUENCY = 166000000;
static const int tRC = 10;
static const int tRCD = 3;
static const int tRAS = 7;
static const int tRP = 3;

@implementation Sdram {
    // Which row is open for the specified bank, or -1 if none is open.
    int mOpenRow[BANK_COUNT];

    // The most recent cycle when a row was activated on this bank.
    long mLastOpen[BANK_COUNT];

    // Elapsed clock cycles.
    long mCycles;
}

- (id)init {
    self = [super init];

    if (self) {
	[self resetStats];
    }

    return self;
}

- (void)resetStats {
    mCycles = 0;
    for (int i = 0; i < BANK_COUNT; i++) {
	mOpenRow[i] = -1;
	mLastOpen[i] = 0;
    }
}

- (long)cycles {
    return mCycles;
}

- (int)bankCount {
    return BANK_COUNT;
}

- (int)rowCount {
    return ROW_COUNT;
}

- (int)columnCount {
    return COLUMN_COUNT;
}

- (int)clockFrequency {
    return CLOCK_FREQUENCY;
}

// Open the specified row on the bank. Close the bank first if necessary.
- (void)openRow:(int)row onBank:(int)bank {
    if (mOpenRow[bank] != row) {
	[self closeBank:bank];
    }

    mOpenRow[bank] = row;

    mCycles += MIN(tRC, mCycles - mLastOpen[bank]);
    mLastOpen[bank] = mCycles;
    mCycles += tRCD;
}

// Close the bank unconditionally. Use -1 for all bank simultaneously.
- (void)closeBank:(int)bank {
    if (bank == -1) {
	// Ignore tRAS here.
	for (int i = 0; i < BANK_COUNT; i++) {
	    mOpenRow[i] = -1;
	}
    } else {
	mCycles += MIN(tRAS, mCycles - mLastOpen[bank]);
	mOpenRow[bank] = -1;
    }

    mCycles += tRP;
}

- (void)read:(int)count bank:(int)bank row:(int)row column:(int)column {
    [self openRow:row onBank:bank];

    mCycles += count + CAS_LATENCY;
}

- (void)write:(int)count bank:(int)bank row:(int)row column:(int)column {
    [self openRow:row onBank:bank];

    mCycles += count; // Writes have no latency.
}

@end
