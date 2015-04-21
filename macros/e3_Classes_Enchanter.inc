|--------------------------------------------------------------------------------|
|- e3_Classes_Enchanter.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains Enchanter specific functions related to,							-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality / flow											-|
|--------------------------------------------------------------------------------|



#event add_RuneTarget "<#1#> Rune on"
#event add_RuneTarget "<#*#> Rune on #1#"
sub event_add_RuneTarget(string line, string RuneTarget)
/if (${Me.Class.ShortName.Equal[ENC]}) {
/if (${Debug}) /echo |- event_add_RuneTarget ==>
	
	/if (!${SpawnCount[pc ${RuneTarget}]}) {
		/docommand ${ChatToggle} I don't see any PCs matching [${RuneTarget}]...
	} else {
	
		/varset RuneTarget ${Spawn[pc ${RuneTarget}].CleanName}
	
		/if (!${Defined[RuneTargets]}) {
			/declare RuneTargets[1] string outer
			/varset RuneTargets[1] ${RuneTarget}
			/docommand ${ChatToggle} Maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget}].
		} else {
			
			/declare i int local
			
			/declare RuneTarget_Names string local
			
			/for i 1 to ${RuneTargets.Size}
			
				/varset RuneTarget_Names ${RuneTarget_Names}${RuneTargets[${i}]},
			
			/next i
			
			/if (${Select[${RuneTarget},${RuneTarget_Names}]}) {
				/docommand ${ChatToggle} I am already maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget}].
			} else {
				/call BuildArray RuneTargets ${RuneTarget}
				/docommand ${ChatToggle} Maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget_Names}${RuneTarget}].
			}
		}
	}
	
/if (${Debug}) /echo <== add_RuneTarget -|
}
/return



#event remove_RuneTarget "<#1#> Rune Off"
#event remove_RuneTarget "<#*#> Rune Off #1#"
#event remove_RuneTarget "<#*#> End Rune on #1#"
#event remove_RuneTarget "<#*#> All runes off"
#event remove_RuneTarget "<#*#> end runes"
sub event_remove_RuneTarget(line, ChatSender)
/if (${Me.Class.ShortName.Equal[ENC]}) {
/if (${Debug}) /echo |- remove_RuneTarget ==>

	/if (${Defined[RuneTargets]}) {

		/if (${Defined[ChatSender]}) {
		
			/declare i int local
			
			/for i 1 to ${RuneTargets.Size}

				/if (${RuneTargets[${i}].Equal[${ChatSender}]}) {
					/docommand ${ChatToggle} No longer maintaining [${chainRune.Arg[1,/]}] on [${RuneTargets[${i}]}].
					/call RemoveArrayElement RuneTargets "${RuneTargets[${i}]}"
				}
			
			/next i
			
		} else {
			/docommand ${ChatToggle} Ending chain runes.
			/deletevar RuneTargets
		}
		
	}
	
/if (${Debug}) /echo <== remove_RuneTarget -|
}
/return



SUB check_Rune
/if (${Debug}) /echo |- chainRune ==>

	/if (!${idle} && ${RuneTargets.Size}) {
		
		/declare i int local
		
		/declare runeSpell string local ${chainRune.Arg[1,/]}
		/declare runeGem string local ${chainRune.Arg[2,/].Right[1]}
		/if (${chainRune.Arg[3,/].Length} && ${chainRune.Arg[3,/].Find[reagent::]}) /declare runeReagent string local ${chainRune.Arg[3,/].Arg[2,::]}
		
		/if (${Defined[runeReagent]} && !${FindItemCount[${runeReagent}]}) {
			/docommand ${ChatToggle} I have run out of [${runeReagent}] and can no longer maintain [${runeSpell}].  Ending chain runes.
			/deletevar RuneTargets
		} else {
		
			/for i 1 to ${RuneTargets.Size}
				/if (${Spawn[pc ${RuneTargets[${i}]}].Distance} < ${Spell[${runeSpell}].MyRange} && ${Me.CurrentMana} > ${Spell[${runeSpell}].Mana} && !${NetBots[${RuneTargets[${i}]}].Buff.Find[${Spell[${runeSpell}].ID}]}) {
					/call e3_Cast ${NetBots[${RuneTargets[${i}]}].ID} "${runeSpell}" gem${runeGem} ${If[${Me.Gem[${runeSpell}]},3s,${Spell[${runeSpell}].RecastTime}s]}
				}
			/if (!${ActionTaken}) /next i
		}
	}

/if (${Debug}) /echo <== chainRune -|
/RETURN



Sub ENC_Setup
/if (${Debug}) /echo |- ENC_Setup ==>

	/if (${Ini[${Character_Ini},Enchanter,Chain Rune].Length}) /call IniToVar "${Character_Ini},Enchanter,Chain Rune" chainRune string outer

/if (${Debug}) /echo <== ENC_Setup -|
/return



Sub ENC_Background_Events

	/doevents add_RuneTarget
	/doevents remove_RuneTarget

/return



SUB ENC_MacroSettings
/if (${Debug}) /echo |- ENC_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#1" "check_CombatBuffs"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#2" "check_Burns"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#3" "check_Debuffs"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#4" "check_DoTs"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#5" "check_Rain"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#6" "check_Nukes"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#7" "check_Buffs"
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#8" "check_Pets"

/if (${Debug}) /echo <== ENC_MacroSettings -|
/RETURN



SUB ENC_CharacterSettings
/if (${Debug}) /echo |- ENC_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Enchanter,Chain Rune"

/if (${Debug}) /echo <== ENC_CharacterSettings -|
/RETURN



Sub ENC_Aliases
/if (${Debug}) /echo |- ENC_Aliases ==>

	/squelch /alias /runeon /bc Rune on
	/squelch /alias /runeoff /bc Rune off

/if (${Debug}) /echo <== ENC_Aliases -|
/return