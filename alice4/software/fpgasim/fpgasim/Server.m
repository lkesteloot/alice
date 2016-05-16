//
//  Server.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>

#import "Server.h"

#define PORT 25423
#define MAX_BUFFER 128

typedef enum {
    STATE_COMMAND,              // Expecting command byte.
    STATE_WINOPEN_LENGTH,	// Expecting length of wintitle string.
    STATE_WINOPEN_TITLE,	// Expecting next title byte.
    STATE_CLEAR,		// Expecting clear color.
    STATE_TRIANGLE,		// Expecting three vertices.
} State;

// Sent on the wire:
typedef enum {
    COMMAND_WINOPEN = 0x00,
    COMMAND_CLEAR = 0x01,
    COMMAND_SWAPBUFFERS = 0x02,
    COMMAND_TRIANGLE = 0x03,
} Command;

@interface Server () {
    // For getting bytes from client.
    unsigned char buffer[MAX_BUFFER];
    int bufferLength;
    int bytesLeft;
}

@property (nonatomic) id<ServerDelegate> delegate;
@property (nonatomic) NSInputStream *inputStream;
@property (nonatomic) NSOutputStream *outputStream;
@property (nonatomic) State state;

@end

@implementation Server

- (id)initWithDelegate:(id<ServerDelegate>)delegate {
    self = [super init];

    if (self) {
	_delegate = delegate;
	_inputStream = nil;
	_outputStream = nil;
	_state = STATE_COMMAND;
	bufferLength = 0;
	bytesLeft = 0;

	[self startServer];
    }

    return self;
}

void handleConnect(CFSocketRef s, CFSocketCallBackType callbackType, CFDataRef address, const void *data, void *info) {
    Server *server = (__bridge Server *) info;
    [server connected:callbackType data:data];
}

- (void)startServer {
    CFSocketContext context = { 0, (__bridge void *) self, NULL, NULL, NULL };

    // Create the socket.
    CFSocketRef myipv4cfsock = CFSocketCreate(
					      kCFAllocatorDefault,
					      PF_INET,
					      SOCK_STREAM,
					      IPPROTO_TCP,
					      kCFSocketAcceptCallBack, handleConnect, &context);
    CFSocketRef myipv6cfsock = CFSocketCreate(
					      kCFAllocatorDefault,
					      PF_INET6,
					      SOCK_STREAM,
					      IPPROTO_TCP,
					      kCFSocketAcceptCallBack, handleConnect, &context);

    // Bind the socket.
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_len = sizeof(sin);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr= INADDR_ANY;

    CFDataRef sincfd = CFDataCreate(
				    kCFAllocatorDefault,
				    (UInt8 *)&sin,
				    sizeof(sin));

    // Make it so that we can bind quickly after killing the app.
    CFSocketNativeHandle native4 = CFSocketGetNative(myipv4cfsock);
    if (native4 >= 0) {
	int yes = 1;
	setsockopt(native4, SOL_SOCKET, SO_REUSEADDR, (void *) &yes, sizeof(yes));
    }
    CFSocketError error = CFSocketSetAddress(myipv4cfsock, sincfd);
    if (error == kCFSocketError) {
	NSLog(@"Can't bind IP4");
	return;
    }
    CFRelease(sincfd);

    struct sockaddr_in6 sin6;

    memset(&sin6, 0, sizeof(sin6));
    sin6.sin6_len = sizeof(sin6);
    sin6.sin6_family = AF_INET6;
    sin6.sin6_port = htons(PORT);
    sin6.sin6_addr = in6addr_any;

    CFDataRef sin6cfd = CFDataCreate(
				     kCFAllocatorDefault,
				     (UInt8 *)&sin6,
				     sizeof(sin6));
    
    error = CFSocketSetAddress(myipv6cfsock, sin6cfd);
    if (error == kCFSocketError) {
	NSLog(@"Can't bind IP6");
	return;
    }
    CFRelease(sin6cfd);

    // Add ourselves to the run loop.
    CFRunLoopSourceRef socketsource = CFSocketCreateRunLoopSource(
								  kCFAllocatorDefault,
								  myipv4cfsock,
								  0);

    CFRunLoopAddSource(
		       CFRunLoopGetCurrent(),
		       socketsource,
		       kCFRunLoopDefaultMode);

    CFRunLoopSourceRef socketsource6 = CFSocketCreateRunLoopSource(
								   kCFAllocatorDefault,
								   myipv6cfsock,
								   0);

    CFRunLoopAddSource(
		       CFRunLoopGetCurrent(),
		       socketsource6,
		       kCFRunLoopDefaultMode);
}

- (void)connected:(CFSocketCallBackType)callbackType data:(const void *)data {
    switch (callbackType) {
	case kCFSocketNoCallBack:
	    NSLog(@"connected none");
	    break;

	case kCFSocketReadCallBack:
	    NSLog(@"connected read");
	    break;

	case kCFSocketAcceptCallBack:
	    NSLog(@"connected accept");
	    [self accept:*(CFSocketNativeHandle *)data];
	    break;

	case kCFSocketDataCallBack:
	    NSLog(@"connected data");
	    break;

	case kCFSocketConnectCallBack:
	    NSLog(@"connected connect");
	    break;

	case kCFSocketWriteCallBack:
	    NSLog(@"connected write");
	    break;

	default:
	    NSLog(@"connected unknown");
	    break;
    }
}

- (void)accept:(CFSocketNativeHandle)nativeSocketHandle {
    uint8_t peerName[SOCK_MAXADDRLEN];
    socklen_t namelen = sizeof(peerName);
    NSData *peer = nil;

    if (getpeername(nativeSocketHandle, (struct sockaddr *)peerName, &namelen) == 0) {
	peer = [NSData dataWithBytes:peerName length:namelen];
    }

    CFReadStreamRef readStream = NULL;
    CFWriteStreamRef writeStream = NULL;
    CFStreamCreatePairWithSocket(kCFAllocatorDefault, nativeSocketHandle, &readStream, &writeStream);

    if (readStream && writeStream) {
	CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
	CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
	[self startConnectionInput:(__bridge NSInputStream *)(readStream) output:(__bridge NSOutputStream *)(writeStream)];
    } else {
	NSLog(@"Could not accept");
	close(nativeSocketHandle);
    }

    if (readStream) {
	CFRelease(readStream);
    }

    if (writeStream) {
	CFRelease(writeStream);
    }
}

- (void)startConnectionInput:(NSInputStream *)istream output:(NSOutputStream *)ostream {
    [istream setDelegate:self];
    [ostream setDelegate:self];
    [istream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:(id)kCFRunLoopCommonModes];
    [ostream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:(id)kCFRunLoopCommonModes];
    [istream open];
    [ostream open];
}

// For NSStreamDelegate:
- (void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)eventCode {
    if (eventCode & NSStreamEventOpenCompleted) {
	NSLog(@"stream open");
    }
    if (eventCode & NSStreamEventHasBytesAvailable) {
	// NSLog(@"stream read");
	uint8_t receiveBuffer[128];
	NSInteger amount = [(NSInputStream *)stream read:receiveBuffer maxLength:sizeof(receiveBuffer)];
	[self handleBytes:receiveBuffer ofLength:amount];
    }
    if (eventCode & NSStreamEventHasSpaceAvailable) {
	NSLog(@"stream write");
    }
    if (eventCode & NSStreamEventErrorOccurred) {
	NSLog(@"stream error");
    }
    if (eventCode & NSStreamEventEndEncountered) {
	NSLog(@"stream end");
    }
}

- (void)handleBytes:(uint8_t *)receiveBuffer ofLength:(NSInteger)len {
    for (NSInteger i = 0; i < len; i++) {
	[self handleByte:receiveBuffer[i]];
    }
}

- (void)handleByte:(uint8_t)b {
    // See if we're receiving a buffer.
    if (bytesLeft > 0) {
	if (bufferLength < MAX_BUFFER) {
	    buffer[bufferLength++] = b;
	    bytesLeft--;
	    if (bytesLeft > 0) {
		// Nothing to do right now.
		return;
	    }

	    // Fallthrough to state machine.
	} else {
	    NSLog(@"Exceeded buffer length for state %d", self.state);
	    self.state = STATE_COMMAND;
	    return;
	}
    }

    switch (self.state) {
	case STATE_COMMAND:
	    // Initial command.
	    // NSLog(@"Got command: %02x", (int)b);
	    switch (b) {
		case COMMAND_WINOPEN:
		    self.state = STATE_WINOPEN_LENGTH;
		    break;

		case COMMAND_CLEAR:
		    [self expectBytes:3 forState:STATE_CLEAR];
		    break;

		case COMMAND_SWAPBUFFERS:
		    [self.delegate swapBuffers];
		    break;

		case COMMAND_TRIANGLE:
		    [self expectBytes:12 forState:STATE_TRIANGLE];
		    break;

		default:
		    // Problem. Reset.
		    NSLog(@"Got unknown command byte %02x", (int)b);
		    self.state = STATE_COMMAND;
		    break;
	    }
	    break;

	case STATE_WINOPEN_LENGTH:
	    if (b == 0) {
		[self.delegate setWindowTitle:@""];
		self.state = STATE_COMMAND;
	    } else {
		[self expectBytes:b forState:STATE_WINOPEN_TITLE];
	    }
	    break;

	case STATE_WINOPEN_TITLE:
	    [self.delegate setWindowTitle:[self bufferAsString]];
	    self.state = STATE_COMMAND;
	    break;

	case STATE_CLEAR:
	    [self.delegate clear:buffer];
	    self.state = STATE_COMMAND;
	    break;

	case STATE_TRIANGLE:
	    [self gotTriangle];
	    self.state = STATE_COMMAND;
	    break;

	default:
	    NSLog(@"In unknown state %d", self.state);
	    self.state = STATE_COMMAND;
	    break;
    }
}

- (void)gotTriangle {
    // Unpack the triangle data.
    screen_vertex v[3];

    for (int i = 0; i < 3; i++) {
	[self unpackScreenVertex:&v[i] fromBuffer:&buffer[i*4]];
    }

    [self.delegate triangle:v];
}

- (void)unpackScreenVertex:(screen_vertex *)v fromBuffer:(unsigned char *)b {
    v->x = b[0] + b[1]*256;
    v->y = b[2] + b[3]*256;
}

// Start expecting a fixed number of bytes.
- (void)expectBytes:(int)byteCount forState:(State)newState {
    self.state = newState;
    bytesLeft = byteCount;
    bufferLength = 0;
}

// Returns the buffer as an ASCII string.
- (NSString *)bufferAsString {
    return [[NSString alloc] initWithBytes:buffer length:bufferLength encoding:NSASCIIStringEncoding];
}

@end
