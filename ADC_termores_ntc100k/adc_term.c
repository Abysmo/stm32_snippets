#include "adc_term.h"


void gpio_ADC_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef ADC_GPIO;

    //ADC IN
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA;
    ADC_GPIO.GPIO_Pin   = GPIO_Pin_1;        //ADC1
    ADC_GPIO.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &ADC_GPIO);


    //clock for ADC (max 14MHz) (72 / 6 = 12)
    RCC_ADCCLKConfig (RCC_PCLK2_Div6);
    //enable ADC system clock
    ADC_DeInit(ADC1); //reset adc values
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    //ADC_StructInit(&ADC_InitStructure);   // Initialize ADC structure (important !)

    // define ADC config
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //single chanel
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // one time run
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1 ,ADC_SampleTime_28Cycles5); // define regular conversion config
    ADC_Init (ADC1, &ADC_InitStructure);   //set config of ADC1
    ADC_Cmd (ADC1,ENABLE);
    calibrate_ADC();
}

void calibrate_ADC()
{
	ADC_Cmd (ADC1,ENABLE);
    ADC_ResetCalibration(ADC1);						// Reset previous calibration
    while(ADC_GetResetCalibrationStatus(ADC1)){};	// wait untill calibration reset ends
	ADC_StartCalibration(ADC1);						//calibrate ADC
	while(ADC_GetCalibrationStatus(ADC1)){};		// wait untill calibration end
}


uint16_t get_ADC_val(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))); //wait until conversation ends
	return ADC_GetConversionValue(ADC1);
}

int get_temp_c(uint8_t samples)
{
	uint8_t cycles_done = 0;
	uint16_t adc_midval = 0;

	do
	{
	adc_midval += get_ADC_val(); //increase total ADC value
	cycles_done++; //increase cycles
	samples--; //decrease samples amount
	}
	while(samples);
	adc_midval /= cycles_done; //calculate middle value from all measurements

	double rThermistor = 0;				// Holds thermistor resistance value
	double tKelvin     = 0;				// Holds calculated temperature
	int tCelsius	   = 0;				// Hold temperature in celsius
	rThermistor = BALANCE_RESISTOR * ((MAX_ADC / adc_midval) - 1);
	tKelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(rThermistor / RESISTOR_ROOM_TEMP)));
	tCelsius = tKelvin - 273.15;		// convert kelvin to celsius
	return tCelsius;					// Return temperature in Celsius
}

