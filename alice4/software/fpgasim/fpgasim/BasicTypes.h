//
//  BasicTypes.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/14/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#ifndef BasicTypes_h
#define BasicTypes_h

typedef unsigned char vec3ub[3];

typedef struct {
    unsigned short x, y;
    unsigned long z;
    unsigned char r, g, b, a;
} screen_vertex;

#endif /* BasicTypes_h */
