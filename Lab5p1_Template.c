#include "init.h" // Always need init.h, otherwise nothing will work.
#include<stdint.h>
#include<stdlib.h>

DMA_HandleTypeDef hdma1;

int size = 10;
//uint32_t, uint16_t, uint8_t
uint32_t mem1dma[10] ;
uint32_t mem2dma[10] ;
uint32_t mem1soft[10] ;
uint32_t mem2soft[10] ;

void DMAInit();

void populate(uint32_t array[10], int x){

	for(int i = 0; i< size; i++){
		array[i] = x;
	}
}

int main(void)
{
	Sys_Init();

	DMAInit();

	populate(mem1dma, 7);
	populate(mem1soft, 7);
	populate(mem2dma, 5);
	populate(mem2soft, 5);



	printf("\033[0m\033[44;33m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);


	printf("software before: %d, %d\r\n", mem1soft[0], mem2soft[0]);
	printf("dma before: %d, %d\r\n", mem1dma[0], mem2dma[0]);


	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


	DWT->CYCCNT = 0; // Clear the cycle counter

	memcpy(mem2soft, mem1soft, size);



	uint32_t cycles = DWT->CYCCNT; // Store the cycle counter
	printf("Software version took %lu CPU cycles\r\n",cycles);
	fflush(stdout);
	DWT->CYCCNT = 0; // Clear the cycle counter
	//printf("Clearing and Moving the Cursor to Home took %lu CPU cycles\r\n",cycles);



	HAL_DMA_Start(&hdma1,  mem1dma,  mem2dma, size);

	HAL_DMA_PollForTransfer(&hdma1, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

	cycles = DWT->CYCCNT; // Store the cycle counter
	//printf("And printing the above took %lu CPU cycles\r\n",cycles);
	printf("DMA version took %lu CPU cycles\r\n",cycles);
	fflush(stdout);

	//printf("before: %d, %d\r\n", test1, test2);
	//HAL_DMA_Start_IT(&hdma1, &test1, &test2, 1);

	//printf("after: %d, %d\r\n", test1, test2);
	printf("software after: %d, %d\r\n", mem1soft[0], mem2soft[0]);
	printf("dma after: %d, %d\r\n", mem1dma[0], mem2dma[0]);



	while(1);
}

void DMAInit(){

	__HAL_RCC_DMA2_CLK_ENABLE();

	hdma1.Instance = DMA2_Stream0;

	//hdam1.Init.Channel = ;

	hdma1.Init.PeriphInc = DMA_PINC_ENABLE;
	//hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	//hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;


	hdma1.Init.Direction = DMA_MEMORY_TO_MEMORY;

	hdma1.Init.MemInc = DMA_MINC_ENABLE;

	hdma1.Init.Mode =  DMA_NORMAL;

	hdma1.Init.Priority = DMA_PRIORITY_HIGH;

	hdma1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	//hdma1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	//hdma1.Init.MemBurst = DMA_MBURST_SINGLE;
	//hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	//hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;

	HAL_DMA_Init(&hdma1);


	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);


}

/*
void DMA2_Stream0_IRQHandler(void){

	HAL_DMA_IRQHandler(&hdma1);
}

void DMATransferComplete(DMA_HandleTypeDef *hdma){

	asm("nop");

}

*/
