
/* Port Map Defines for XA-SK-AUDIO ("Audio Slice") 1v0 */
#define PORT_I2S_DAC0	                XS1_PORT_1D
#define PORT_I2S_DAC1                   XS1_PORT_1M
#define PORT_I2S_ADC0                   XS1_PORT_1I
#define PORT_I2S_ADC1	                XS1_PORT_1L
#define PORT_I2S_LRCLK		            XS1_PORT_1H
#define PORT_I2S_BCLK		            XS1_PORT_1A
#define PORT_MCLK_IN		            XS1_PORT_1E

#define PORT_GPIO			            XS1_PORT_4E
#define PORT_I2C                        XS1_PORT_4F

/* General output port bit definitions */
#define P_GPIO_SS_EN_CTRL       0x01    /* SPI Slave Select Enable. 0 - SPI SS Enabled, 1 - SPI SS Disabled. */
#define P_GPIO_MCLK_SEL         0x02    /* MCLK frequency select. 0 - 22.5792MHz, 1 - 24.576MHz. */
#define P_GPIO_COD_RST_N        0x04    /* CODEC RESET. Active low. */
#define P_GPIO_LED              0x08    /* LED. Active high. */
