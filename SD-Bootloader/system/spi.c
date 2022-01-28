#include "sys.h"
#include "spi.h"

SPI_InitTypeDef  SPI_InitStructure;

void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(	SPI_RCC, ENABLE );
	RCC_APB2PeriphClockCmd(	SPI_GPIO_RCC, ENABLE );

	GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_MISO | SPI_SCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);

	GPIO_SetBits(SPI_PORT, SPI_MOSI | SPI_MISO | SPI_SCK);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI, &SPI_InitStructure);
	SPI_Cmd(SPI, ENABLE);
	SPI2_ReadWriteByte(0xff);
}

void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
	SPI_Init(SPI, &SPI_InitStructure);
	SPI_Cmd(SPI, ENABLE);
}

u8 SPI2_ReadWriteByte(u8 TxData)
{
	u8 retry = 0;
	while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if (retry > 200) return 0;
	}
	SPI_I2S_SendData(SPI, TxData);
	retry = 0;

	while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		if (retry > 200) return 0;
	}
	return SPI_I2S_ReceiveData(SPI);
}
