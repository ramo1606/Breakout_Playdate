#include "level.h"

#define LEVELS_AMMOUNT 18

static char* levels[LEVELS_AMMOUNT] = {
    //01 simple breakout
    "///xb8xxb8xxbpbbpbbpbxxb8xxb8x"

    //02 stairway to heaven
    ,"/b/bb/bbp/b3/b4/b4p/b6/bsb5/b7p/h9s"

    //03 invader
    ,"xxsxxxxxs/xxsxxxxxs/xxxsxxxs/xxxbbbbbxxxxxbbbbbbbxxxbbbbbbbbbxxbbpbbbpbbxxbbbbbbbbbxxbbbbbbbbbxxbxbxxxbxbxxbxbxxxbxbxxxxxbxb/"

    //04 twin towers
    ,"//xbbbhxbbbhxxbphbxbphbxxbhpbxbhpbxxhbbbxhbbbxxbbbbxbbbbxxbbbhxbbbhxxbphbxbphbxxbhpbxbhpbxxhbbbxhbbbx"

    //05 get inside
    ,"pi//xi/xixxxhh/xixxhbbh/xixhpbbph/xixhbssbh/xixxhbbh/xixxxhh/xi/xi/xi/si9"

    //08 cups high
    ,"/xixixxxixixxixixxxixixxisixxxisixxiiixxxiiix/xxxbbpbb/xxxbbbbb/xxxpbpbp/xxxbbbbb/xxxbbpbb/"

    //14 clogged lanes
    ,"//pxpxpxpxpxpbxbxbxbxbxbbxbxbxbxbxbbxbxbxbxbxbbxisisisixbbxbxbxbxbxbbxbxbxbxbxbsxixsxsxixsbxbxbxbxbxb"

    //16 enegry core
    ,"//xibpbpbpbixxixxxxxxxixxixxxxxxxixxixiiiiixixxixibsbixixxixibsbixixxixibpbixixxixxxxxxxixxixxxxxxxixxiiiiiiiiix"

    //11 oreo
    ,"///xi8xxbbpbbbpbbxxbbbbbbbbbxxpbbbpbbbpxxbbbbbbbbbxxbbpbbbpbbxxi8x"

    //17 shelves
    ,"//xbxbxpxbxbxxixixixixixpxbxbxbxbxpixixixixixixbxpxsxpxbxxixixixixixbxbxbxbxbxbixixixixixixpxbxpxbxpxxixixixixix"

    //12 border wall
    ,"/bbbsbbbsbbbpxxxxxxxxxpb9b/biiiixiiiibxpxxxpxxxpxhiiiiiiiiih/bbpbbbbbpbbpxxxxxxxxxpb9b"

    //10 mellow center
    ,"/ph8pbxxxxxxxxxbbxhhhphhhxbbxhxxxxxhxbbxhxhhhxhxbbxpxhshxpxbbxhxhhhxhxbbxhxxxxxhxbbxhhhhhhhxbpxxxxxxxxxph9h"

    //13 lungs
    ,"///bbbpixipbbbhbbiixiibbhbbbsixisbbbbpbsixisbpbihiiixiiihibbbpixipbbbbbbbixibbbb"

    //15 diagonal
    ,"///bb/bbbb/bbbbbbb/bsbbbbbbb/pbpbbbbbbbbihbbpbpbbbbxxihbbbbpbbxxxxihibbsbxxxxxxxhibpxxxxxxxxxhi"

    //18 lazy devs
    ,"//xxxxbpb/xbpbihibpbxbihixxxihibbixxxxxxxibpibxxsxxbipbibxxxxxbibxbibxxxbibxxbibbbbbibxxxbibbbib/xxxbihib/"
};

char* LEVEL_get(int level)
{
    char* return_level = levels[0];
    if (level < LEVELS_AMMOUNT) 
    {
        return_level = levels[level];
    }
    return return_level;
}

int LEVEL_getAmount()
{
    return LEVELS_AMMOUNT;
}
