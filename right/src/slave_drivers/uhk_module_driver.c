#include "i2c_addresses.h"
#include "i2c.h"
#include "slave_drivers/uhk_module_driver.h"
#include "slave_protocol.h"
#include "main.h"
#include "peripherals/test_led.h"
#include "bool_array_converter.h"
#include "crc16.h"

uhk_module_state_t UhkModuleStates[UHK_MODULE_MAX_COUNT];
uhk_module_phase_t uhkModulePhase = UhkModulePhase_SendKeystatesRequestCommand;
uint8_t txBuffer[2];
uint8_t rxBuffer[KEY_STATE_BUFFER_SIZE];

void UhkModuleSlaveDriver_Init(uint8_t uhkModuleId)
{
    uhk_module_state_t* uhkModuleState = UhkModuleStates + uhkModuleId;
    uhkModuleState->isTestLedOn = true;
    uhkModuleState->ledPwmBrightness = 0x64;
}

void UhkModuleSlaveDriver_Update(uint8_t uhkModuleId)
{
    uhk_module_state_t *uhkModuleInternalState = UhkModuleStates + uhkModuleId;

    switch (uhkModulePhase) {
        case UhkModulePhase_SendKeystatesRequestCommand:
            txBuffer[0] = SlaveCommand_GetKeyStates;
            I2cAsyncWrite(I2C_ADDRESS_LEFT_KEYBOARD_HALF, txBuffer, 1);
            uhkModulePhase = UhkModulePhase_ReceiveKeystates;
            break;
        case UhkModulePhase_ReceiveKeystates:
            I2cAsyncRead(I2C_ADDRESS_LEFT_KEYBOARD_HALF, rxBuffer, KEY_STATE_BUFFER_SIZE);
            uhkModulePhase = UhkModulePhase_SendPwmBrightnessCommand;
            break;
        case UhkModulePhase_SendPwmBrightnessCommand:
            if (CRC16_IsMessageValid(rxBuffer, KEY_STATE_SIZE)) {
                BoolBitsToBytes(rxBuffer, CurrentKeyStates[SLOT_ID_LEFT_KEYBOARD_HALF], LEFT_KEYBOARD_HALF_KEY_COUNT);
            }
            txBuffer[0] = SlaveCommand_SetLedPwmBrightness;
            txBuffer[1] = uhkModuleInternalState->ledPwmBrightness;
            I2cAsyncWrite(I2C_ADDRESS_LEFT_KEYBOARD_HALF, txBuffer, 2);
            uhkModulePhase = UhkModulePhase_SendTestLedCommand;
            break;
        case UhkModulePhase_SendTestLedCommand:
            txBuffer[0] = SlaveCommand_SetTestLed;
            txBuffer[1] = uhkModuleInternalState->isTestLedOn;
            I2cAsyncWrite(I2C_ADDRESS_LEFT_KEYBOARD_HALF, txBuffer, 2);
            uhkModulePhase = UhkModulePhase_SendKeystatesRequestCommand;
            break;
    }
}