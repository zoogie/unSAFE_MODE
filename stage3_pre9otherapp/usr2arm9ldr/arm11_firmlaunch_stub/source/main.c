#include "types.h"

#define LCD_REGS_BASE           0x10202000

#define LCD_TOP_FILL_REG        *(vu32 *)(LCD_REGS_BASE + 0x200 + 4)
#define LCD_BOTTOM_FILL_REG     *(vu32 *)(LCD_REGS_BASE + 0xA00 + 4)
#define LCD_FILL_ENABLE         (1u << 24)

#define SCREEN_TOP_WIDTH     400
#define SCREEN_BOTTOM_WIDTH  320
#define SCREEN_HEIGHT        240
#define SCREEN_TOP_FBSIZE    (3 * SCREEN_TOP_WIDTH * SCREEN_HEIGHT)
#define SCREEN_BOTTOM_FBSIZE (3 * SCREEN_BOTTOM_WIDTH * SCREEN_HEIGHT)

struct fb {
     u8 *top_left;
     u8 *top_right;
     u8 *bottom;
} ;

static const struct fb fbs[2] =
{
    {
        .top_left  = (u8 *)0x18300000,
        .top_right = (u8 *)0x18300000,
        .bottom    = (u8 *)0x18346500,
    },
    {
        .top_left  = (u8 *)0x18400000,
        .top_right = (u8 *)0x18400000,
        .bottom    = (u8 *)0x18446500,
    },
};

static void setupFramebuffers(const struct fb *fbs)
{
    *(vu32 *)0x10400468 = (u32)fbs[0].top_left;
    *(vu32 *)0x1040046c = (u32)fbs[1].top_left;
    *(vu32 *)0x10400494 = (u32)fbs[0].top_right;
    *(vu32 *)0x10400498 = (u32)fbs[1].top_right;
    *(vu32 *)0x10400568 = (u32)fbs[0].bottom;
    *(vu32 *)0x1040056c = (u32)fbs[1].bottom;
}

static void clearScreens(const struct fb *fb)
{
    //Setting up two simultaneous memory fills using the GPU

    vu32 *REGs_PSC0 = (vu32 *)0x10400010,
         *REGs_PSC1 = (vu32 *)0x10400020;

    REGs_PSC0[0] = (u32)fb->top_left >> 3; //Start address
    REGs_PSC0[1] = (u32)(fb->top_left + SCREEN_TOP_FBSIZE) >> 3; //End address
    REGs_PSC0[2] = 0; //Fill value
    REGs_PSC0[3] = (2 << 8) | 1; //32-bit pattern; start

    REGs_PSC1[0] = (u32)fb->bottom >> 3; //Start address
    REGs_PSC1[1] = (u32)(fb->bottom + SCREEN_BOTTOM_FBSIZE) >> 3; //End address
    REGs_PSC1[2] = 0; //Fill value
    REGs_PSC1[3] = (2 << 8) | 1; //32-bit pattern; start

    while(!((REGs_PSC0[3] & 2) && (REGs_PSC1[3] & 2)));
}

void error(void)
{
    // Fill the screens with red
    LCD_TOP_FILL_REG = LCD_FILL_ENABLE | 0xFF;
    LCD_BOTTOM_FILL_REG = LCD_FILL_ENABLE | 0xFF;
    while(true);
}

void main(void)
{
    // Fill the screens with black
    LCD_TOP_FILL_REG = LCD_FILL_ENABLE | 0;
    LCD_BOTTOM_FILL_REG = LCD_FILL_ENABLE | 0;

    setupFramebuffers(fbs);
    clearScreens(&fbs[0]);
    clearScreens(&fbs[1]);

    LCD_TOP_FILL_REG = 0;
    LCD_BOTTOM_FILL_REG = 0;
}
