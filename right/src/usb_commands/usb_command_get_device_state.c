#include "fsl_common.h"
#include "segment_display.h"
#include "usb_commands/usb_command_get_device_state.h"
#include "usb_protocol_handler.h"
#include "eeprom.h"
#include "peripherals/merge_sensor.h"
#include "slave_drivers/uhk_module_driver.h"
#include "usb_report_updater.h"
#include "timer.h"
#include "layer_switcher.h"
#include "slave_scheduler.h"

#define MODULE_CONNECTION_STATE(SLOT_ID) ( \
        Timer_GetElapsedTime(&ModuleConnectionStates[SLOT_ID].lastTimeConnected) < 350 ? \
        ModuleConnectionStates[SLOT_ID].moduleId : 0 \
    )

static uhk_error_state_t getErrorState()
{
    if (SegmentDisplay_SlotIsActive(SegmentDisplaySlot_Error)) {
        return UhkErrorState_Error;
    } else if (SegmentDisplay_SlotIsActive(SegmentDisplaySlot_Warn)) {
        return UhkErrorState_Warn;
    } else {
        return UhkErrorState_Fine;
    }
}

void UsbCommand_GetKeyboardState(void)
{
    SetUsbTxBufferUint8(1, IsEepromBusy);
    SetUsbTxBufferUint8(2, MERGE_SENSOR_IS_MERGED | (getErrorState() << 1));
    SetUsbTxBufferUint8(3, MODULE_CONNECTION_STATE(UhkModuleDriverId_LeftKeyboardHalf));
    SetUsbTxBufferUint8(4, MODULE_CONNECTION_STATE(UhkModuleDriverId_LeftModule));
    SetUsbTxBufferUint8(5, MODULE_CONNECTION_STATE(UhkModuleDriverId_RightModule));
    SetUsbTxBufferUint8(6, ActiveLayer | (ActiveLayer != LayerId_Base && !ActiveLayerHeld ? (1 << 7) : 0) ); //Active layer + most significant bit if layer is toggled
    LastUsbGetKeyboardStateRequestTimestamp = CurrentTime;
}
