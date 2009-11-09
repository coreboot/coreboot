

#define LED_MESSAGE_START       0xFF
#define LED_MESSAGE_FINISH      0x99
#define LED_MESSAGE_RAM         0x01


#ifdef TECHNEXION_EARLY_SETUP

// TechNexion's Post Code Initially.
void technexion_post_code_init(void);

#endif

void technexion_post_code(uint8_t udata8);
