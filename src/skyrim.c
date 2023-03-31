#include "global.h"
#include "main.h"
#include "string_util.h"
#include "decompress.h"
#include "graphics.h"
#include "palette.h"
#include "util.h"
#include "text.h"
#include "menu.h"
#include "malloc.h"
#include "bg.h"
#include "sprite.h"
#include "strings.h"
#include "text_window.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "trainer_pokemon_sprites.h"
#include "constants/songs.h"
#include "constants/rgb.h"

#define TAG_TITLE_PRESENTS  1500
#define TAG_TITLE_ELDER     1501
#define TAG_TITLE_SKYRIM    1502
#define TAG_TITLE           2000

// char |        0       |           1         |            2          |            3          |
//  map | 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 1,
        .priority = 1,
        .baseTile = 0,
    },
    {
        .bg = 2,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 1,
        .paletteMode = 1,
        .priority = 2,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sTextWindows[] =
{
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 27,
        .height = 4,
        .paletteNum = 0,
        .baseBlock = 404
    },
};

static const struct Subsprite sSubsprites_Presents[] = {
	{ .x=-64, .y=-8, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset= 0, .priority=0 },
	{ .x=-32, .y=-8, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset= 8, .priority=0 },
	{ .x=  0, .y=-8, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset=16, .priority=0 },
	{ .x= 32, .y=-8, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset=24, .priority=0 },
	{ .x=-32, .y= 8, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset=32, .priority=0 },
	{ .x=  0, .y= 8, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset=40, .priority=0 },
};

static const struct Subsprite sSubsprites_ElderScrolls[] = {
	{ .x=-64, .y=0, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset= 0, .priority=0 },
	{ .x=-32, .y=0, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset= 8, .priority=0 },
	{ .x=  0, .y=0, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset=16, .priority=0 },
	{ .x= 32, .y=0, .shape=SPRITE_SHAPE(32x16), .size=SPRITE_SIZE(32x16), .tileOffset=24, .priority=0 },
};

static const struct Subsprite sSubsprites_Skyrim[] = {
	{ .x=-64, .y=-8, .shape=SPRITE_SHAPE(64x32), .size=SPRITE_SIZE(64x32), .tileOffset= 0, .priority=0 },
	{ .x=  0, .y=-8, .shape=SPRITE_SHAPE(64x32), .size=SPRITE_SIZE(64x32), .tileOffset=32, .priority=0 },
};

static const struct SubspriteTable sSubspriteTable_Presents[] = {
	ARRAY_COUNT(sSubsprites_Presents), sSubsprites_Presents
};
static const struct SubspriteTable sSubspriteTable_ElderScrolls[] = {
	ARRAY_COUNT(sSubsprites_ElderScrolls), sSubsprites_ElderScrolls
};
static const struct SubspriteTable sSubspriteTable_Skyrim[] = {
	ARRAY_COUNT(sSubsprites_Skyrim), sSubsprites_Skyrim
};

static const struct OamData sOamData_Titles =
{
    .x = 0, .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_BLEND,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct SpriteTemplate sSpriteTemplate_Title =
{
    .tileTag = TAG_TITLE_PRESENTS,
    .paletteTag = TAG_TITLE,
    .oam = &sOamData_Titles,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};


const u32 sGfx_bg[] = INCBIN_U32("graphics/skyrim/skyrim_bg.8bpp.lz");
const u32 sGfx_fg[] = INCBIN_U32("graphics/skyrim/skyrim_fg.8bpp.lz");

const u32 sMap_bg[]  = INCBIN_U32("graphics/skyrim/skyrim_bg.bin.lz");
const u32 sMap_fg0[] = INCBIN_U32("graphics/skyrim/skyrim_fg0.bin.lz");
const u32 sMap_fg1[] = INCBIN_U32("graphics/skyrim/skyrim_fg1.bin.lz");

const u32 sPal_skyrim[] = INCBIN_U32("graphics/skyrim/skyrim.gbapal.lz");

// const u32 gSkyrimGfx[] = INCBIN_U32("graphics/skyrim/test.8bpp.lz");
// const u32 gSkyrimMap[] = INCBIN_U32("graphics/skyrim/test.tilemap.lz");
// const u32 gSkyrimPal[] = INCBIN_U32("graphics/skyrim/test.gbapal.lz");

const u32 sGfx_TitlePresents[] = INCBIN_U32("graphics/skyrim/presents.4bpp.lz");
const u32 sGfx_TitleElderScrolls[] = INCBIN_U32("graphics/skyrim/elder.4bpp.lz");
const u32 sGfx_TitleSkyrim[] = INCBIN_U32("graphics/skyrim/title.4bpp.lz");
const u16 sPal_Title[] = INCBIN_U16("graphics/skyrim/title.gbapal");

static const struct CompressedSpriteSheet sSpriteSheet_Title[] =
{
    {sGfx_TitlePresents, 0x600, TAG_TITLE_PRESENTS},
    {sGfx_TitleElderScrolls, 0x400, TAG_TITLE_ELDER},
    {sGfx_TitleSkyrim, 0x800, TAG_TITLE_SKYRIM},
    {},
};
static const struct SpritePalette sSpritePalette_Title[] =
{
    {sPal_Title, TAG_TITLE}, 
    {},
};


//-----------------------------------------------------------------------------

static void Task_Skyrim_Init(u8 taskId);
static void Task_Skyrim_RunBackground(u8 taskId);
static void Skyrim_GoToNextStage(u8 taskId);

EWRAM_DATA static s16 sBgXoff = 0;
EWRAM_DATA static s16 sBgYoff = 0;

static void CB2_Skyrim(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_Skyrim(void)
{
	u16 off;
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
	
	off = Q_8_8_TO_INT(sBgXoff);
    SetGpuReg(REG_OFFSET_BG2HOFS, off);
    // SetGpuReg(REG_OFFSET_BG1HOFS, 0);
	
	off = Q_8_8_TO_INT(sBgYoff);
    SetGpuReg(REG_OFFSET_BG2VOFS, off);
    SetGpuReg(REG_OFFSET_BG1VOFS, off);
}



void CB2_InitSkyrim(void)
{
	// u8 taskId;
    // u8 spriteId;
    u16 savedIme;
    u32 i;

    ResetBgsAndClearDma3BusyFlags(0);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    SetVBlankCallback(NULL);
	
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, -2);
	
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
	
    ResetPaletteFade();
	
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    
    ResetAllPicSprites();
    
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    savedIme = REG_IME;
    REG_IME = 0;
    REG_IE |= 1;
    REG_IME = savedIme;
	
    InitWindows(sTextWindows);
    // LoadMainMenuWindowFrameTiles(0, 0xF3);
    // LoadMessageBoxGfx(0, 0xFC, BG_PLTT_ID(15));
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
    
	LZ77UnCompVram(sGfx_fg, (u8*) BG_CHAR_ADDR(0));
	LZ77UnCompVram(sGfx_bg, (u8*) BG_CHAR_ADDR(1));
	LZ77UnCompVram(sMap_fg0, (u8*) BG_SCREEN_ADDR(29));
	LZ77UnCompVram(sMap_bg,  (u8*) BG_SCREEN_ADDR(30));
	LoadCompressedPalette(sPal_skyrim, BG_PLTT_ID(0), PLTT_SIZE_8BPP);
    
    for (i = 0; i < ARRAY_COUNT(sSpriteSheet_Title) - 1; i++)
        LoadCompressedSpriteSheet(&sSpriteSheet_Title[i]);
    LoadSpritePalettes(sSpritePalette_Title);
    
    BeginNormalPaletteFade(PALETTES_BG, 0, 16, 16, RGB_BLACK);
    
    SetVBlankCallback(VBlankCB_Skyrim);
    SetMainCallback2(CB2_Skyrim);
	
	CreateTask(Task_Skyrim_Init, 0);
}


//-----------------------------------------------------------------------------
#pragma region 

#define PAL_TEXT        0xFFFF0000
#define PAL_BACKGROUND  0x0000FFFF

#define tMainTask data[0]
#define tAlphaCoeff1 data[1]
#define tAlphaCoeff2 data[2]
#define tDelay data[3]
#define tDelayTimer data[4]

#define tTimer data[0]
#define tIsDoneFadingSprites data[1]
#define tCurrSprite data[2]

static void Task_Skyrim_RunBackground(u8 taskId)
{
	sBgXoff -= Q_8_8(0.05);
}

//-----------------------------------------------------------------------------

static void Task_Skyrim_FadeSpritesOut(u8 taskId)
{
	int alphaCoeff2;

    if (gTasks[taskId].tAlphaCoeff1 == 0)
    {
        gTasks[gTasks[taskId].tMainTask].tIsDoneFadingSprites = TRUE;
        gSprites[gTasks[gTasks[taskId].tMainTask].tCurrSprite].invisible = TRUE;
        DestroyTask(taskId);
    }
    else if (gTasks[taskId].tDelayTimer)
    {
        gTasks[taskId].tDelayTimer--;
    }
    else
    {
        gTasks[taskId].tDelayTimer = gTasks[taskId].tDelay;
        gTasks[taskId].tAlphaCoeff1--;
        gTasks[taskId].tAlphaCoeff2++;
        alphaCoeff2 = gTasks[taskId].tAlphaCoeff2 << 8;
        SetGpuReg(REG_OFFSET_BLDALPHA, gTasks[taskId].tAlphaCoeff1 + alphaCoeff2);
    }
}
static void Skyrim_StartSpriteFadeOut(u8 taskId, u8 delay)
{
    u8 taskId2;

    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG_ALL | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_OBJ);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
    SetGpuReg(REG_OFFSET_BLDY, 0);
    gTasks[taskId].tIsDoneFadingSprites = FALSE;
    taskId2 = CreateTask(Task_Skyrim_FadeSpritesOut, 0);
    gTasks[taskId2].tMainTask = taskId;
    gTasks[taskId2].tAlphaCoeff1 = 16;
    gTasks[taskId2].tAlphaCoeff2 = 0;
    gTasks[taskId2].tDelay = delay;
    gTasks[taskId2].tDelayTimer = delay;

}

//-----------------------------------------------------------------------------

static void Task_Skyrim_FadeSpritesIn(u8 taskId)
{
    int alphaCoeff2;

    if (gTasks[taskId].tAlphaCoeff1 == 16)
    {
        gTasks[gTasks[taskId].tMainTask].tIsDoneFadingSprites = TRUE;
        DestroyTask(taskId);
    }
    else if (gTasks[taskId].tDelayTimer)
    {
        gTasks[taskId].tDelayTimer--;
    }
    else
    {
        gTasks[taskId].tDelayTimer = gTasks[taskId].tDelay;
        gTasks[taskId].tAlphaCoeff1++;
        gTasks[taskId].tAlphaCoeff2--;
        alphaCoeff2 = gTasks[taskId].tAlphaCoeff2 << 8;
        SetGpuReg(REG_OFFSET_BLDALPHA, gTasks[taskId].tAlphaCoeff1 + alphaCoeff2);
    }
}
static void Skyrim_StartSpriteFadeIn(u8 taskId, u8 delay)
{
    u8 taskId2;

    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG_ALL | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_OBJ);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, 16));
    SetGpuReg(REG_OFFSET_BLDY, 0);
    gTasks[taskId].tIsDoneFadingSprites = 0;
    taskId2 = CreateTask(Task_Skyrim_FadeSpritesIn, 0);
    gTasks[taskId2].tMainTask = taskId;
    gTasks[taskId2].tAlphaCoeff1 = 0;
    gTasks[taskId2].tAlphaCoeff2 = 16;
    gTasks[taskId2].tDelay = delay;
    gTasks[taskId2].tDelayTimer = delay;
    gSprites[gTasks[gTasks[taskId].tMainTask].tCurrSprite].invisible = FALSE;
}

//-----------------------------------------------------------------------------

static void Task_Skyrim_Init(u8 taskId)
{
    u8 sid;
    
	sBgXoff = Q_8_8(0);
	sBgYoff = Q_8_8(-8);
    
    sid = CreateSprite(&sSpriteTemplate_Title, 120, 70, 0);
    SetSubspriteTables(&gSprites[sid], sSubspriteTable_Presents);
    gSprites[sid].invisible = TRUE;
    gTasks[taskId].tCurrSprite = sid;
	
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_WaitSpriteFade(u8 taskId)
{
	if (gTasks[taskId].tIsDoneFadingSprites) {
		Skyrim_GoToNextStage(taskId);
    }
}
static void Task_Skyrim_WaitPaletteFade(u8 taskId)
{
	if (!gPaletteFade.active) {
		Skyrim_GoToNextStage(taskId);
    }
}
static void Task_Skyrim_WaitMessage(u8 taskId)
{
	if (!RunTextPrintersAndIsPrinter0Active()) {
		Skyrim_GoToNextStage(taskId);
    }
}
static void Task_Skyrim_WaitMessageAndClear(u8 taskId)
{
	if (!RunTextPrintersAndIsPrinter0Active()) {
        FillWindowPixelBuffer(0, PIXEL_FILL(0));
        PutWindowTilemap(0);
        CopyWindowToVram(0, COPYWIN_GFX);
        
		Skyrim_GoToNextStage(taskId);
    }
}
static void Task_Skyrim_Delay(u8 taskId)
{
	gTasks[taskId].tTimer--;
	if (!gTasks[taskId].tTimer) {
		Skyrim_GoToNextStage(taskId);
    }
}

static void Task_Skyrim_StartFadeOutText(u8 taskId)
{
    Skyrim_StartSpriteFadeOut(taskId, 10);
	// BeginNormalPaletteFade(PAL_TEXT, 10, 0, 0x10, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}
static void Task_Skyrim_StartFadeOutBackground(u8 taskId)
{
	BeginNormalPaletteFade(PAL_BACKGROUND, 10, 0, 0x10, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}
static void Task_Skyrim_StartFadeOutAll(u8 taskId)
{
	BeginNormalPaletteFade(PALETTES_ALL, 20, 0, 0x10, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}

//TODO:
//  Bethesda Game Studios
//  presents
// <scene fade up slowly>
//  The Elder Scrolls V
//  Skyrim <big>

static void Task_Skyrim_DisplayPresents(u8 taskId)
{
    u8 sid;
    struct Sprite *sprite = &gSprites[gTasks[taskId].tCurrSprite];
    
	// load "Bethesda Game Studios presents" message to screen
    sprite->sheetTileStart = GetSpriteTileStartByTag(TAG_TITLE_PRESENTS);
    SetSpriteSheetFrameTileNum(sprite);
    SetSubspriteTables(sprite, sSubspriteTable_Presents);
	
    Skyrim_StartSpriteFadeIn(taskId, 10);
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_DisplayTitle(u8 taskId)
{
    u8 sid;
    struct Sprite *sprite = &gSprites[gTasks[taskId].tCurrSprite];
    
	// load "The Elder Scrolls V" message to screen
    sprite->sheetTileStart = GetSpriteTileStartByTag(TAG_TITLE_ELDER);
    SetSpriteSheetFrameTileNum(sprite);
    SetSubspriteTables(sprite, sSubspriteTable_ElderScrolls);
    
    Skyrim_StartSpriteFadeIn(taskId, 10);
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_DisplaySubtitle(u8 taskId)
{
    u8 sid;
    struct Sprite *sprite = &gSprites[gTasks[taskId].tCurrSprite];
    
	// TODO: load "Skyrim" title card to screen
    sprite->sheetTileStart = GetSpriteTileStartByTag(TAG_TITLE_SKYRIM);
    SetSpriteSheetFrameTileNum(sprite);
    SetSubspriteTables(sprite, sSubspriteTable_Skyrim);
    
    Skyrim_StartSpriteFadeIn(taskId, 10);
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_FadeInBackground(u8 taskId)
{
    CreateTask(Task_Skyrim_RunBackground, 5);
	BeginNormalPaletteFade(PALETTES_BG, 10, 0x10, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}


static void Task_Skyrim_ShiftBgUp(u8 taskId)
{
    if ((sBgYoff) < Q_8_8(24)) {
        sBgYoff += Q_8_8(0.5);
    } else {
	    Skyrim_GoToNextStage(taskId);
    }
}

extern const u8 gText_Skyrim_IntroLine1[];
static void Task_Skyrim_DisplayFinallyAwake(u8 taskId)
{
	// load "Hey you, you're finally awake." message to screen
	FillWindowPixelBuffer(0, PIXEL_FILL(0));
	AddTextPrinterParameterized(0, FONT_SKYRIM, gText_Skyrim_IntroLine1, 0, 1, 2, NULL);
	PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_GFX);
	
	// Switch background to guy facing us
	LZ77UnCompVram(sMap_fg1, (u8*) BG_SCREEN_ADDR(29));
	
	Skyrim_GoToNextStage(taskId);
}

extern const u8 gText_Skyrim_AprilFools[];
extern const u8 gText_Skyrim_ComingSoon[];
static void Task_Skyrim_DisplayAprilFools(u8 taskId)
{
    // TODO: Move message (BG0) to middle of screen
    SetGpuReg(REG_OFFSET_BG0VOFS, 60);
    
    // Note: We're hiding all the backgrounds here to have pure black.
    // This requires that the "skyrim.pal"'s index 0 color is black, as
    // palette index 0 controls what is displayed as the background color
    // when no tiles are present.
    HideBg(1);
    HideBg(2);
    
	// load "Happy April Fools from RainbowDevs" message to screen
    FillWindowPixelBuffer(0, PIXEL_FILL(0));
	AddTextPrinterParameterized(0, FONT_SKYRIM, gText_Skyrim_AprilFools, 0, 1, 0, NULL);
	PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_GFX);
    
	BeginNormalPaletteFade(PALETTES_ALL, 1, 16, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_DisplayComingSoon(u8 taskId)
{
    FillWindowPixelBuffer(0, PIXEL_FILL(0));
	AddTextPrinterParameterized(0, FONT_SKYRIM, gText_Skyrim_ComingSoon, 0, 1, 0, NULL);
	PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_GFX);
    Skyrim_GoToNextStage(taskId);
}



#pragma endregion
//-----------------------------------------------------------------------------

#define DO_FUNC(fn) gTasks[taskId].func = fn; break
#define DO_DELAY(time) gTasks[taskId].func = Task_Skyrim_Delay; gTasks[taskId].tTimer = time; break

static void Skyrim_GoToNextStage(u8 taskId)
{
	switch (gMain.state++) {
		case  0: DO_DELAY((2 * 60));
		case  1: DO_FUNC(Task_Skyrim_DisplayPresents);
		case  2: DO_FUNC(Task_Skyrim_WaitSpriteFade);
		case  3: DO_DELAY((4 * 60));
		case  4: DO_FUNC(Task_Skyrim_StartFadeOutText);
		case  5: DO_FUNC(Task_Skyrim_WaitSpriteFade);
		
		case 10: DO_DELAY((2 * 60));
		case 11: DO_FUNC(Task_Skyrim_DisplayTitle);
		case 12: DO_FUNC(Task_Skyrim_WaitSpriteFade);
		case 13: DO_DELAY((3 * 60));
		case 14: DO_FUNC(Task_Skyrim_FadeInBackground);
		// case 15: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		case 16: DO_DELAY((1 * 60));
		case 17: DO_FUNC(Task_Skyrim_StartFadeOutText);
		case 18: DO_FUNC(Task_Skyrim_WaitSpriteFade);
        
		case 20: DO_DELAY((2 * 60));
		case 21: DO_FUNC(Task_Skyrim_DisplaySubtitle);
		case 22: DO_FUNC(Task_Skyrim_WaitSpriteFade);
		case 23: DO_DELAY((4 * 60));
		case 24: DO_FUNC(Task_Skyrim_StartFadeOutText);
		case 25: DO_FUNC(Task_Skyrim_WaitSpriteFade);
		
		case 30: DO_DELAY((4 * 60));
        case 31: DO_FUNC(Task_Skyrim_ShiftBgUp);
		case 32: DO_DELAY((1 * 60));
		case 35: DO_FUNC(Task_Skyrim_DisplayFinallyAwake);
		case 36: DO_FUNC(Task_Skyrim_WaitMessageAndClear);
		
		case 40: DO_DELAY((2 * 60));
		case 41: DO_FUNC(Task_Skyrim_StartFadeOutAll);
		case 42: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		case 43: DO_DELAY((2 * 60));
		case 44: DO_FUNC(Task_Skyrim_DisplayAprilFools);
		case 45: DO_FUNC(Task_Skyrim_WaitMessage);
		case 46: DO_DELAY((6 * 60));
        
		case 47: DO_FUNC(Task_Skyrim_DisplayComingSoon);
		case 48: DO_FUNC(Task_Skyrim_WaitMessage);
		case 49: DO_DELAY((10 * 60));
		
		case 100:
			SoftReset(RESET_ALL);
			break;
		
		default: DO_FUNC(Skyrim_GoToNextStage); // repeat until state
	}
}

#undef tTimer
