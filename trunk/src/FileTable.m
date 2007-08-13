/*

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2
 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#import "FileTable.h"

@implementation FileTable

- (int)swipe:(int)type withEvent:(struct __GSEvent *)event;
{
    if (_allowDelete == YES) {
        CGRect rect = GSEventGetLocationInWindow(event);
        CGPoint point = CGPointMake(rect.origin.x, rect.origin.y - 45);
        CGPoint offset = _startOffset; 
        LOGDEBUG("FileTable.swipe: %d %f, %f", type, point.x, point.y);

        point.x += offset.x;
        point.y += offset.y;
        int row = [ self rowAtPoint:point ];

        [ [ self visibleCellForRow:row column:0] 
           _showDeleteOrInsertion:YES 
           withDisclosure:NO
           animated:YES 
           isDelete:YES 
           andRemoveConfirmation:YES
        ];

        return [super swipe:type withEvent:event];
    }
}

- (void)allowDelete:(BOOL)allow {
    _allowDelete = allow;
}

@end

@implementation UIDeletableCell

- (void)removeControlWillHideRemoveConfirmation:(id)fp8
{
    [ self _showDeleteOrInsertion:NO
          withDisclosure:NO
          animated:YES
          isDelete:YES
          andRemoveConfirmation:YES
    ];
}

- (void)_willBeDeleted
{
    NSString *file = [_path stringByAppendingPathComponent: [_files objectAtIndex:[ _table _rowForTableCell:self]]];
    char *fn = [file cStringUsingEncoding: NSASCIIStringEncoding];

    LOGDEBUG("UIDeletableCell._willBeDeleted: %s", fn);
 
    if (!strcmp(fn + (strlen(fn)-4), ".sav"))
        unlink(fn);

#ifdef DELETE_ROMS
    if (!strcasecmp(fn + (strlen(fn)-4), ".nes"))
        unlink(fn);
#endif
    
    
}

- (void)setTable:(UITable *)table {
    _table = table;
}

- (void)setFiles:(NSMutableArray *)files {
    _files = files;
}

- (NSString *)path {
        return [[_path retain] autorelease];
}

- (void)setPath: (NSString *)path {
        [_path release];
        _path = [path copy];
}

@end

