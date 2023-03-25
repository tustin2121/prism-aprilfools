PrismMap_Outside_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PrismMap_Outside_FrameTable
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, PrismMap_Outside_OnWarpIntoMap
	.byte 0

.align 2
PrismMap_Outside_OnWarpIntoMap:
	map_script_2 VAR_LITTLEROOT_TOWN_STATE, 3, PrismMap_Outside_EventScript_PlaceLandslide
	.2byte 0
PrismMap_Outside_FrameTable:
	map_script_2 VAR_LITTLEROOT_TOWN_STATE, 0, PrismMap_Outside_EventScript_IntroCutscene
	.2byte 0

PrismMap_Outside_EventScript_PlaceLandslide:
	setmetatile 18, 25, 0x311, TRUE
	setmetatile 19, 25, 0x312, TRUE
	setmetatile 18, 26, 0x08B, TRUE
	setmetatile 19, 26, 0x08C, TRUE
	setmetatile 17, 25, 0x0E0, TRUE
	setmetatile 18, 27, 0x0E0, TRUE
	setmetatile 18, 24, 0x0E0, TRUE
	setmetatile 19, 24, 0x0E0, TRUE
	return


PrismMap_Outside_EventScript_IntroCutscene:
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceLeft
	applymovement 1, Common_Movement_FaceRight
	message PrismMap_Outside_Text_IntroCutscene
	waitmessage
	waitbuttonpress
	releaseall
	setvar VAR_LITTLEROOT_TOWN_STATE, 1
	end
PrismMap_Outside_Text_IntroCutscene:
	.string "It's so beautiful here, isn't it?\p"
	.string "I'm glad we could have a relaxing\n"
	.string "time together…\p"
	.string "I'm sorry you had to deal with your\n"
	.string "father moving on.\p"
	.string "No child wants to go through that…\p"
	.string "{PLAYER}, your father has always\n"
	.string "been a great man, and, while he's had\l"
	.string "many fans, you're still his biggest.\p"
	.string "I know you want to follow in his\n"
	.string "footsteps…\p"
	.string "But promise me, no matter where you go\n"
	.string "in life… never forget your dear mother.\p"
	.string "Thanks for coming with me, {PLAYER}.$"

PrismMap_Outside_EventScript_Mother::
	msgbox PrismMap_Outside_Text_Mother, MSGBOX_NPC
	end
PrismMap_Outside_Text_Mother:
	.string "It's so beautiful here, isn't it?\p"
	.string "I've missed just escaping into the\n"
	.string "wilderness…\p"
	.string "Thanks for coming with me, {PLAYER}.$"


PrismMap_Outside_EventScript_TestSkyrim::
	lock
	fadescreen FADE_TO_BLACK
	delay 15
	@ 
	startskyrim
	waitstate
	release
	end
	
	
PrismMap_Outside_EventScript_Fire::
	msgbox PrismMap_Outside_Text_Fire, MSGBOX_SIGN
	end
PrismMap_Outside_Text_Fire:
	.string "Looks hot! Better not touch it…$"


PrismMap_Outside_EventScript_ExitCutscene::
	lockall
	getplayerxy VAR_0x800A, VAR_0x800B
	subvar VAR_0x800A, 9
	@
	playbgm MUS_FOLLOW_ME, FALSE
	applymovement 1, PrismMap_Outside_Movement_ExitCutscene_0
	message PrismMap_Outside_Text_LeavingCutscene1
	waitmovement 1
	waitmessage
	@
	message PrismMap_Outside_Text_LeavingCutscene2
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	selectpointer PrismMap_Outside_MovementTable_ExitCutscene_Start, VAR_0x800A
	applymovement 1, 0
	waitmovement 1
	waitmessage
	waitbuttonpress
	closemessage
	@
	fadedefaultbgm
	selectpointer PrismMap_Outside_MovementTable_ExitCutscene_End, VAR_0x800A
	applymovement 1, 0
	waitmovement 1
	setvar VAR_LITTLEROOT_TOWN_STATE, 2
	releaseall
	end
PrismMap_Outside_Text_LeavingCutscene1:
	.string "{PLAYER}!\p$"
PrismMap_Outside_Text_LeavingCutscene2:
	.string "…Oh, heading out for a walk?\p"
	.string "Could you try to get some\n"
	.string "firewood for us?\p"
	.string "The fire will need some soon to\n"
	.string "keep us nice and warm tonight!\p"
	.string "…And, {PLAYER}…\n"
	.string "…{PAUSE 30} …{PAUSE 30} …{PAUSE 30} Just…{PAUSE 20} be safe!$"
.align 2
PrismMap_Outside_MovementTable_ExitCutscene_Start:
	.4byte PrismMap_Outside_Movement_ExitCutscene_Start1
	.4byte PrismMap_Outside_Movement_ExitCutscene_Start2
	.4byte 0
PrismMap_Outside_MovementTable_ExitCutscene_End:
	.4byte PrismMap_Outside_Movement_ExitCutscene_End1
	.4byte PrismMap_Outside_Movement_ExitCutscene_End2
	.4byte 0
PrismMap_Outside_Movement_ExitCutscene_0:
	emote_exclamation_mark
	face_down
	step_end
PrismMap_Outside_Movement_ExitCutscene_Start1:
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_down
	step_end
PrismMap_Outside_Movement_ExitCutscene_Start2:
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_down
	step_end
PrismMap_Outside_Movement_ExitCutscene_End1:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	face_down
	step_end
PrismMap_Outside_Movement_ExitCutscene_End2:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	face_down
	step_end


PrismMap_Outside_EventScript_LandSlide::
	lock
	playbgm MUS_DUMMY, TRUE
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	delay 12
	#
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	waitstate
	@
	call PrismMap_Outside_EventScript_PlaceLandslide
	special DrawWholeMapView
	@
	setvar VAR_0x8004, 1   @ vertical pan
	setvar VAR_0x8005, 1   @ horizontal pan
	setvar VAR_0x8006, 16  @ num shakes
	setvar VAR_0x8007, 3   @ shake delay
	special ShakeCamera
	setvar VAR_LITTLEROOT_TOWN_STATE, 3
	release
	end




