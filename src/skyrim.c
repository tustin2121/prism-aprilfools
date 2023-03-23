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
#include "strings.h"
#include "text_window.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "trainer_pokemon_sprites.h"
#include "constants/songs.h"
#include "constants/rgb.h"

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

const u32 sGfx_bg[] = INCBIN_U32("graphics/skyrim/skyrim_bg.8bpp.lz");
const u32 sGfx_fg[] = INCBIN_U32("graphics/skyrim/skyrim_fg.8bpp.lz");

const u32 sMap_bg[]  = INCBIN_U32("graphics/skyrim/skyrim_bg.bin.lz");
const u32 sMap_fg0[] = INCBIN_U32("graphics/skyrim/skyrim_fg0.bin.lz");
const u32 sMap_fg1[] = INCBIN_U32("graphics/skyrim/skyrim_fg1.bin.lz");

const u32 sPal_skyrim[] = INCBIN_U32("graphics/skyrim/skyrim.gbapal.lz");

// const u32 gSkyrimGfx[] = INCBIN_U32("graphics/skyrim/test.8bpp.lz");
// const u32 gSkyrimMap[] = INCBIN_U32("graphics/skyrim/test.tilemap.lz");
// const u32 gSkyrimPal[] = INCBIN_U32("graphics/skyrim/test.gbapal.lz");

//-----------------------------------------------------------------------------

static void Task_Skyrim_Init(u8 taskId);
static void Task_Skyrim_RunBackground(u8 taskId);
static void Skyrim_GoToNextStage(u8 taskId);

EWRAM_DATA static u16 sBgXoff = 0;
EWRAM_DATA static u16 sBgYoff = 0;

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
	u8 taskId;
    u8 spriteId;
    u16 savedIme;

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
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
	
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
	
    ResetPaletteFade();
	
	LZ77UnCompVram(sGfx_fg, (u8*) BG_CHAR_ADDR(0));
	LZ77UnCompVram(sGfx_bg, (u8*) BG_CHAR_ADDR(1));
	LZ77UnCompVram(sMap_fg0, (u8*) BG_SCREEN_ADDR(29));
	LZ77UnCompVram(sMap_bg,  (u8*) BG_SCREEN_ADDR(30));
	LoadCompressedPalette(sPal_skyrim, BG_PLTT_ID(0), PLTT_SIZE_8BPP);
	
	// LZ77UnCompVram(sBirchSpeechShadowGfx, (u8 *)VRAM);
    // LZ77UnCompVram(sBirchSpeechBgMap, (u8 *)(BG_SCREEN_ADDR(7)));
    // LoadPalette(sBirchSpeechBgPals, BG_PLTT_ID(0), 2 * PLTT_SIZE_4BPP);
    // LoadPalette(&sBirchSpeechBgGradientPal[1], BG_PLTT_ID(0) + 1, PLTT_SIZEOF(8));
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    
	// taskId = CreateTask(Task_NewGameBirchSpeech_ReturnFromNamingScreenShowTextbox, 0);
    // gTasks[taskId].tTimer = 5;
    // gTasks[taskId].tBG1HOFS = -60;
    ResetAllPicSprites();
    
	BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
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
	
    SetVBlankCallback(VBlankCB_Skyrim);
    SetMainCallback2(CB2_Skyrim);
	
	CreateTask(Task_Skyrim_Init, 0);
}

//-----------------------------------------------------------------------------

static void Task_Skyrim_RunBackground(u8 taskId)
{
	sBgXoff -= Q_8_8(0.02);
}

//-----------------------------------------------------------------------------
#pragma region 

#define PAL_TEXT        0xFFFF0000
#define PAL_BACKGROUND  0x0000FFFF

#define tTimer data[0]

static void Task_Skyrim_Init(u8 taskId)
{
	CreateTask(Task_Skyrim_RunBackground, 5);
	sBgXoff = Q_8_8(0);
	sBgYoff = Q_8_8(-8);
	
	Skyrim_GoToNextStage(taskId);
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
static void Task_Skyrim_Delay(u8 taskId)
{
	gTasks[taskId].tTimer--;
	if (!gTasks[taskId].tTimer) {
		Skyrim_GoToNextStage(taskId);
    }
}

static void Task_Skyrim_StartFadeOutText(u8 taskId)
{
	BeginNormalPaletteFade(PAL_TEXT, 10, 0, 0x10, RGB_BLACK);
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

extern const u8 gText_Skyrim_Presents[];
static void Task_Skyrim_DisplayPresents(u8 taskId)
{
	FillWindowPixelBuffer(0, PIXEL_FILL(0));
	AddTextPrinterParameterized(0, FONT_SKYRIM, gText_Skyrim_Presents, 0, 1, 0, NULL);
	PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_GFX);
	
	// TODO: load "Bethesda Game Studios presents" message to screen
	BeginNormalPaletteFade(PAL_TEXT, 10, 0x10, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}

extern const u8 gText_Skyrim_Title[];
static void Task_Skyrim_DisplayTitle(u8 taskId)
{
	// TODO: load "The Elder Scrolls V: Skyrim" message to screen
	BeginNormalPaletteFade(PAL_TEXT, 10, 0x10, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_FadeInBackground(u8 taskId)
{
	// TODO: load "The Elder Scrolls V: Skyrim" message to screen
	BeginNormalPaletteFade(PAL_BACKGROUND, 10, 0x10, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}

extern const u8 gText_Skyrim_IntroLine1[];
static void Task_Skyrim_DisplayFinallyAwake(u8 taskId)
{
	FillWindowPixelBuffer(0, PIXEL_FILL(0));
	AddTextPrinterParameterized(0, FONT_SKYRIM, gText_Skyrim_IntroLine1, 0, 1, 2, NULL);
	PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_GFX);
	
	LZ77UnCompVram(sMap_fg1, (u8*) BG_SCREEN_ADDR(29));
	
	// TODO: Switch background to guy facing us
	// TODO: load "Hey you, you're finally awake." message to screen
	BeginNormalPaletteFade(PALETTES_ALL, 10, 0x10, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}

static void Task_Skyrim_DisplayAprilFools(u8 taskId)
{
	// TODO: load "Happy April Fools from RainbowDevs" message to screen
	BeginNormalPaletteFade(PALETTES_ALL, 10, 0x10, 0, RGB_BLACK);
	Skyrim_GoToNextStage(taskId);
}




#pragma endregion
//-----------------------------------------------------------------------------

#define DO_FUNC(fn) gTasks[taskId].func = fn; break
#define DO_DELAY(time) gTasks[taskId].func = Task_Skyrim_Delay; gTasks[taskId].tTimer = time; break

static void Skyrim_GoToNextStage(u8 taskId)
{
	switch (gMain.state++) {
		case  0: DO_DELAY((4 * 60));
		case  1: DO_FUNC(Task_Skyrim_DisplayPresents);
		case  2: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		case  3: DO_DELAY((4 * 60));
		case  4: DO_FUNC(Task_Skyrim_StartFadeOutText);
		case  5: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		
		case 10: DO_DELAY((2 * 60));
		case 11: DO_FUNC(Task_Skyrim_DisplayTitle);
		case 12: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		case 13: DO_DELAY((3 * 60));
		case 14: DO_FUNC(Task_Skyrim_FadeInBackground);
		case 15: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		case 16: DO_DELAY((3 * 60));
		case 17: DO_FUNC(Task_Skyrim_StartFadeOutText);
		case 18: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		
		case 20: DO_DELAY((10 * 60));
		case 21: DO_FUNC(Task_Skyrim_DisplayFinallyAwake);
		case 22: DO_FUNC(Task_Skyrim_WaitMessage);
		
		case 30: DO_DELAY((2 * 60));
		case 31: DO_FUNC(Task_Skyrim_StartFadeOutAll);
		case 32: DO_FUNC(Task_Skyrim_WaitPaletteFade);
		case 33: DO_DELAY((2 * 60));
		case 34: DO_FUNC(Task_Skyrim_DisplayAprilFools);
		case 35: DO_FUNC(Task_Skyrim_WaitMessage);
		case 36: DO_DELAY((10 * 60));
		
		case 40:
			SoftReset(RESET_ALL);
			break;
		
		default: DO_FUNC(Skyrim_GoToNextStage); // repeat until state
	}
}

#undef tTimer
