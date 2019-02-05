
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
    gui_addKeyAndText("buttonPrevious",		"Previous");
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

    gui_addKeyAndText("db_001", "Description : 001 - Influence Device ! ClassName : Influence ! Drive  : None ! Brain : None ! Weapon : Lasers ! Sensor 1 : ~ ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Robot activity influence device. This helmet is self-powered and will control any robot for a short time. Lasers are turret mounted. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_123", "Description : 123 - Disposal Droid ! ClassName : Disposal ! Drive  : Tracks ! Brain : None ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : Infra-red ! Sensor 3 : ~ ! Notes : Simple rubbish disposal robot. Common device in most space craft to maintain a clean ship. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_139", "Description : 139 - Disposal Droid ! ClassName : Disposal ! Drive  : Anti-grav ! Brain : None ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Created by Dr. Masternak to clean up large heaps of rubbish. Its scoop is used to collect rubbish. It is then crushed internally. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_247", "Description : 247 - Servant Droid ! ClassName : Servant ! Drive  : Anti-grav ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Light duty servant robot. One of the first to use the anti-grav system. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_249", "Description : 249 - Servant Droid ! ClassName : Servant ! Drive  : Tripedal ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Cheaper version of the anti-grav servant robot. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_296", "Description : 296 - Servant Droid ! ClassName : Servant ! Drive  : Tracks ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : This robot is mainly used for serving drinks. A tray is mounted on the head. Built by Orchard and Marsden Enterprises. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_302", "Description : 302 - Messenger Droid ! ClassName : Messenger ! Drive  : Anti-grav ! Brain : None ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Common device for moving small packages. Clamp is mounted on the lower body. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_329", "Description : 329 - Messenger Droid ! ClassName : Messenger ! Drive  : Wheels ! Brain : None ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Early type messenger robot. Large wheels impede motion on small craft. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_420", "Description : 420 - Maintenance Droid ! ClassName : Maintenance ! Drive  : Tracks ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Slow maintenance robot. Confined to drive maintenance during flight. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_476", "Description : 476 - Maintenance Droid ! ClassName : Maintenance ! Drive  : Anti-grav ! Brain : Neutronic ! Weapon : Lasers ! Sensor 1 : Spectral ! Sensor 2 : Infra-red ! Sensor 3 : ~ ! Notes : Ship maintenance robot. Fitted with multiple arms to carry out repairs to the ship efficiently. All craft built after the Jupiter Incident are supplied with a team of these. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_493", "Description : 493 - Maintenance Droid ! ClassName : Maintenance ! Drive  : Anti-grav ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Slave maintenance droid. Standard version will carry its own toolbox. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_516", "Description : 516 - Crew Droid ! ClassName : Crew ! Drive  : Bipedal ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Early crew droid. Able to carry out simple flight checks only. No longer supplied. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_571", "Description : 571 - Crew Droid ! ClassName : Crew ! Drive  : Bipedal ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Standard crew droid. Supplied with the ship. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_598", "Description : 598 - Crew Droid ! ClassName : Crew ! Drive  : Bipedal ! Brain : Neutronic ! Weapon : None ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : A highly sophisticated device. Able to control the Robo-Freighter on its own. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_614", "Description : 614 - Sentinal Droid ! ClassName : Sentinal ! Drive  : Bipedal ! Brain : Neutronic ! Weapon : Laser Rifle ! Sensor 1 : Spectral ! Sensor 2 : Subsonic ! Sensor 3 : ~ ! Notes : Low security sentinel droid. Used to protect areas of the ship from intruders. A slow but sure device. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_615", "Description : 615 - Sentinal Droid ! ClassName : Sentinal ! Drive  : Anti-grav ! Brain : Neutronic ! Weapon : Lasers ! Sensor 1 : Spectral ! Sensor 2 : Infra-red ! Sensor 3 : ~ ! Notes : Sophisticated sentinel droid. Only 2000 built by the Nicholson corporation. These are now very rare. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_629", "Description : 629 - Sentinal Droid ! ClassName : Sentinel ! Drive  : Tracks ! Brain : Neutronic ! Weapon : Lasers ! Sensor 1 : Spectral ! Sensor 2 : Subsonic ! Sensor 3 : ~ ! Notes : Slow sentinel droid. Lasers are built into the turret. These are mounted on a small tank body. May be fitted with an auto-cannon on the Gillen version. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_711", "Description : 711 - Battle Droid ! ClassName : Battle ! Drive  : Bipdeal ! Brain : Neutronic ! Weapon : Disrupter ! Sensor 1 : Ultra-sonic ! Sensor 2 : Radar ! Sensor 3 : ~ ! Notes : Heavy duty battle droid. Disruptor is built into the head. One of the first in service with the Military. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_742", "Description : 742 - Battle Droid ! ClassName : Battle ! Drive  : Bipedeal ! Brain : Neutronic ! Weapon : Disrupter ! Sensor 1 : Spectral ! Sensor 2 : Radar ! Sensor 3 : ~ ! Notes : This version is the one mainly used by the Military. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_751", "Description : 751 - Battle Droid ! ClassName : Battle ! Drive  : Bipedal ! Brain : Neutronic ! Weapon : Lasers ! Sensor 1 : Spectral ! Sensor 2 : ~ ! Sensor 3 : ~ ! Notes : Very heavy duty battle droid. Only a few have so far entered service. These are the most powerful battle units ever built. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_821", "Description : 821 - Security Droid ! ClassName : Security ! Drive  : Anti-grav ! Brain : Neutronic ! Weapon : Lasers ! Sensor 1 : Spectral ! Sensor 2 : Radar ! Sensor 3 : Infra-red ! Notes : A very reliable anti-grav unit is fitted onto this droid. It will patrol the ship and eliminate intruders as soon as detected by powerful sensors. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_834", "Description : 834 - Security Droid ! ClassName : Security ! Drive  : Anti-grav ! Brain : Neutronic ! Weapon : Lasers ! Sensor 1 : Spectral ! Sensor 2 : Radar ! Sensor 3 : ~ ! Notes : Early type anti-grav security droid. Fitted with an over-driven anti-grav unit. This droid is very fast but is not reliable. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_883", "Description : 883 - Security Droid ! ClassName : Security ! Drive  : Wheels ! Brain : Neutronic ! Weapon : Exterminator ! Sensor 1 : Spectral ! Sensor 2 : Radar ! Sensor 3 : ~ ! Notes : This droid was designed from archive data. For some unknown reason it instills great fear in Human adversaries. ! ! ! ! ! ! Information Repeats ! ! ! ! !");
    gui_addKeyAndText("db_999", "Description : 999 - Command cyborg ! ClassName : Command ! Drive  : Anti-grav ! Brain : Primode ! Weapon : Lasers ! Sensor 1 : Infra-red ! Sensor 2 : Radar ! Sensor 3 : Subsonic ! Notes : Experimental command cyborg. Fitted with a new type of brain. Mounted on a Security Droid anti-grav unit for convenience. Warning: the influence device may not control a primode brain for long. ! ! ! ! ! ! Information Repeats ! ! ! ! !");


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
