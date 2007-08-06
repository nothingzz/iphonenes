//
//  FileBrowser.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/04/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface FileBrowser : UIView 
{
	NSMutableArray *_extensions;
	NSMutableArray *_files;
	UITable *_table;
	NSString *_path;
	int _rowCount;
	id _delegate;
}

- (id)initWithFrame:(CGRect)rect;
- (NSString *)path;
- (void)setPath: (NSString *)path;
- (void)reloadData;
- (void)setDelegate:(id)delegate;
- (int)numberOfRowsInTable:(UITable *)table;
- (UITableCell *)table:(UITable *)table cellForRow:(int)row column:(UITableColumn *)col;
- (void)tableRowSelected:(NSNotification *)notification;
- (NSString *)selectedFile;

@end
