/** 
   Copyright (C) 2010 Free Software Foundation, Inc.
   
   Written by:  Richard Frith-Macdonald <rfm@gnu.org>
   Date:	September 2010
   
   This file is part of the WebServer Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA.

   $Date: 2010-09-17 16:47:13 +0100 (Fri, 17 Sep 2010) $ $Revision: 31364 $
   */ 

#import	<Foundation/NSAutoreleasePool.h>
#import	<Foundation/NSData.h>
#import	<Foundation/NSDebug.h>
#import	<Foundation/NSException.h>
#import	<Foundation/NSFileHandle.h>
#import	<Foundation/NSObject.h>
#import	<Foundation/NSNotification.h>
#import	<Foundation/NSRunLoop.h>
#import	<Foundation/NSString.h>
#import	<Foundation/NSThread.h>
#import	<Foundation/NSTimer.h>
#import	<GNUstepBase/GSMime.h>

@class	WebServer;
@class	WebServerConfig;
@class	WebServerConnection;
@class	WebServerResponse;

/* This class is used to hold configuration information needed by a single
 * connection ... once set up an instance is never modified so it can be
 * shared between threads.  When configuration is modified, it is replaced
 * by a new instance.
 */
@interface	WebServerConfig: NSObject
{
@public
  BOOL			verbose;	// logging type is detailed/verbose
  BOOL			durations;	// log request and connection times
  BOOL                  reverse;	// should do reverse DNS lookup
  BOOL			secureProxy;	// using a secure proxy
  NSUInteger		maxBodySize;
  NSUInteger		maxRequestSize;
  NSUInteger		maxConnectionRequests;
  NSTimeInterval	maxConnectionDuration;
  NSTimeInterval	connectionTimeout;
}
@end

/* We need to ensure that our map table holds response information safely
 * and efficiently ... so we use a subclass where we control -hash and
 * -isEqual: to ensure that each object is unique and quick.
 * We also store a pointer to the owning connection so that we can find
 * the connections from the response really quickly.
 */
@interface	WebServerResponse : GSMimeDocument
{
  WebServerConnection	*webServerConnection;
}
- (void) setWebServerConnection: (WebServerConnection*)c;
- (WebServerConnection*) webServerConnection;
@end


@interface	WebServerConnection : NSObject
{
  NSNotificationCenter	*nc;
  NSTimer		*ticker;
  WebServer		*server;
  WebServerResponse	*response;
  WebServerConfig	*conf;
  NSString		*address;	// Client address
  NSString		*command;	// Command sent by client
  NSString		*agent;		// User-Agent header
  NSString		*result;	// Result sent back
  NSString		*user;		// The remote user
  NSFileHandle		*handle;
  GSMimeParser		*parser;
  NSMutableData		*buffer;
  NSData		*excess;
  NSUInteger		byteCount;
  NSUInteger		identity;
  NSTimeInterval	ticked;
  NSTimeInterval	extended;
  NSTimeInterval	requestStart;
  NSTimeInterval	connectionStart;
  NSTimeInterval	duration;
  NSUInteger		requests;
  BOOL			processing;
  BOOL			shouldClose;
  BOOL			hasReset;
  BOOL			simple;
  BOOL			quiet;		// Suppress log of warning/debug info?
  BOOL			ssl;		// Should perform SSL negotiation?
  BOOL			handshake;	// Currently in SSL handshake?
}
- (NSString*) address;
- (NSString*) audit;
- (NSTimeInterval) connectionDuration: (NSTimeInterval)now;
- (void) end;
- (BOOL) ended;
- (NSData*) excess;
- (void) extend: (NSTimeInterval)when;
- (NSFileHandle*) handle;
- (BOOL) hasReset;
- (NSUInteger) identity;
- (id) initWithHandle: (NSFileHandle*)hdl
		  for: (WebServer*)svr
	      address: (NSString*)adr
	       config: (WebServerConfig*)c
		quiet: (BOOL)q
		  ssl: (BOOL)s
	      refusal: (NSString*)r;
- (NSUInteger) moreBytes: (NSUInteger)count;
- (GSMimeParser*) parser;
- (BOOL) processing;
- (BOOL) quiet;
- (GSMimeDocument*) request;
- (NSTimeInterval) requestDuration: (NSTimeInterval)now;
- (void) reset;
- (void) respond;
- (WebServerResponse*) response;
- (void) setAddress: (NSString*)aString;
- (void) setAgent: (NSString*)aString;
- (void) setConnectionStart: (NSTimeInterval)when;
- (void) setExcess: (NSData*)d;
- (void) setParser: (GSMimeParser*)aParser;
- (void) setProcessing: (BOOL)aFlag;
- (void) setRequestEnd: (NSTimeInterval)when;
- (void) setRequestStart: (NSTimeInterval)when;
- (void) setResult: (NSString*)aString;
- (void) setShouldClose: (BOOL)aFlag;
- (void) setSimple: (BOOL)aFlag;
- (void) setTicked: (NSTimeInterval)when;
- (void) setUser: (NSString*)aString;
- (BOOL) shouldClose;
- (void) start;
- (NSTimeInterval) ticked;
- (void) timeout: (NSTimer*)t;

- (void) _didData: (NSData*)d;
- (void) _didRead: (NSNotification*)notification;
- (void) _didWrite: (NSNotification*)notification;
@end

@interface	WebServer (Internal)
- (void) _alert: (NSString*)fmt, ...;
- (void) _audit: (WebServerConnection*)connection;
- (void) _didConnect: (NSNotification*)notification;
- (void) _endConnect: (WebServerConnection*)connection;
- (void) _listen;
- (void) _log: (NSString*)fmt, ...;
- (void) _process1: (WebServerConnection*)connection;
- (void) _process2: (WebServerConnection*)connection;
- (void) _removeConnection: (WebServerConnection*)connection;
- (void) _runConnection: (WebServerConnection*)connection;
- (void) _threadReadFrom: (NSFileHandle*)handle;
- (void) _threadWrite: (NSData*)data to: (NSFileHandle*)handle;
@end
