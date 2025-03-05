// Macros and labels for pins and expressions used

// Potentiometer GPIOs

#define KP_POT    26
#define KI_POT    27
#define KD_POT    28
#define REF_POT   29

// ADC channels for potentiometers

#define KP_CH     (KP_POT - 26)
#define KI_CH     (KI_POT - 26)
#define KD_CH     (KD_POT - 26)
#define REF_CH    (REF_POT - 26)

// I2C GPIOs

#define SDA_PIN   4
#define SCL_PIN   5

// L298 GPIOs

#define DIR1_PIN  16
#define DIR2_PIN  17
#define PWM_PIN   18

// GPIO for switch

#define OPT_PIN   6