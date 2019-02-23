#include <hdr/gui/gui_main.h>
#include <hdr/system/sys_audio.h>
#include "hdr/game/gam_transfer.h"

uint          droidToTransferInto;
SDL_TimerID   currentTimerEventID;

//----------------------------------------------------------------------------
//
// Start the transfer process by starting the first event timer
void gam_initTransfer (uint whichDroid)
//----------------------------------------------------------------------------
{
  _myEventData testEvent;

  evt_sendEvent (USER_EVENT_AUDIO, AUDIO_STOP_SAMPLE, SND_TRANSFER_1, 0, 0, glm::vec2 (), glm::vec2 (), "");

//  evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_TRANSFER_START, 0, 0, glm::vec2 (), glm::vec2 (), "");

  droidToTransferInto = whichDroid;

  printf ("Transfer into droid [ %i ]\n", droidToTransferInto);

  testEvent.eventType = USER_EVENT_TRANSFER;
  testEvent.eventAction = MODE_TRANSFER_INTRO_1;
  testEvent.data1 = 2000;

  currentGUIScreen = gui_findIndex (GUI_OBJECT_SCREEN, "scrTransferIntro0");
  sys_changeMode (MODE_TRANSFER_INTRO_0);

  currentTimerEventID = evt_registerTimerAndEvent (testEvent.data1, testEvent, "MODE_TRANSFER_INTRO_0");
}

//----------------------------------------------------------------
//
// Handle an transfer game user event - called from Transfer Thread
int gam_processTransferEventQueue (void *ptr)
//----------------------------------------------------------------
{
  _myEventData tempEventData;
  _myEventData testEvent;

  while (runThreads)
    {
      SDL_Delay (THREAD_DELAY_MS);

      if (!transferEventQueue.empty ())   // stuff in the queue to process
        {
          if (SDL_LockMutex (transferMutex) == 0)
            {
              tempEventData = transferEventQueue.front ();
              transferEventQueue.pop ();
              SDL_UnlockMutex (transferMutex);
            }

          switch (tempEventData.eventAction)
            {
              case USER_EVENT_KEY_EVENT:
                gam_transferHandleInputEvent (tempEventData.data1, tempEventData.data2, tempEventData.data3);
                break;

              case MODE_TRANSFER_INTRO_0:
                break;

              case MODE_TRANSFER_INTRO_1:

                printf ("In event Intro 1 - start sound\n");

                evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_TRANSFER_STAGE_1, 0, 0, glm::vec2 (), glm::vec2 (), "");

                currentGUIScreen = gui_findIndex (GUI_OBJECT_SCREEN, "scrTransferIntro1");
                sys_changeMode (MODE_TRANSFER_INTRO_1);

                if ((!audioAvailable) || (!as_useSound)) {
                    testEvent.eventType = USER_EVENT_TRANSFER;
                    testEvent.eventAction = MODE_TRANSFER_INTRO_2;
                    testEvent.data1 = 2000;

                    currentTimerEventID = evt_registerTimerAndEvent(static_cast<Uint32>(testEvent.data1), testEvent,
                                                                    "TRANSFER_EVENT_INTRO_2");
                }

              break;

              case MODE_TRANSFER_INTRO_2:

                  printf ("In event Intro 2 - start sound\n");

                  evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_TRANSFER_STAGE_2, 0, 0, glm::vec2 (), glm::vec2 (), "");

                currentGUIScreen = gui_findIndex (GUI_OBJECT_SCREEN, "scrTransferIntro2");
                sys_changeMode (MODE_TRANSFER_INTRO_2);

                    if ((!audioAvailable) || (!as_useSound)) {
                      testEvent.eventType = USER_EVENT_TRANSFER;
                      testEvent.eventAction = MODE_TRANSFER_SELECT_SIDE;
                      testEvent.data1 = 2000;

                      currentTimerEventID = evt_registerTimerAndEvent(static_cast<Uint32>(testEvent.data1), testEvent,
                                                                      "MODE_TRANSFER_SELECT_SIDE");
                    }

              break;

                case MODE_TRANSFER_SELECT_SIDE:
                  printf ("In event MODE_TRANSFER_SELECT_SIDE - start sound\n");

                    evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_TRANSFER_START, 0, 0, glm::vec2 (), glm::vec2 (), "");

                    currentGUIScreen = gui_findIndex (GUI_OBJECT_SCREEN, "scrTransferStart");
                    sys_changeMode (MODE_TRANSFER_SELECT_SIDE);
                  break;

              default:
                break;
            }
        }
    }
  printf ("TRANSFER thread stopped.\n");
  return 0;
}

//-----------------------------------------------------------------------------
//
// Handle an input event while in transfer mode
void gam_transferHandleInputEvent (int eventAction, int eventType, int eventSource)
//-----------------------------------------------------------------------------
{
    _myEventData testEvent;

  switch (currentMode)
    {
      case MODE_TRANSFER_INTRO_0:         // Show commencing message
          if ((eventAction == MY_INPUT_ACTION_PRESS) && ( eventType == MY_INPUT_ACTION))        // Move to next screen
          {
              evt_removeTimerAndEvent(currentTimerEventID);

              currentGUIScreen = gui_findIndex(GUI_OBJECT_SCREEN, "scrTransferIntro1");
              sys_changeMode(MODE_TRANSFER_INTRO_1);

              testEvent.eventType = USER_EVENT_TRANSFER;
              testEvent.eventAction = MODE_TRANSFER_INTRO_1;
              testEvent.data1 = 4000;

              currentTimerEventID = evt_registerTimerAndEvent(static_cast<Uint32>(testEvent.data1), testEvent, "MODE_TRANSFER_INTRO_1");
          }

            break;

      case MODE_TRANSFER_INTRO_1:       // Show current droid information
      /*
        if ((eventAction == MY_INPUT_ACTION_PRESS) && ( eventType == MY_INPUT_ACTION))        // Move to next screen
        {
            evt_removeTimerAndEvent(currentTimerEventID);
            currentTimerEventID = -1;

            testEvent.eventType = USER_EVENT_TRANSFER;
            testEvent.eventAction = TRANSFER_EVENT_INTRO_2;
            testEvent.data1 = 4000;

            currentTimerEventID = evt_registerTimerAndEvent (static_cast<Uint32>(testEvent.data1), testEvent, "TRANSFER_EVENT_INTRO_2");
        }
        */
        break;

      case MODE_TRANSFER_INTRO_2:       // Show target droid information
        break;

      case MODE_TRANSFER_START:         //
        break;

      case MODE_TRANSFER_SELECT_SIDE:   // Countdown to choose side
        break;

      case MODE_TRANSFER_SELECT:        // Play the transfer game
        break;

      case MODE_TRANSFER_LOST:          // Lost the transfer
        break;

      case MODE_TRANSFER_COPY:          // Won the transfer
        break;

      case MODE_TRANSFER_DEADLOCK:      // Deadlock - start again
        break;

      case MODE_TRANSFER_FINISH:        // Clean up
        break;

      default:
        break;
    }
}

//----------------------------------------------------------------
//
// Check for input or sound ending for Transfer
void gam_processTransfer()
//----------------------------------------------------------------
{
  _myEventData    testEvent;

  switch (currentMode)
    {
      case MODE_TRANSFER_INTRO_0:
        break;

      case MODE_TRANSFER_INTRO_1:
        if ((audioAvailable) || (as_useSound))
        {
          if (!aud_isSoundPlaying(SND_TRANSFER_STAGE_1))
          {
              printf ("Intro 1 - sound stopped playing - remove timer [ %i ]\n", currentTimerEventID);

            evt_removeTimerAndEvent(currentTimerEventID);
            currentTimerEventID = -1;

            testEvent.eventType = USER_EVENT_TRANSFER;
            testEvent.eventAction = MODE_TRANSFER_INTRO_2;
            testEvent.data1 = 2000;

            currentTimerEventID = evt_registerTimerAndEvent(static_cast<Uint32>(testEvent.data1), testEvent,
                                                            "MODE_TRANSFER_INTRO_2");

            printf ("Intro 1 - new timerID [ %i ]\n", currentTimerEventID);

            sys_changeMode(MODE_TRANSFER_INTRO_2);
          }
        }
        break;

      case MODE_TRANSFER_INTRO_2:
        if ((audioAvailable) || (as_useSound))
        {
          if (!aud_isSoundPlaying(SND_TRANSFER_STAGE_2))
          {
            printf ("Intro 2 - sound stopped playing - remove timer [ %i ]\n", currentTimerEventID);

            evt_removeTimerAndEvent(currentTimerEventID);
            currentTimerEventID = -1;

            testEvent.eventType = USER_EVENT_TRANSFER;
            testEvent.eventAction = MODE_TRANSFER_SELECT_SIDE;
            testEvent.data1 = 2000;

            currentTimerEventID = evt_registerTimerAndEvent(static_cast<Uint32>(testEvent.data1), testEvent,
                                                            "MODE_TRANSFER_SELECT_SIDE");

            printf ("Intro 2 - new timerID [ %i ]\n", currentTimerEventID);

            sys_changeMode(MODE_TRANSFER_SELECT_SIDE);
          }
        }
        break;

        case MODE_TRANSFER_SELECT_SIDE:
          break;

      default:
        break;
    }
}