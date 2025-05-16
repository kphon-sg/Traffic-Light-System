#include <LPC17xx.h>

// Pin definitions for clarity
#define RED_LED     (1 << 0)   // P2.0 - Red LED
#define YELLOW_LED  (1 << 1)   // P2.1 - Yellow LED
#define GREEN_LED   (1 << 2)   // P2.2 - Green LED
#define BUTTON_PIN  (1 << 10)  // P2.10 - Push button (active low)

// Constants
#define AUTO_CHANGE_INTERVAL 5  // Seconds before auto-changing state
#define DEBOUNCE_MS 50          // Button debounce time in milliseconds
#define SYSTEM_CLOCK 25000000    // System clock frequency (25MHz)

// LED states
typedef enum {
    STATE_RED = 0,
    STATE_GREEN,
    STATE_YELLOW,
    STATE_COUNT  // For modulo operation
} LedState;

// Global variables
static volatile LedState current_state = STATE_RED;
static volatile uint32_t timer_count = 0;
static volatile uint8_t button_pressed = 0;

// Simple delay function for debouncing
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 10000; i++);
}

// Set LED based on state
void setLED(LedState state) {
    // Clear all LEDs first
    LPC_GPIO2->FIOCLR = RED_LED | YELLOW_LED | GREEN_LED;
    
    // Set the appropriate LED
    switch (state) {
        case STATE_RED:    LPC_GPIO2->FIOSET = RED_LED; break;
        case STATE_GREEN:  LPC_GPIO2->FIOSET = GREEN_LED; break;
        case STATE_YELLOW: LPC_GPIO2->FIOSET = YELLOW_LED; break;
        default:           LPC_GPIO2->FIOSET = RED_LED; break; // Default to red for safety
    }
}

// Timer0 interrupt handler (triggers every 1 second)
void TIMER0_IRQHandler(void) {
    if (LPC_TIM0->IR & 0x1) {  // Check MR0 interrupt
        timer_count++;
        if (timer_count >= AUTO_CHANGE_INTERVAL) {
            current_state = (LedState)((current_state + 1) % STATE_COUNT);
            setLED(current_state);
            timer_count = 0;
        }
        LPC_TIM0->IR = 1;  // Clear interrupt flag
    }
}

// External interrupt handler for button press with debouncing
void EINT3_IRQHandler(void) {
    if (LPC_GPIOINT->IO2IntStatF & BUTTON_PIN) {  // Check falling edge
        if (!button_pressed && !(LPC_GPIO2->FIOPIN & BUTTON_PIN)) {
            button_pressed = 1;
            current_state = (LedState)((current_state + 1) % STATE_COUNT);
            setLED(current_state);
            timer_count = 0;
            delay_ms(DEBOUNCE_MS);  // Simple debounce
        } else {
            button_pressed = 0;
        }
        LPC_GPIOINT->IO2IntClr = BUTTON_PIN;  // Clear interrupt
    }
}

// Initialize GPIO pins
void initGPIO(void) {
    // Configure LED pins as output
    LPC_GPIO2->FIODIR |= RED_LED | YELLOW_LED | GREEN_LED;
    
    // Configure button pin as input
    LPC_GPIO2->FIODIR &= ~BUTTON_PIN;
    LPC_GPIO2->FIOMASK &= ~BUTTON_PIN;
    
    // Set P2.10 as GPIO
    LPC_PINCON->PINSEL4 &= ~(3 << 20);
}

// Initialize Timer0 for 1-second intervals
void initTimer(void) {
    // Enable power to Timer0
    LPC_SC->PCONP |= (1 << 1);
    
    // Configure timer
    LPC_TIM0->MR0 = SYSTEM_CLOCK;  // 1 second at 25MHz
    LPC_TIM0->MCR = 3;             // Interrupt and reset on MR0
    LPC_TIM0->TCR = 1;             // Enable timer
    
    // Enable Timer0 interrupt
    NVIC_EnableIRQ(TIMER0_IRQn);
}

// Initialize button interrupt
void initButtonInterrupt(void) {
    // Enable falling edge interrupt for button
    LPC_GPIOINT->IO2IntEnF |= BUTTON_PIN;
    
    // Enable EINT3 interrupt
    NVIC_EnableIRQ(EINT3_IRQn);
}

// Main function
int main(void) {
    // Initialize peripherals
    initGPIO();
    initTimer();
    initButtonInterrupt();
    
    // Set initial LED state
    setLED(current_state);
    
    // Enter infinite loop; interrupts handle functionality
    while (1) {
        __WFI();  // Wait for interrupt to save power
    }
    
    return 0;  // Will never reach here
}