#include<Foundation/NSAutoreleasePool.h>
#include<Foundation/NSTimer.h>
#include<Foundation/NSArray.h>
#include<AppKit/NSPasteboard.h>
#include<unistd.h>

void lookup(const char* str);
void init_db(const char* path);

@interface WDKClipper : NSObject
- (void) timerFired:(NSTimer *) timer;
@end

@implementation WDKClipper
int previousChangeCount;
- (id) init;
{
	NSPasteboard *pboard = [NSPasteboard generalPasteboard];
	previousChangeCount = [pboard changeCount];
	return self;
}
- (void) timerFired:(NSTimer *)timer;
{
	NSPasteboard *pboard = [NSPasteboard generalPasteboard];
	int changeCount = [pboard changeCount];
	if ( changeCount <= previousChangeCount )
		return;
	previousChangeCount = changeCount;

	id string = nil;
	NSArray *supportedTypes = [NSArray 	arrayWithObject:NSStringPboardType];
	NSString *bestType = [pboard availableTypeFromArray: supportedTypes];

    if ( [bestType isEqualToString:NSStringPboardType] )
    {
        string = [pboard stringForType:NSStringPboardType];
        lookup([string UTF8String]);
    }
}
@end

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	WDKClipper *waClip = [[WDKClipper new] autorelease];

	/*NSStatusBar *sBar = [NSStatusBar systemStatusBar];
	NSStatusItem *sItem = [sBar statusItemWithLength:NSVariableStatusItemLength];
	[sItem retain];
	[sItem setTitle: @"Wadoku"];
	[sItem setHighlightMode:YES];
	[sItem setMenu:sbMenu];*/

	init_db(NULL,false);

    while(true) {
        [waClip timerFired:nil];
		usleep(500);
	}
	[pool drain];
	return 0;
}
