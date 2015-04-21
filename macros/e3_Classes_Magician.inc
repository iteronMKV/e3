|----------------------------------------------------------------------------|
|- e3_Classes_Magician.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Magician specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



#event petWeapons "#1# tells you, 'Pet Weapons'"
#event petWeapons "<#1#> Pet Weapons"
#event petWeapons "#1# tells you, 'Pet Toys'"
#event petWeapons "<#1#> Pet Toys"
sub event_petWeapons(string line, string ChatSender)
/if (${Debug}) /echo |- EVENT_petWeapons ==>

	/if (${petItems.Size}) {
	
		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|
		/if (!${Bool[${Macro.Return.Arg[2,_]}]}) {

			/call IniToVar "${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
			
			| If the user is not valid, check allow_BuffRequests
			/if (${Macro.Return}) {
		
				| If allow_BuffRequests=TRUE, check restricked PCs
				/if (!${Select[${ChatSender},${Ini[${Macro_Ini},Buff Requests,Restricted PCs]}]}) /goto :allowAccess
			}
		} else {
		
			:allowAccess
			
			/if (!${SpawnCount[pc ${ChatSender}]}) {
				/if (${Debug}) /echo |- EVENT_petWeapons -| [${ChatSender}] is not in ${Zone}.
			} else {
				
				/if (${Spawn[pc ${ChatSender}].Distance} > 200) {
					/tell ${ChatSender} You're too far away.
				} else {
				
					/if (!${Spawn[pc ${ChatSender}].LineOfSight}) {
						/tell ${ChatSender} I can't see you.
					} else {

						/if (!${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}) {
							/tell ${ChatSender} You don't have a pet, jackass! 
						} else {
						
							/if (${ChatSender.NotEqual[${Me}]}) /tell ${ChatSender} Incoming pet weapons.  Please stay still for a few moments.
							
							| Declare local variables
							/declare i int local
							/declare x int local
							/declare e int local
							/declare retryTimer timer local
							/declare miscTimer timer local
							/declare summonedItem string local
							
							| Target the pet, and move to it
							/call TrueTarget ${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
							/call MoveTo -SpawnID|${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
							
							/for i 1 to ${petItems.Size}
							
							/if (${Target.Distance} < 20) {

								/call e3_Cast 0 "${petItems[${i}]}/GiveUpTimer|${If[${Me.Gem[${petItems[${i}].Arg[1,/]}]},50,150]}" ${petItems[${i}].Arg[2,/]} ${If[${petItems[${i}].Arg[2,/].Left[3].Equal[gem]},30s,10s]}
								/delay 20 ${Cursor.ID}

								/if (${Cursor.ID}) {
									
									/varset summonedItem ${Cursor.Name}
									/varset retryTimer 50
									
									:OpenTrade_Loop
									
									/click left target
									/delay 10 ${Window[GiveWnd].Open}
									
									/if (!${Window[GiveWnd].Open}) {
										/if (${retryTimer}) {
											/goto :OpenTrade_Loop
										} else {
											/echo Failed to open trade with ${Target.CleanName}.
										}
									} else {
										
										/varset retryTimer 30
										:WaitAccept_Loop
										
										/notify GiveWnd GVW_Give_Button LeftMouseUp
										/delay 5 !${Window[GiveWnd].Open}
										
										/if (${Window[GiveWnd].Open}) {
											/if (${retryTimer}) {
												/goto :WaitAccept_Loop
											} else {
												/echo Failed to open trade with ${Target.CleanName}.
											}
										}
									}
								}
								
								/if (${FindItemCount[=${summonedItem}]}) {
									
									/if (${Debug}) /echo |- EVENT_petWeapons -| Dropped [${summonedItem}] in my inventory.
								
									| Open InventoryWindow.
									/varset miscTimer 3s
									
									:openInventory
									/if (!${Window[InventoryWindow].Open}) /nomodkey /keypress i
									/delay 5 ${Window[InventoryWindow].Open}
									/if (!${Window[InventoryWindow].Open} && ${miscTimer}) /goto :openInventory
						
									| Search packslots.
									/varset e 0
									/for x 1 to 8  
									
										| If the item in the packslot is the summoned item, pick it up.
										/if (!${Me.Inventory[pack${x}].Container} && ${summonedItem.Equal[${Me.Inventory[pack${x}]}]}) {
											/if (${Debug}) /echo |- EVENT_petWeapons -| Picking up [${summonedItem}] form pack${x}
											/itemnotify pack${x} leftmouseup
										} else {
										
											| If the item in the packslot is a container, search it.
											/for e 1 to ${Me.Inventory[pack${x}].Container}
												/if (${summonedItem.Equal[${Me.Inventory[pack${x}].Item[${e}]}]}) {
													/if (!${Window[pack${x}].Open}) /itemnotify pack${x} rightmouseup
													/delay 1s ${Window[pack${x}].Open}
													/if (${Debug}) /echo |- EVENT_petWeapons -| Picking up [${summonedItem}] form pack${x} slot${e}
													/itemnotify in pack${x} ${e} leftmouseup
												}
											/next e
										}
									/next x
									
									/if (${retryTimer}) {
										/goto :OpenTrade_Loop
									} else {
										/if (${Cursor.Name.Equal[${summonedItem}]}) /destroy 
									}
								}
							}
							
							/next i
						}
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_petWeapons -|
/return



#event distributePetWeapons "#1# tells you, 'Hand out Pet Weapons'"
#event distributePetWeapons "<#1#> Hand out Pet Weapons"
#event distributePetWeapons "#1# tells you, 'Distribute Pet Weapons'"
#event distributePetWeapons "<#1#> Distribute Pet Weapons"
sub event_distributePetWeapons(line, ChatSender)
/if (${Debug}) /echo |- event_distributePetWeapons ==>

	/if (${petItems.Size}) {
	
		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|
		/if (!${Bool[${Macro.Return.Arg[2,_]}]}) {

			/call IniToVar "${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
			
			| If the user is not valid, check allow_BuffRequests
			/if (${Macro.Return}) {
		
				| If allow_BuffRequests=TRUE, check restricked PCs
				/if (!${Select[${ChatSender},${Ini[${Macro_Ini},Buff Requests,Restricted PCs]}]}) /goto :allowAccess
			}
		} else {
		
			:allowAccess
			
			/if (!${SpawnCount[pc ${ChatSender}]}) {
				/if (${Debug}) /echo |- EVENT_petWeapons -| [${ChatSender}] is not in ${Zone}.
			} else {
				
				/if (${Spawn[pc ${ChatSender}].Distance} > 200) {
					/tell ${ChatSender} You're too far away.
				} else {
				
					/if (!${Spawn[pc ${ChatSender}].LineOfSight}) {
						/tell ${ChatSender} I can't see you.
					} else {
					
					
						/if (${ChatSender.NotEqual[${Me}]}) /tell ${ChatSender} Handing out pet weapons...
					
						/declare originalLoc string local ${Me.Loc.Replace[ ,]}
						/declare startingLoc string local ${Spawn[pc ${ChatSender}].Loc.Replace[ ,]}
						
						/declare nearbyPetOwners string local
						/declare petOwner string local
						/declare petID int local
						
						/declare i int local
						
					
						| Move to the starting point
						/call MoveTo -Loc|${startingLoc}
					
						| Make a list of all the pets nearby
						/for i 1 to ${SpawnCount[radius 100]}
						
							/if (${NearestSpawn[${i}].Type.Equal[PC]} && ${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID} && ${Spawn[id ${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID}].LineOfSight}) /varset nearbyPetOwners ${nearbyPetOwners}${NearestSpawn[${i}]},
				
						/next i
					
						| Pass out weapons to each person on the nearbyPetOwners list
						
						/varset i 1
						
						:next_PetOwner
						
						| set petOwner and petID, then send the owner a msg.
						/varset petOwner ${nearbyPetOwners.Arg[${i},,]}
					
						/call EVENT_petWeapons "${Line}" "${petOwner}"

						/varcalc i ${i} + 1
						/if (${Bool[${nearbyPetOwners.Arg[${i},,]}]}) /goto :next_PetOwner
						
						| Move to the starting point
						/call MoveTo -Loc|${originalLoc}
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== event_distributePetWeapons -|
/return



#event CoH_Group "#*# tells the group, 'CoH the group"
#event CoH_Group "#*# tells the group, 'CoH group"
sub event_CoH_Group
/if (${Debug}) /echo |- CoH_Group ==>

	/if (${Defined[cohSpell]}) {

	}

/if (${Debug}) /echo |- CoH_Group ==>
/return



Sub MAG_Setup
/if (${Debug}) /echo |- MAG_Setup ==>
	
	/call IniToArray "${Character_Ini},Magician,Summoned Pet Item#" petItems

/if (${Debug}) /echo <== MAG_Setup -|
/return



Sub MAG_Background_Events

/return



SUB MAG_MacroSettings
/if (${Debug}) /echo |- MAG_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#1" "check_CombatBuffs"	
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#2" "check_petHeal"
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#3" "check_Burns"
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#4" "check_Debuffs"
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#5" "check_Rain"
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#6" "check_Nukes"
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#7" "check_Buffs"
	/call WriteToIni "${advSettings_Ini},MAG Functions,MAG Function#8" "check_Pets"

/if (${Debug}) /echo <== MAG_MacroSettings -|
/RETURN



SUB MAG_CharacterSettings
/if (${Debug}) /echo |- MAG_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Magician,Summoned Pet Item#1"

/if (${Debug}) /echo <== MAG_CharacterSettings -|
/RETURN



Sub MAG_Aliases
/if (${Debug}) /echo |- MAG_Aliases ==>

	/squelch /alias /dpw /bc Distribute Pet Weapons
	/squelch /alias /PetWeapons /bc Pet Weapons

/if (${Debug}) /echo <== MAG_Aliases -|
/return



#event callgroup "<#1#> Call Group#2#"
sub event_callgroup( line, ChatSender, args )
/if (${Debug}) /echo |- EVENT_testit ==>

        /if ( ${Me.Class.ShortName.Equal[MAG]} ) {

                /declare i int local 0
                /declare checkMember string local
                /declare castName string local Call of the Hero

                /for i 1 to ${Group}
                        /varset checkMember ${Group.Member[${i}]}
                        /if (${Spawn[pc =${checkMember}].Distance} > 20 && !${Group.Member[${i}].Class.ShortName.Equal[MAG]}) {
                                /delay 25s ( ${Me.SpellReady[${castName}]} )
                                /gsay Calling: ${checkMember}
                                /call e3_Cast ${Spawn[pc =${checkMember}].ID} "${castName}/GiveUpTimer|${If[!${Me.Book[${castName}]},0,${If[${Me.Gem[${castName}]},30,90]}]}"
                        }
                /next i

        }


/if (${Debug}) /echo <== EVENT_callgroup -|
/return