#include<Foundation/NSTask.h>
#include<Foundation/NSArray.h>
#include<Foundation/NSString.h>

void notify(const char* ctitle, const char* ctext) {
	NSString* title = [NSString stringWithUTF8String:ctitle];
	NSString *text = [NSString stringWithUTF8String:ctext];
	NSTask *task = [NSTask new];
	[task setLaunchPath:@"/usr/local/bin/growlnotify"];
	[task setArguments:[NSArray arrayWithObjects:@"--image",@"./wadoku.png",@"-m",text,@"-t",title,nil]];
	[task launch];
	[task release];
	[title release];
	[text release];
}