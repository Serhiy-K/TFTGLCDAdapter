#include "flash.h"

#if FLASH_PROG_TEST_EN
uint32_t WriteBuf[FLASH_PAGE_SIZE / 4];
#endif  //#if FLASH_PROG_TEST_EN

FStat Flash_Init(void)
{
    FLASH_Unlock();
    return FuncOK;
}

FStat Flash_Write(uint32_t Addr, void *WriteBuf, uint32_t Len)
{
    FLASH_Status stat;
    uint32_t *buf;
    uint32_t i;
    
    if ( FuncOK != Flash_Erase( Addr, Len ) )   return FuncErr;
    
    buf = (uint32_t *)WriteBuf;
    for ( i = 0; i < Len / 4; i++ )
    {
        stat = FLASH_ProgramWord( Addr + i * 4, buf[i] );
        if (FLASH_COMPLETE != (stat))
            return FuncErr;
    }
    
    for ( i = 0; i < Len / 4; i++ )
    {
        if ( *((uint32_t *)(Addr + i * 4)) != buf[i] )  return FuncErr;
    }
    
    return FuncOK;
}

FStat Flash_Erase(uint32_t Addr, uint32_t Len)
{
    FLASH_Status stat;
    uint32_t erase_addr;
    
    if ( 0 == Addr % FLASH_PAGE_SIZE )
    {
        stat = FLASH_ErasePage( Addr );
        if (FLASH_COMPLETE != (stat))   return FuncErr;
    }
    
    erase_addr = ( Addr + FLASH_PAGE_SIZE ) - ( Addr % FLASH_PAGE_SIZE );
    for ( ; erase_addr < Addr + Len; erase_addr += FLASH_PAGE_SIZE )
    {
        stat = FLASH_ErasePage( erase_addr );
        if (FLASH_COMPLETE != (stat))
            return FuncErr;
    }
    
    for ( erase_addr = Addr; erase_addr < Addr + Len; erase_addr += 4)
    {
        if ( FLASH_EMPTY != *((uint32_t *)erase_addr) ) return FuncErr;
    }
    
    return FuncOK;
}
