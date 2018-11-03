
#include "hdr/system/sys_utils.h"

typedef struct
{
    float       x;
    float       y;
    float       width;
    float       height;
} _myBoundingBox;

//----------------------------------------------------------------------
//
// Is an object visible on the screen
bool sys_visibleOnScreen ( cpVect worldCoord, int shapeSize )
//----------------------------------------------------------------------
{
    _myBoundingBox	screenArea;

    if ( ( worldCoord.x < 0 ) || ( worldCoord.y < 0 ) )
        return false;

    screenArea.x = playerDroid.worldPos.x - ( shapeSize * 2 );
screenArea.y = playerDroid.worldPos.y - ( shapeSize * 2 );
    screenArea.width = winWidth + shapeSize;
    screenArea.height = winHeight + shapeSize;

    if ( worldCoord.x < screenArea.x )
        return false;

    if ( worldCoord.y < screenArea.y )
        return false;

    if ( worldCoord.x > screenArea.x + ( screenArea.width + ( shapeSize * 2 ) ) )
        return false;

    if ( worldCoord.y > screenArea.y + ( screenArea.height + ( shapeSize * 2 ) ) )
        return false;

    return true;
}
