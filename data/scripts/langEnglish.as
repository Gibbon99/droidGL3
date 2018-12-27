
//------------------------------------------------------------
//
// Choose language and set strings
void as_setLanguageStrings()
//------------------------------------------------------------
{
	switch (currentLanguage)
	{
		case LANG_ENGLISH:
			as_setEnglishText();
			break;

		case LANG_ITALIAN:
			as_setItalianText();
			break;
	}
}

//------------------------------------------------------------
//
// " ! " - is a line break in a textbox = needs a space either side
//
// Create English text strings
void as_setEnglishText()
//------------------------------------------------------------
{
	gui_addKeyAndText("move",				"Move");
	gui_addKeyAndText("charging",			"Charging");
	gui_addKeyAndText("healing",			"Healing");
	gui_addKeyAndText("transfer",			"Transfer");
	gui_addKeyAndText("terminal",			"Terminal");
	gui_addKeyAndText("paused",				"Paused");
	gui_addKeyAndText("lift",				"Lift");
	gui_addKeyAndText("shipView",			"Ship View");
	gui_addKeyAndText("database",			"Database");
	gui_addKeyAndText("deckView",			"Deck View");
	gui_addKeyAndText("captured",			"Captured");
	gui_addKeyAndText("healed",				"No damage");
	gui_addKeyAndText("deadlock",			"Deadlock");

	gui_addKeyAndText("startGame", 			"Start Game");
	gui_addKeyAndText("joinGame", 			"Join Game");
	gui_addKeyAndText("tutorial", 			"Tutorial");
	gui_addKeyAndText("options", 			"Options");
	gui_addKeyAndText("help", 				"Information");
	gui_addKeyAndText("exit", 				"Exit Game");
	gui_addKeyAndText("logoffTerminal",		"Logoff Terminal");

	gui_addKeyAndText("buttonOptionsBack",  "Back");


	gui_addKeyAndText("restartTextBox",		"This setting will not take effect until a restart.");
	gui_addKeyAndText("exitTextBox", 		"Do you wish to terminate your connection ?");
	gui_addKeyAndText("tutMoveLabel", 		"Tutorial - Move");
	gui_addKeyAndText("tutMoveText",		"  Use the arrow keys to move. ! ! Pressing the Action Key will shoot in your current direction. ! ! Hold down the Action Key when not moving to initiate transfer.");
	gui_addKeyAndText("buttonNext",			"Next");
	gui_addKeyAndText("buttonCancel",		"Cancel");
	gui_addKeyAndText("buttonConfirm",		"Confirm");
	gui_addKeyAndText("buttonApply",		"Apply");

	gui_addKeyAndText("droidInfo",			"Droid Information");
	gui_addKeyAndText("deckMap",			"Deck Map");
	gui_addKeyAndText("shipView",			"Ship View");

	gui_addKeyAndText("transferOne",		"Commencing transfer process...");
	gui_addKeyAndText("transferTwo",		"This is the droid you currently control");
	gui_addKeyAndText("transferThree",		"This is the droid you want to control");
	gui_addKeyAndText("transferFour",		"Taking control of new droid...");
	gui_addKeyAndText("transferFive",		"Transfer Deadlock. Try again.");
	gui_addKeyAndText("transferLost",		"You have lost the Transfer process.");
	gui_addKeyAndText("transferLostHUD",	"Transfer lost");

	gui_addKeyAndText("tutTransText",		" To take over another droid you enter into Transfer mode. ! ! Press and hold the Action key with no other keys down to enter this mode. Colliding with another droid will initiate transfer mode.");
	gui_addKeyAndText("tutTransGameText",	"To gain control of another droid, you must win the circuit transfer game. Select the side you will use. Press the UP / DOWN keys to move your token to a suitable circuit. Press the Action key to select that circuit. You must have more of your color before the time runs out in order to win.");
	gui_addKeyAndText("tutTransferLabel",	"Tutorial - Transfer Mode");
	gui_addKeyAndText("tutTransGameLabel",	"Tutorial - Transfer Game");
	gui_addKeyAndText("tutLiftLabel",		"Tutorial - Lifts");
	gui_addKeyAndText("tutLiftText",		"To change between decks, you use a lift. A lift is activated by stopping over a lift tile and pressing the Action Key. Use the direction keys to select a new level and press the Action Key to select it.");
	gui_addKeyAndText("tutTerminalLabel",	"Tutorial - Terminals");
	gui_addKeyAndText("tutTerminalText",	"Stop at a terminal and press the Action Key. This will grant you access to the ships computer systems. Access to the droid database is controlled by the access level of your current droid.");
	gui_addKeyAndText("tutHealingLabel",	"Tutorial - Healing");
	gui_addKeyAndText("tutHealingText",		"All droids have a health indicator which spins on them. The slower this spins, the lower the health level of the droid. To repair damage stop over a repair tile. Your score is subtracted as you repair.");

	gui_addKeyAndText("tutTipsLabel",		"Tutorial - Gameplay tips");
	gui_addKeyAndText("tutTipsText",		"  - Colliding with another droid will damage both. ! - Some droids are immune to the disrupter weapons. ! - Your control over a droid weakens with time. ! - The first number on a droid indicates its ranking.");
	gui_addKeyAndText("audioPlaySnd",		"Play sound effects");
	gui_addKeyAndText("audioSndDisabled",	"Sound is disabled.");
	gui_addKeyAndText("videoUseBackingBMP",	"Use backing bitmap");
	gui_addKeyAndText("videoUseHoldingBMP", "Use holding bitmap");
	gui_addKeyAndText("fullScreen",			"Run full screen");
	gui_addKeyAndText("tileColor",			"Tile Color");
	gui_addKeyAndText("redValue",				"red");
	gui_addKeyAndText("greenValue",				"green");
	gui_addKeyAndText("blueValue",				"blue");
	gui_addKeyAndText("yellowValue",			"yellow");
	gui_addKeyAndText("redLabel",				"Red");
	gui_addKeyAndText("greenLabel",				"Green");
	gui_addKeyAndText("blueLabel",				"Blue");
	gui_addKeyAndText("yellowLabel",			"Yellow");
	gui_addKeyAndText("retroLabel",				"Retro");
	gui_addKeyAndText("futureLabel",			"Future");
	gui_addKeyAndText("retroValue",				"retro");
	gui_addKeyAndText("futureValue",			"future");
	gui_addKeyAndText("tileType",				"Tile Type");
	gui_addKeyAndText("classicValue",			"classic");
	gui_addKeyAndText("classicLabel",			"Classic");
	gui_addKeyAndText("turquoiseLabel",			"Turquoise");
	gui_addKeyAndText("turquoiseValue",			"turquoise");
	gui_addKeyAndText("grayLabel",				"Gray");
	gui_addKeyAndText("grayValue",				"gray");
	gui_addKeyAndText("particleType",			"Particle Type");
	gui_addKeyAndText("particleOffValue",		"0");
	gui_addKeyAndText("particlePrimValue",		"1");
	gui_addKeyAndText("particleBitmapValue",	"2");
	gui_addKeyAndText("particleOffLabel",		"Off");
	gui_addKeyAndText("particlePrimLabel",		"Primitive");
	gui_addKeyAndText("particleBitmapLabel",	"Bitmap");
	gui_addKeyAndText("swapCopyValue",			"1");
	gui_addKeyAndText("swapCopyLabel",			"Copy");
	gui_addKeyAndText("swapFlipValue",			"2");
	gui_addKeyAndText("swapFlipLabel",			"Flip");
	gui_addKeyAndText("swapType",				"Swap Method");
	gui_addKeyAndText("vsyncType",				"VSync Method");
	gui_addKeyAndText("vsyncSystemValue",		"0");
	gui_addKeyAndText("vsyncSystemLabel",		"System");
	gui_addKeyAndText("vsyncWaitValue",			"1");
	gui_addKeyAndText("vsyncWaitLabel",			"Wait");
	gui_addKeyAndText("vsyncOffValue",			"2");
	gui_addKeyAndText("vsyncOffLabel",			"Off");
	gui_addKeyAndText("optionVideo",			"Video");
	gui_addKeyAndText("optionAudio",			"Audio");
	gui_addKeyAndText("optionGraphics",			"Graphics");
	gui_addKeyAndText("optionGame",				"Game");
	gui_addKeyAndText("optionMainMenu",			"Main Menu");
	gui_addKeyAndText("videoOptions",			"Video Options");
	gui_addKeyAndText("langEnglish",			"English");
	gui_addKeyAndText("langItalian",			"Italian");
	gui_addKeyAndText("languageType",			"Language");
	gui_addKeyAndText("optionControls",			"Controls");
	gui_addKeyAndText("controlsHelp",			"Press ENTER to change key binding.");
	gui_addKeyAndText("captureHelp",			"Press key to set");
	gui_addKeyAndText("captureHelpCancel",		"ESC to cancel");
	gui_addKeyAndText("scrollText",		"         Welcome to Paradroid. ! ! ^ Press [ LEFT CTRL KEY ] to play or wait for further instructions ! Use the [ ARROW KEYS ] to navigate the menus ! Press [ ENTER ] to make a choice ! ! ^ Clear the freighter of robots by destroying them with twin lasers or by transferring control to them. ! ! Control is by keyboard only, as follows: ! ! At all times the keyboard moves the robot and holding fire down will allow use of lifts and consoles. ! ! In addition, pressing fire with no keys pressed will prepare the current robot for transfer. Contact with another robot with the fire key down will initiate transfer.! ! ! ^ ! ! ! A fleet of Robo-Freighters on its way to the Beta Ceti system reported entering an uncharted field of asteroids. Each ship carries a cargo of battle droids to reinforce the outworld defences. ! ! Two distress beacons have been recovered. Similar messages were stored on each. The ships had been bombarded by a powerful radionic beam from one of the asteroids. ! ! All of the robots on the ships, including those in storage, became hyper-active. The crews report an attack by droids, isolating them on the bridge. They cannot reach the shuttle and can hold out for only a couple more hours. ! ! Since these beacons were located two days ago, we can only fear the worst. ! ! Some of the fleet was last seen heading for enemy space. In enemy hands the droids can be used against our forces. ! ! Docking would be impossible, but we can beam aboard a prototype Influence Device.! ^ ! ! ! The 001 Influence device consists of a helmet, which, when placed over a robot's control unit can halt the normal activities of that robot for a short time. The helmet has its own power supply and powers the robot itself, at an upgraded capability. The helmet also uses an energy cloak for protection of the host. ! ! The helmet is fitted with twin lasers mounted in a turrent. These are low powered and have a slow recycle rate. ! ! Most of the device's resources are channeled towards holding control of the host robot, as it attempts to resume 'normal' operation. It is therefore necessary to change the host robot often to prevent the device from burning out. Transfer to a new robot requires the device to drain its host of energy in order to take it over. Failure to achieve transfer results in the device being a free agent once more. ! ! Further information incoming... ! ^ ! ! ! An Influence device can transmit only certain data, namely its own location and the location of other robots in visual range. This data is merged with known ship layouts on your C64 remote terminal. ! ! Additional information about the ship and robots may be obtained by accessing the ship's computer at a console. A small-scale plan of the whole deck is available, as well as a side elevation of the ship. ! ! Robots are represented on-screen as a symbol showing a three-digit number. The first digit shown is the important one, the class of the robot. It denotes strength also. ! ! To find out more about any given robot, use the robot enquiry system at a console. Only data about units of a lower class than your current host is available, since it is the host's security clearance which is used to access the console. ! ! ! ! ! Further information incoming... ! ^ ! ! ! ! ! Paradroid programmed by Andrew Braybrook. ! ! ! Sound advice by Steve Turner. ! ! ! SDL2 - OpenGL port by David Berry. ! 2019. ! ! ! ! ! Powered by the SDL2 - OpenGL library. ! Scripting engine by AngelScript. ! Physics by Chipmunk Physics. ! Network library: RakNet ! ! ! ! Press fire to play. ! ! ! ! ! ! ^ ! ! ! ! Message restarts. ! ! ! ! ! ! ! !");

	gui_addKeyAndText("controlType", "Input type");
	gui_addKeyAndText("inputKeyboardValue", "0");
	gui_addKeyAndText("inputKeyboardLabel", "Keyboard");
	gui_addKeyAndText("inputJoystickValue", "1");
	gui_addKeyAndText("inputJoystickLabel", "Joystick");

	gui_addKeyAndText("db_001description", "001 - Influence Device");
	gui_addKeyAndText("db_001className", "Influence");
	gui_addKeyAndText("db_001drive", "None");
	gui_addKeyAndText("db_001brain", "None");
	gui_addKeyAndText("db_001weapon", "Lasers");
	gui_addKeyAndText("db_001sensor1", "~");
	gui_addKeyAndText("db_001sensor2", "~");
	gui_addKeyAndText("db_001sensor3", "~");
	gui_addKeyAndText("db_001notes", "Robot activity influence device. This helmet is self-powered and will control any robot for a short time. Lasers are turret mounted.");

	gui_addKeyAndText("db_123description", "123 - Disposal Droid");
	gui_addKeyAndText("db_123className", "Disposal");
	gui_addKeyAndText("db_123drive", "Tracks");
	gui_addKeyAndText("db_123brain", "None");
	gui_addKeyAndText("db_123weapon", "None");
	gui_addKeyAndText("db_123sensor1", "Spectral");
	gui_addKeyAndText("db_123sensor2", "Infra-red");
	gui_addKeyAndText("db_123sensor3", "~");
	gui_addKeyAndText("db_123notes", "Simple rubbish disposal robot. Common device in most space craft to maintain a clean ship.");

	gui_addKeyAndText("db_139description", "139 - Disposal Droid");
	gui_addKeyAndText("db_139className", "Disposal");
	gui_addKeyAndText("db_139drive", "Anti-grav");
	gui_addKeyAndText("db_139brain", "None");
	gui_addKeyAndText("db_139weapon", "None");
	gui_addKeyAndText("db_139sensor1", "Spectral");
	gui_addKeyAndText("db_139sensor2", "~");
	gui_addKeyAndText("db_139sensor3", "~");
	gui_addKeyAndText("db_139notes", "Created by Dr. Masternak to clean up large heaps of rubbish. Its scoop is used to collect rubbish. It is then crushed internally.");

	gui_addKeyAndText("db_247description", "247 - Servant Droid");
	gui_addKeyAndText("db_247className", "Servant");
	gui_addKeyAndText("db_247drive", "Anti-grav");
	gui_addKeyAndText("db_247brain", "Neutronic");
	gui_addKeyAndText("db_247weapon", "None");
	gui_addKeyAndText("db_247sensor1", "Spectral");
	gui_addKeyAndText("db_247sensor2", "~");
	gui_addKeyAndText("db_247sensor3", "~");
	gui_addKeyAndText("db_247notes", "Light duty servant robot. One of the first to use the anti-grav system.");

	gui_addKeyAndText("db_249description", "249 - Servant Droid");
	gui_addKeyAndText("db_249className", "Servant");
	gui_addKeyAndText("db_249drive", "Tripedal");
	gui_addKeyAndText("db_249brain", "Neutronic");
	gui_addKeyAndText("db_249weapon", "None");
	gui_addKeyAndText("db_249sensor1", "Spectral");
	gui_addKeyAndText("db_249sensor2", "~");
	gui_addKeyAndText("db_249sensor3", "~");
	gui_addKeyAndText("db_249notes", "Cheaper version of the anti-grav servant robot.");

	gui_addKeyAndText("db_296description", "296 - Servant Droid");
	gui_addKeyAndText("db_296className", "Servant");
	gui_addKeyAndText("db_296drive", "Tracks");
	gui_addKeyAndText("db_296brain", "Neutronic");
	gui_addKeyAndText("db_296weapon", "None");
	gui_addKeyAndText("db_296sensor1", "Spectral");
	gui_addKeyAndText("db_296sensor2", "~");
	gui_addKeyAndText("db_296sensor3", "~");
	gui_addKeyAndText("db_296notes", "This robot is mainly used for serving drinks. A tray is mounted on the head. Built by Orchard and Marsden Enterprises.");

	gui_addKeyAndText("db_302description", "302 - Messenger Droid");
	gui_addKeyAndText("db_302className", "Messenger");
	gui_addKeyAndText("db_302drive", "Anti-grav");
	gui_addKeyAndText("db_302brain", "None");
	gui_addKeyAndText("db_302weapon", "None");
	gui_addKeyAndText("db_302sensor1", "Spectral");
	gui_addKeyAndText("db_302sensor2", "~");
	gui_addKeyAndText("db_302sensor3", "~");
	gui_addKeyAndText("db_302notes", "Common device for moving small packages. Clamp is mounted on the lower body.");

	gui_addKeyAndText("db_329description", "329 - Messenger Droid");
	gui_addKeyAndText("db_329className", "Messenger");
	gui_addKeyAndText("db_329drive", "Wheels");
	gui_addKeyAndText("db_329brain", "None");
	gui_addKeyAndText("db_329weapon", "None");
	gui_addKeyAndText("db_329sensor1", "Spectral");
	gui_addKeyAndText("db_329sensor2", "~");
	gui_addKeyAndText("db_329sensor3", "~");
	gui_addKeyAndText("db_329notes", "Early type messenger robot. Large wheels impede motion on small craft.");

	gui_addKeyAndText("db_420description", "420 - Maintenance Droid");
	gui_addKeyAndText("db_420className", "Maintenance");
	gui_addKeyAndText("db_420drive", "Tracks");
	gui_addKeyAndText("db_420brain", "Neutronic");
	gui_addKeyAndText("db_420weapon", "None");
	gui_addKeyAndText("db_420sensor1", "Spectral");
	gui_addKeyAndText("db_420sensor2", "~");
	gui_addKeyAndText("db_420sensor3", "~");
	gui_addKeyAndText("db_420notes", "Slow maintenance robot. Confined to drive maintenance during flight.");

	gui_addKeyAndText("db_476description", "476 - Maintenance Droid");
	gui_addKeyAndText("db_476className", "Maintenance");
	gui_addKeyAndText("db_476drive", "Anti-grav");
	gui_addKeyAndText("db_476brain", "Neutronic");
	gui_addKeyAndText("db_476weapon", "Lasers");
	gui_addKeyAndText("db_476sensor1", "Spectral");
	gui_addKeyAndText("db_476sensor2", "Infra-red");
	gui_addKeyAndText("db_476sensor3", "~");
	gui_addKeyAndText("db_476notes", "Ship maintenance robot. Fitted with multiple arms to carry out repairs to the ship efficiently. All craft built after the Jupiter Incident are supplied with a team of these.");

	gui_addKeyAndText("db_493description", "493 - Maintenance Droid");
	gui_addKeyAndText("db_493className", "Maintenance");
	gui_addKeyAndText("db_493drive", "Anti-grav");
	gui_addKeyAndText("db_493brain", "Neutronic");
	gui_addKeyAndText("db_493weapon", "None");
	gui_addKeyAndText("db_493sensor1", "Spectral");
	gui_addKeyAndText("db_493sensor2", "~");
	gui_addKeyAndText("db_493sensor3", "~");
	gui_addKeyAndText("db_493notes", "Slave maintenance droid. Standard version will carry its own toolbox.");

	gui_addKeyAndText("db_516description", "516 - Crew Droid");
	gui_addKeyAndText("db_516className", "Crew");
	gui_addKeyAndText("db_516drive", "Bipedal");
	gui_addKeyAndText("db_516brain", "Neutronic");
	gui_addKeyAndText("db_516weapon", "None");
	gui_addKeyAndText("db_516sensor1", "Spectral");
	gui_addKeyAndText("db_516sensor2", "~");
	gui_addKeyAndText("db_516sensor3", "~");
	gui_addKeyAndText("db_516notes", "Early crew droid. Able to carry out simple flight checks only. No longer supplied.");

	gui_addKeyAndText("db_571description", "571 - Crew Droid");
	gui_addKeyAndText("db_571className", "Crew");
	gui_addKeyAndText("db_571drive", "Bipedal");
	gui_addKeyAndText("db_571brain", "Neutronic");
	gui_addKeyAndText("db_571weapon", "None");
	gui_addKeyAndText("db_571sensor1", "Spectral");
	gui_addKeyAndText("db_571sensor2", "~");
	gui_addKeyAndText("db_571sensor3", "~");
	gui_addKeyAndText("db_571notes", "Standard crew droid. Supplied with the ship.");

	gui_addKeyAndText("db_598description", "598 - Crew Droid");
	gui_addKeyAndText("db_598className", "Crew");
	gui_addKeyAndText("db_598drive", "Bipedal");
	gui_addKeyAndText("db_598brain", "Neutronic");
	gui_addKeyAndText("db_598weapon", "None");
	gui_addKeyAndText("db_598sensor1", "Spectral");
	gui_addKeyAndText("db_598sensor2", "~");
	gui_addKeyAndText("db_598sensor3", "~");
	gui_addKeyAndText("db_598notes", "A highly sophisticated device. Able to control the Robo-Freighter on its own.");

	gui_addKeyAndText("db_614description", "614 - Sentinal Droid");
	gui_addKeyAndText("db_614className", "Sentinal");
	gui_addKeyAndText("db_614drive", "Bipedal");
	gui_addKeyAndText("db_614brain", "Neutronic");
	gui_addKeyAndText("db_614weapon", "Laser Rifle");
	gui_addKeyAndText("db_614sensor1", "Spectral");
	gui_addKeyAndText("db_614sensor2", "Subsonic");
	gui_addKeyAndText("db_614sensor3", "~");
	gui_addKeyAndText("db_614notes", "Low security sentinel droid. Used to protect areas of the ship from intruders. A slow but sure device.");

	gui_addKeyAndText("db_615description", "615 - Sentinal Droid");
	gui_addKeyAndText("db_615className", "Sentinal");
	gui_addKeyAndText("db_615drive", "Anti-grav");
	gui_addKeyAndText("db_615brain", "Neutronic");
	gui_addKeyAndText("db_615weapon", "Lasers");
	gui_addKeyAndText("db_615sensor1", "Spectral");
	gui_addKeyAndText("db_615sensor2", "Infra-red");
	gui_addKeyAndText("db_615sensor3", "~");
	gui_addKeyAndText("db_615notes", "Sophisticated sentinel droid. Only 2000 built by the Nicholson corporation. These are now very rare.");

	gui_addKeyAndText("db_629description", "629 - Sentinal Droid");
	gui_addKeyAndText("db_629className", "Sentinel");
	gui_addKeyAndText("db_629drive", "Tracks");
	gui_addKeyAndText("db_629brain", "Neutronic");
	gui_addKeyAndText("db_629weapon", "Lasers");
	gui_addKeyAndText("db_629sensor1", "Spectral");
	gui_addKeyAndText("db_629sensor2", "Subsonic");
	gui_addKeyAndText("db_629sensor3", "~");
	gui_addKeyAndText("db_629notes", "Slow sentinel droid. Lasers are built into the turret. These are mounted on a small tank body. May be fitted with an auto-cannon on the Gillen version.");

	gui_addKeyAndText("db_711description", "711 - Battle Droid");
	gui_addKeyAndText("db_711className", "Battle");
	gui_addKeyAndText("db_711drive", "Bipdeal");
	gui_addKeyAndText("db_711brain", "Neutronic");
	gui_addKeyAndText("db_711weapon", "Disrupter");
	gui_addKeyAndText("db_711sensor1", "Ultra-sonic");
	gui_addKeyAndText("db_711sensor2", "Radar");
	gui_addKeyAndText("db_711sensor3", "~");
	gui_addKeyAndText("db_711notes", "Heavy duty battle droid. Disruptor is built into the head. One of the first in service with the Military.");

	gui_addKeyAndText("db_742description", "742 - Battle Droid");
	gui_addKeyAndText("db_742className", "Battle");
	gui_addKeyAndText("db_742drive", "Bipedeal");
	gui_addKeyAndText("db_742brain", "Neutronic");
	gui_addKeyAndText("db_742weapon", "Disrupter");
	gui_addKeyAndText("db_742sensor1", "Spectral");
	gui_addKeyAndText("db_742sensor2", "Radar");
	gui_addKeyAndText("db_742sensor3", "~");
	gui_addKeyAndText("db_742notes", "This version is the one mainly used by the Military.");

	gui_addKeyAndText("db_751description", "751 - Battle Droid");
	gui_addKeyAndText("db_751className", "Battle");
	gui_addKeyAndText("db_751drive", "Bipedal");
	gui_addKeyAndText("db_751brain", "Neutronic");
	gui_addKeyAndText("db_751weapon", "Lasers");
	gui_addKeyAndText("db_751sensor1", "Spectral");
	gui_addKeyAndText("db_751sensor2", "~");
	gui_addKeyAndText("db_751sensor3", "~");
	gui_addKeyAndText("db_751notes", "Very heavy duty battle droid. Only a few have so far entered service. These are the most powerful battle units ever built.");

	gui_addKeyAndText("db_821description", "821 - Security Droid");
	gui_addKeyAndText("db_821className", "Security");
	gui_addKeyAndText("db_821drive", "Anti-grav");
	gui_addKeyAndText("db_821brain", "Neutronic");
	gui_addKeyAndText("db_821weapon", "Lasers");
	gui_addKeyAndText("db_821sensor1", "Spectral");
	gui_addKeyAndText("db_821sensor2", "Radar");
	gui_addKeyAndText("db_821sensor3", "Infra-red");
	gui_addKeyAndText("db_821notes", "A very reliable anti-grav unit is fitted onto this droid. It will patrol the ship and eliminate intruders as soon as detected by powerful sensors.");

	gui_addKeyAndText("db_834description", "834 - Security Droid");
	gui_addKeyAndText("db_834className", "Security");
	gui_addKeyAndText("db_834drive", "Anit-grav");
	gui_addKeyAndText("db_834brain", "Neutronic");
	gui_addKeyAndText("db_834weapon", "Lasers");
	gui_addKeyAndText("db_834sensor1", "Spectral");
	gui_addKeyAndText("db_834sensor2", "Radar");
	gui_addKeyAndText("db_834sensor3", "~");
	gui_addKeyAndText("db_834notes", "Early type anti-grav security droid. Fitted with an over-driven anti-grav unit. This droid is very fast but is not reliable.");

	gui_addKeyAndText("db_883description", "883 - Security Droid");
	gui_addKeyAndText("db_883className", "Security");
	gui_addKeyAndText("db_883drive", "Wheels");
	gui_addKeyAndText("db_883brain", "Neutronic");
	gui_addKeyAndText("db_883weapon", "Exterminator");
	gui_addKeyAndText("db_883sensor1", "Spectral");
	gui_addKeyAndText("db_883sensor2", "Radar");
	gui_addKeyAndText("db_883sensor3", "~");
	gui_addKeyAndText("db_883notes", "This droid was designed from archive data. For some unknown reason it instills great fear in Human adversaries.");

	gui_addKeyAndText("db_999description", "999 - Command cyborg");
	gui_addKeyAndText("db_999className", "Command");
	gui_addKeyAndText("db_999drive", "Anti-grav");
	gui_addKeyAndText("db_999brain", "Primode");
	gui_addKeyAndText("db_999weapon", "Lasers");
	gui_addKeyAndText("db_999sensor1", "Infra-red");
	gui_addKeyAndText("db_999sensor2", "Radar");
	gui_addKeyAndText("db_999sensor3", "Subsonic");
	gui_addKeyAndText("db_999notes", "Experimental command cyborg. Fitted with a new type of brain. Mounted on a Security Droid anti-grav unit for convenience. Warning: the influence device may not control a primode brain for long.");

	gui_addKeyAndText("TransTerm",	"Transmission Terminated.");
	gui_addKeyAndText("displayScoreTextBox", "High Scores");
	gui_addKeyAndText("1", "1");
	gui_addKeyAndText("2", "2");
	gui_addKeyAndText("3", "3");
	gui_addKeyAndText("4", "4");
	gui_addKeyAndText("5", "5");

	gui_addKeyAndText("editTextBox", "Enter your intials using the arrow keys.");

	gui_addKeyAndText("endGameTextBox", "Well done. You have finished the game.");
	gui_addKeyAndText("buttonEndGameYes", "Return to menu");

	gui_addKeyAndText("beamOn", "Beam On");

}
