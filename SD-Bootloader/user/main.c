#include "delay.h"
#include "flash.h"
#include "fat.h"
#include "bootloaders.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    SD_BootInit();
    Flash_Init(); //Going to load new firmware

    BOOT_PORT->BSRR = BOOT_PIN;

    if (FAT_Init() == FuncOK)
        BootLoader_FromSDCard();

    IAP_LoadApp(BOOTLOADER_FLASH_ADDR); //Booting from old/new firmware
}
