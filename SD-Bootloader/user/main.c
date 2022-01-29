#include "delay.h"
#include "flash.h"
#include "fat.h"
#include "bootloaders.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    SD_BootInit();

    BOOT_PORT->BSRR = BOOT_PIN;

    if (FAT_Init() == FuncOK)
        BootLoader_FromSDCard();

    IAP_LoadApp(); //Booting from old/new firmware
}
