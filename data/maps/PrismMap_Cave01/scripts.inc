PrismMap_Cave01_MapScripts::
	.byte 0

PrismMap_Cave01_EventScript_TunnelMan::
	lock
	faceplayer
	msgbox PrismMap_Cave01_Text_TunnelMan
	closemessage
	@
	applymovement 1, PrismMap_Cave01_Movement_TunnelMan_Man
	waitmovement 1
	applymovement OBJ_EVENT_ID_PLAYER, PrismMap_Cave01_Movement_TunnelMan_Player
	waitmovement OBJ_EVENT_ID_PLAYER
	@
	applymovement OBJ_EVENT_ID_PLAYER, PrismMap_Cave01_Movement_TunnelMan_Player2
	fadescreen FADE_TO_BLACK
	@ 
	playse SE_SWITCH
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_CLICK
	waitse
	playse SE_UNLOCK
	waitse
	playse SE_LAVARIDGE_FALL_WARP
	delay 20
	playse SE_ICE_BREAK
	waitse
	playse SE_FALL
	waitse
	delay 20
	playse SE_M_ROCK_THROW
	waitse
	playse SE_M_STRENGTH
	delay 10
	playse SE_M_EXPLOSION
	waitse
	@ playse SE_M_EARTHQUAKE
	@ waitse
	delay 60
	@ 
	playbgm MUS_B_PYRAMID_TOP, TRUE
	delay 15
	startskyrim
	waitstate
	release
	end

PrismMap_Cave01_Text_TunnelMan:
	.string "Hey, kid, are you lost?\p"
	.string "Your campsite is up north?\n"
	.string "The path was blocked by a landslide?\p"
	.string "Well, you're in luck! This cart will\n"
	.string "take you right back there.$"
PrismMap_Cave01_Movement_TunnelMan_Man:
	walk_right
	walk_in_place_faster_left
	step_end
PrismMap_Cave01_Movement_TunnelMan_Player:
	walk_down
	delay_8
	walk_in_place_right
	delay_16
	walk_down
	walk_down
	step_end
PrismMap_Cave01_Movement_TunnelMan_Player2:
	walk_down
	step_end

EventScript_Pickaxe::
	msgbox EventScript_Pickaxe_Text, MSGBOX_SIGN
	end
EventScript_Pickaxe_Text::
	.string "You need a Mining Pick to mine.$"

