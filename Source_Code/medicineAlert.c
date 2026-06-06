#include <lpc214x.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define SLAVE_ADDR 0x4E

#define AA 2
#define SI 3
#define STO 4
#define STA 5
#define I2EN 6

char arr[50];
unsigned char current_mode = 0;


int hour, min;


#define MAX_ALARMS 9   /* easy because delete/select can use one key 1-9 */

typedef struct
{
    unsigned char medicine_no;   /* serial number: 1, 2, 3 ... */
    unsigned char hour;          /* store in decimal, not BCD */
    unsigned char minute;        /* store in decimal, not BCD */
    unsigned char active;        /* 1 = valid alarm, 0 = empty */
} Alarm;

Alarm alarms[MAX_ALARMS];
unsigned char alarm_count = 0;

volatile unsigned char alarm_ringing = 0;
volatile unsigned char active_alarm_index = 0xFF;

void Check_Alarm(void);
void Alarm_On(unsigned char index);
void Alarm_Snooze_10s(void);

#define MODE_MENU        0
#define MODE_VIEW_ALARM  1
#define MODE_ADD_ALARM   2
#define MODE_DELETE_ALARM 3
#define MODE_VIEW_TIME   5
#define MODE_VIEW_TEMP   6

void wait(int count);
void delay_ms(int count);

void serial(void);
void UART0_SendChar(char ch);
void UART0_SendString(char *str);
void ShowMenu(void);

void I2C_Init(void);
int I2C_Start(void);

void senddata(char data);
void LCD_Command(char cmnd);
void LCD_Char(char data);
void LCD_String(char *str);
void LCD_Init(void);


unsigned int Decimal_To_BCD(unsigned int value)
{
    return ((value / 10) << 4) | (value % 10);
}

unsigned int BCD_To_Decimal(unsigned int value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}


void RTC_Delay_Seconds(unsigned int seconds)
{
    unsigned int start_sec;

    start_sec = SEC;

    while((SEC - start_sec) < seconds);
}



void selected_key_7segment(unsigned char key)
{
    unsigned int da[16] =
    {
        0x003F0000,
        0x00060000,
        0x005B0000,
        0x004F0000,
        0x00660000,
        0x006D0000,
        0x007D0000,
        0x00070000,
        0x007F0000,
        0x006F0000,
        0x00770000,
        0x007C0000,
        0x00390000,
        0x005E0000,
        0x00790000,
        0x00710000
    };

    IOCLR0 = 0x00FF0000;

    if(key == '0')
        IOSET0 = da[0];

    else if(key == '1')
        IOSET0 = da[1];

    else if(key == '2')
        IOSET0 = da[2];

    else if(key == '3')
        IOSET0 = da[3];

    else if(key == '4')
        IOSET0 = da[4];

    else if(key == '5')
        IOSET0 = da[5];

    else if(key == '6')
        IOSET0 = da[6];

    else if(key == '7')
        IOSET0 = da[7];

    else if(key == '8')
        IOSET0 = da[8];

    else if(key == '9')
        IOSET0 = da[9];

    else if(key == 'A')
        IOSET0 = da[10];

    else if(key == 'B')
        IOSET0 = da[11];

    else if(key == 'C')
        IOSET0 = da[12];

    else if(key == 'D')
        IOSET0 = da[13];

    else if(key == 'E')
        IOSET0 = da[14];

    else if(key == 'F')
        IOSET0 = da[15];
}



void wait(int count)
{
    while(count--);
}

void delay_ms(int count)
{
    int i, j;

    for(j = 0; j < count; j++)
    {
        for(i = 0; i < 3000; i++);
    }
}

void serial(void)
{
    U0LCR = 0x83;
    U0DLL = 0x62;
    U0DLM = 0x00;
    U0LCR = 0x03;
}

void UART0_SendChar(char ch)
{
  while(!(U0LSR & 0x20));
  U0THR = ch;
}

void UART0_SendString(char *str)
{
  while(*str)
  {
    UART0_SendChar(*str);
    str++;
  }
}


void ShowMenu(void)
{
  UART0_SendString("\r\n================================\r\n");
  UART0_SendString(" AUTOMATIC MEDICINE REMINDER\r\n");
  UART0_SendString("================================\r\n");
  UART0_SendString("1. View Alarms\r\n");
  UART0_SendString("2. Add Medicine Alarm\r\n");
  UART0_SendString("3. Delete Alarm\r\n");
  UART0_SendString("4. View Current Time\r\n");
  UART0_SendString("5. View Temperature\r\n");
  UART0_SendString("6. HOME\r\n");
  UART0_SendString("Select Option:\r\n");
}
void I2C_Init(void)
{
//VPBDIV = 0x02;
I2C0SCLH = 150;
I2C0SCLL = 150;
I2C0CONSET = (1 << I2EN);
}

int I2C_Start(void)
{
I2C0CONCLR = (1 << STO);
I2C0CONCLR = (1 << SI);
I2C0CONSET = (1 << STA);
return 0;
}

void senddata(char data)
{
while(!(I2C0CONSET & 0x08));
I2C0DAT = data;
I2C0CONCLR = (1 << SI);
delay_ms(2);
}

void LCD_Command(char cmnd)
{
char data;

data = (cmnd & 0xF0) | 0x0C;
senddata(data);

data = (cmnd & 0xF0) | 0x08;
senddata(data);

data = ((cmnd << 4) & 0xF0) | 0x0C;
senddata(data);

data = ((cmnd << 4) & 0xF0) | 0x08;
senddata(data);

delay_ms(2);
}

void LCD_Char(char ch)
{
    char data;

    data = (ch & 0xF0) | 0x0D;
    senddata(data);

    data = (ch & 0xF0) | 0x09;
    senddata(data);

    data = ((ch << 4) & 0xF0) | 0x0D;
    senddata(data);

    data = ((ch << 4) & 0xF0) | 0x09;
    senddata(data);

    delay_ms(2);
}

void LCD_String(char *str)
{
    while(*str)
    {
        LCD_Char(*str++);
    }
}

void LCD_Init(void)
{
    char welcome1[] = "Welcome to ";
    char welcome2[] = "Medicine Monitor";

    I2C_Start();

    wait(5000);  //can't use rtc based delay, bcz it is not yet initialized, we can initialise rtc before lcd, but we can't use lcd to display the current time

    while(!(I2C0CONSET & 0x08));

    I2C0CONCLR = (1 << STO);
    I2C0CONCLR = (1 << STA);

    I2C0CONSET = (1 << AA);
    I2C0DAT = SLAVE_ADDR;
    I2C0CONCLR = (1 << SI);

while(!(I2C0CONSET & 0x08));

if(I2C0STAT == 0x18)
{
    I2C0CONSET = (1 << AA);
    I2C0DAT = 0x00;
    I2C0CONCLR = (1 << SI);

    while(!(I2C0CONSET & 0x08));

    if(I2C0STAT == 0x28)
    {
        LCD_Command(0x02);
        LCD_Command(0x28);
        LCD_Command(0x0C);
        LCD_Command(0x06);
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String(welcome1);

        LCD_Command(0xC0);
        LCD_String(welcome2);
    }
}

delay_ms(2000);
}




unsigned int ADC_Read(void)
{
    unsigned int value;

    AD0CR |= (1 << 24);   // Start conversion

    while (1)
    {
        value = AD0GDR;

        if (value & 0x80000000)
            break;
    }

    return (value >> 6) & 0x3FF;
}


void ADC_Init(void)
{
    PINSEL1 |= (1 << 24);   // P0.28 = AD0.1
    AD0CR = 0x00200402;     // ADC enable, channel 1
}

float Read_Temperature(void)
{
    unsigned int adcValue;
    float voltage;
    float resistance;
    float lnR;
    float temperature;

    adcValue = ADC_Read();

    voltage = (adcValue * 3.3) / 1023.0;

    resistance = ((3.3 * 10000.0) / voltage) - 10000.0;

    lnR = log(resistance);

    temperature =
    (1.0 /
    (
        0.001129148 +
        (0.000234125 * lnR) +
        (0.0000000876741 * lnR * lnR * lnR)
    )) - 273.15;

    return temperature;
}



unsigned char Keypad_Scan(void)
{
    unsigned long int value, i;

    unsigned int row0[4] = {0x00EE0000, 0x00ED0000, 0x00EB0000, 0x00E70000};
    unsigned int row1[4] = {0x00DE0000, 0x00DD0000, 0x00DB0000, 0x00D70000};
    unsigned int row2[4] = {0x00BE0000, 0x00BD0000, 0x00BB0000, 0x00B70000};
    unsigned int row3[4] = {0x007E0000, 0x007D0000, 0x007B0000, 0x00770000};

    while(1)
    {
				Check_Alarm(); //check repeatedly
			
        IO1PIN = 0x00FF0000;
        IOCLR1 = 0x00100000;
        value = IO1PIN;
        delay_ms(2);
        value = value & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row0[i])
            {
                if(i == 0) return '1';
                if(i == 1) return '2';
                if(i == 2) return '3';
                if(i == 3) return '4';
            }
        }

        IO1PIN = 0x00FF0000;
        IOCLR1 = 0x00200000;
        value = IO1PIN;
        delay_ms(2);
        value = value & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row1[i])
            {
                if(i == 0) return '5';
                if(i == 1) return '6';
                if(i == 2) return '7';
                if(i == 3) return '8';
            }
        }

        IO1PIN = 0x00FF0000;
        IOCLR1 = 0x00400000;
        value = IO1PIN;
        delay_ms(2);
        value = value & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row2[i])
            {
                if(i == 0) return '9';
                if(i == 1) return 'A';
                if(i == 2) return 'B';
                if(i == 3) return 'C';
            }
        }

        IO1PIN = 0x00FF0000;
        IOCLR1 = 0x00800000;
        value = IO1PIN;
        delay_ms(2);
        value = value & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row3[i])
            {
                if(i == 0) return 'D';
                if(i == 1) return 'E';
                if(i == 2) return 'F';
                if(i == 3) return '0';
            }
        }
}
}




/*unsigned char exit_on1_key_pressed(void)
{
    unsigned long int value;

    IO1PIN = 0x00FF0000;

    IOCLR1 = 0x00100000;

    value = IOPIN1;

    value = value & 0x00FF0000;

    if(value == 0x00EE0000)
    {
        return 1;
    }

    return 0;
}*/




unsigned char check_any_key_pressed(void)
{
    unsigned long int value, i;

    unsigned int row0[4] =
    {
        0x00EE0000,
        0x00ED0000,
        0x00EB0000,
        0x00E70000
    };

    unsigned int row1[4] =
    {
        0x00DE0000,
        0x00DD0000,
        0x00DB0000,
        0x00D70000
    };

    unsigned int row2[4] =
    {
        0x00BE0000,
        0x00BD0000,
        0x00BB0000,
        0x00B70000
    };

    unsigned int row3[4] =
    {
        0x007E0000,
        0x007D0000,
        0x007B0000,
        0x00770000
    };

   

   
        /* ---------------- ROW 0 ---------------- */
        IOSET1 = 0x00FF0000;
        IOCLR1 = 0x00100000;

        value = IO1PIN & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row0[i])
            {
                if(i == 0) return '1';
                if(i == 1) return '2';
                if(i == 2) return '3';
                if(i == 3) return '4';
            }
        }

        /* ---------------- ROW 1 ---------------- */
        IOSET1 = 0x00FF0000;
        IOCLR1 = 0x00200000;

        value = IO1PIN & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row1[i])
            {
                if(i == 0) return '5';
                if(i == 1) return '6';
                if(i == 2) return '7';
                if(i == 3) return '8';
            }
        }

        /* ---------------- ROW 2 ---------------- */
        IOSET1 = 0x00FF0000;
        IOCLR1 = 0x00400000;

        value = IO1PIN & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row2[i])
            {
                if(i == 0) return '9';
                if(i == 1) return 'A';
                if(i == 2) return 'B';
                if(i == 3) return 'C';
            }
        }

        /* ---------------- ROW 3 ---------------- */
        IOSET1 = 0x00FF0000;
        IOCLR1 = 0x00800000;

        value = IO1PIN & 0x00FF0000;

        for(i = 0; i < 4; i++)
        {
            if(value == row3[i])
            {
                if(i == 0) return 'D';
                if(i == 1) return 'E';
                if(i == 2) return 'F';
                if(i == 3) return '0';
            }
				}


    return 0;
}










/*unsigned char check_any_key_pressed(void)
{
    unsigned long int value;

    IO1PIN = 0x00FF0000;
    IOCLR1 = 0x00100000;
    value = IO1PIN & 0x00FF0000;
    if(value != 0x00FF0000) return 1;

    IO1PIN = 0x00FF0000;
    IOCLR1 = 0x00200000;
    value = IO1PIN & 0x00FF0000;
    if(value != 0x00FF0000) return 1;

    IO1PIN = 0x00FF0000;
    IOCLR1 = 0x00400000;
    value = IO1PIN & 0x00FF0000;
    if(value != 0x00FF0000) return 1;

    IO1PIN = 0x00FF0000;
    IOCLR1 = 0x00800000;
    value = IO1PIN & 0x00FF0000;
    if(value != 0x00FF0000) return 1;

    return 0;
}*/





void View_Time_Mode(void)
{

    UART0_SendString("\r\nViewing Current Time\r\n");
    UART0_SendString("Press any key to go back to HOME\r\n");

    LCD_Command(0x01);

   
        sprintf(arr,"%02d:%02d:%02d",
        BCD_To_Decimal(HOUR),
        BCD_To_Decimal(MIN),
        BCD_To_Decimal(SEC));

        UART0_SendString("\rCurrent Time: ");
        UART0_SendString(arr);

        LCD_Command(0x80);
        LCD_String(arr);

        RTC_Delay_Seconds(1);

     while(1)
    {   
				Check_Alarm(); //check repeatedly
				
        if(check_any_key_pressed())
        {
            current_mode = MODE_MENU;
            LCD_Command(0x01);
            return;
        }
    }
}


void View_Temp_Mode(void)
{
    float temperature;

    UART0_SendString("\r\nViewing Temperature\r\n");
    UART0_SendString("Press any key to go back to HOME\r\n");

    LCD_Command(0x01);

   
        temperature = Read_Temperature();

        sprintf(arr,"Temp = %.2f C\r\n",temperature);

        UART0_SendString(arr);

        LCD_Command(0x80);
        LCD_String("Temperature");

        LCD_Command(0xC0);
        sprintf(arr,"%.2f C",temperature);
        LCD_String(arr);

        RTC_Delay_Seconds(1);

while(1)
    {
			Check_Alarm(); //check repeatedly

if(check_any_key_pressed())
        {
            current_mode = MODE_MENU;
            LCD_Command(0x01);
            return;
        }
    }
}




void Menu_Action(unsigned char key)
{

    if(key == '1')
    {
        UART0_SendString("\r\nView Alarms Selected\r\n");
        current_mode = MODE_VIEW_ALARM;
    }
    else if(key == '2')
    {
        UART0_SendString("\r\nAdd Medicine Alarm Selected\r\n");
        current_mode = MODE_ADD_ALARM;
    }
    else if(key == '3')
    {
        UART0_SendString("\r\nDelete Alarm Selected\r\n");
        current_mode = MODE_DELETE_ALARM;
    }
    else if(key == '4')
    {
        UART0_SendString("\r\nView Current Time Selected\r\n");
        current_mode = MODE_VIEW_TIME;
    }
    else if(key == '5')
    {
        UART0_SendString("\r\nView Temperature Selected\r\n");
        current_mode = MODE_VIEW_TEMP;
    }
    else if(key == '6')
    {
        UART0_SendString("\r\nHome Selected\r\n");
        current_mode = MODE_MENU;
    }

}


void Menu_Mode(void)
{
    unsigned char key;

ShowMenu();

    key = Keypad_Scan();

selected_key_7segment(key);

UART0_SendString("\r\nSelected Option: ");
UART0_SendChar(key);
UART0_SendString("\r\n");

    if(key >= '1' && key <= '6')
    {
        Menu_Action(key);
    }
    else
    {
        UART0_SendString("\r\nInvalid Option\r\n");
    }

}

void Welcome_Display(void)
{
    UART0_SendString("\r\n================================\r\n");
    UART0_SendString(" Welcome To Medicine Monitoring System\r\n");
    UART0_SendString("================================\r\n");
}



unsigned char Get_Digit(void)
{
    unsigned char key;

    while(1)
    {
			Check_Alarm(); //check repeatedly
			
				delay_ms(30);
        key = Keypad_Scan();

        if(key >= '0' && key <= '9')
        {
            UART0_SendChar(key);
selected_key_7segment(key);

while(check_any_key_pressed());
delay_ms(20);

            return key;
        }
        else
        {
            UART0_SendString("\r\nInvalid Input. Enter Digit 0-9\r\n");
selected_key_7segment(key);
 while(check_any_key_pressed());
        }
    }
}



void display_set_time(void)
{

    sprintf(arr,"%02d:%02d:%02d",
        BCD_To_Decimal(HOUR),
        BCD_To_Decimal(MIN),
        BCD_To_Decimal(SEC));

    UART0_SendString("\r\nRTC Started Successfully\r\n");
    UART0_SendString("Current Time: ");
    UART0_SendString(arr);
    UART0_SendString("\r\n");

    LCD_Command(0x01);

    LCD_Command(0x80);
    LCD_String(arr);

    LCD_Command(0xC0);
    LCD_String("RTC Started");

RTC_Delay_Seconds(3);
 
}


void Alarm_Off(void)
{
    //Buzzer_Off();
    IO0SET = 0x0000F000;   /* LEDs OFF for common anode */

    alarm_ringing = 0;
    active_alarm_index = 0xFF;

    UART0_SendString("\r\nAlarm Stopped\r\n");

    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Alarm Stopped");

    current_mode = MODE_MENU;
}


void Know_Current_Time(void)
{
    unsigned char key;
    unsigned int hour_tens, hour_units, min_tens, min_units;

while(1){
	
	Check_Alarm(); //check repeatedly
	
    UART0_SendString("\r\nEnter Current Time\r\n");
    UART0_SendString("Enter 4 digits in HHMM format\r\n");

    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Enter Time");

    UART0_SendString("\r\nHour Tens : ");
    key = Get_Digit();
    hour_tens = key - '0';

    UART0_SendString("\r\nHour Units: ");
    key = Get_Digit();
    hour_units = key - '0';

    UART0_SendString("\r\nMinute Tens: ");
    key = Get_Digit();
    min_tens = key - '0';

    UART0_SendString("\r\nMinute Units: ");
    key = Get_Digit();
    min_units = key - '0';

hour = (hour_tens * 10) + hour_units;
min = (min_tens * 10) + min_units;

if(hour < 24 && min < 60)
{
HOUR = Decimal_To_BCD(hour);
MIN  = Decimal_To_BCD(min);
SEC  = Decimal_To_BCD(0);
break;
}


UART0_SendString("\r\nInvalid Time entered\r\n");


}


CCR = 0x02;
PREINT  = 0x01C8;
PREFRAC = 0x61C0;
CCR = 0x01;

delay_ms(100);

display_set_time();

}



void Display_Alarm_List(void)
{
    unsigned char i;
    char buf[40];

    UART0_SendString("\r\nStored Alarms\r\n");
    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Stored Alarms");

    for(i = 0; i < alarm_count; i++)
    {
        if(alarms[i].active)
        {
            sprintf(buf, "\r\nAlarm %d -> %02d:%02d",
                    alarms[i].medicine_no,
                    alarms[i].hour,
                    alarms[i].minute);

            UART0_SendString(buf);
        }
    }

    UART0_SendString("\r\n");
}



void View_Alarm_Mode(void)
{
    Display_Alarm_List();

    UART0_SendString("\r\nPress any key to go back to HOME\r\n");

    while(1)
    {
        if(check_any_key_pressed())
        {
            while(check_any_key_pressed());

            delay_ms(20);

            current_mode = MODE_MENU;

            LCD_Command(0x01);

            return;
        }
    }
}

void Add_Alarm_Mode(void)
{
    unsigned char key;
    unsigned int hour_tens, hour_units, min_tens, min_units;
    unsigned int hour_value, min_value;
    char buf[40];

    if(alarm_count >= MAX_ALARMS)
    {
        UART0_SendString("\r\nAlarm memory full\r\n");
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("Alarm Full");
        RTC_Delay_Seconds(2);
        current_mode = MODE_MENU;
        return;
    }

    UART0_SendString("\r\nAdd New Alarm\r\n");
    UART0_SendString("Enter Time in HHMM format\r\n");

    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Enter Time");

    while(1)
    {
			
			Check_Alarm(); //check repeatedly
			
        UART0_SendString("\r\nHour Tens : ");
        key = Get_Digit();
        hour_tens = key - '0';

        UART0_SendString("\r\nHour Units: ");
        key = Get_Digit();
        hour_units = key - '0';

        UART0_SendString("\r\nMinute Tens: ");
        key = Get_Digit();
        min_tens = key - '0';

        UART0_SendString("\r\nMinute Units: ");
        key = Get_Digit();
        min_units = key - '0';

        hour_value = (hour_tens * 10) + hour_units;
        min_value  = (min_tens * 10) + min_units;

        if(hour_value < 24 && min_value < 60)
        {
            break;
        }

        UART0_SendString("\r\nInvalid Time. Enter Again\r\n");
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("Invalid Time");
        RTC_Delay_Seconds(2);

        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("Enter Time");
    }

    alarms[alarm_count].medicine_no = alarm_count + 1;
    alarms[alarm_count].hour = hour_value;
    alarms[alarm_count].minute = min_value;
    alarms[alarm_count].active = 1;

    alarm_count++;

    sprintf(buf, "\r\nAlarm Saved: %02d:%02d\r\n", hour_value, min_value);
    UART0_SendString(buf);

    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Alarm Saved");

    LCD_Command(0xC0);
    sprintf(buf, "%02d:%02d", hour_value, min_value);
    LCD_String(buf);

    RTC_Delay_Seconds(2);

    current_mode = MODE_MENU;
}




void Alarm_Response(void)
{
    unsigned char key;
    unsigned int start_sec;
    unsigned int current_sec;
    unsigned int elapsed;

    start_sec = BCD_To_Decimal(SEC);

    while(alarm_ringing)
    {
        key = Keypad_Scan();

        if(key == '1')
        {
            Alarm_Off();
            return;
        }

        else if(key == '2')
        {
            Alarm_Snooze_10s();
            return;
        }

        current_sec = BCD_To_Decimal(SEC);

        /* Handle RTC rollover */

        if(current_sec >= start_sec)
        {
            elapsed = current_sec - start_sec;
        }
        else
        {
            elapsed = (60 - start_sec) + current_sec;
        }

        /* Auto OFF after 10 seconds */

        if(elapsed >= 10)
        {
            UART0_SendString("\r\nAlarm Timeout\r\n");

            Alarm_Off();
            return;
        }
    }
}


void Alarm_Snooze_10s(void)
{
    unsigned char temp_index;

    temp_index = active_alarm_index;

    //Buzzer_Off();
    IO0SET = 0x0000F000;   /* LEDs OFF */

    UART0_SendString("\r\nAlarm Snoozed for 10 seconds\r\n");

    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Snoozed 10 sec");

    alarm_ringing = 0;
    active_alarm_index = 0xFF;

    RTC_Delay_Seconds(10);

    Alarm_On(temp_index);
}



void Delete_Alarm_Mode(void)
{
    unsigned char key;
    unsigned int delete_index;
    unsigned char i;

    if(alarm_count == 0)
    {
        UART0_SendString("\r\nNo alarms to delete\r\n");
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("No Alarms");
        RTC_Delay_Seconds(2);
        current_mode = MODE_MENU;
        return;
    }

    Display_Alarm_List();

    UART0_SendString("\r\nEnter Alarm No to Delete (1-9)\r\n");
    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Del Alarm No");

    key = Get_Digit();

    if(key < '1' || key > ('0' + alarm_count))
    {
        UART0_SendString("\r\nInvalid Alarm Number\r\n");
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("Invalid No");
        RTC_Delay_Seconds(2);
        current_mode = MODE_MENU;
        return;
    }

    delete_index = key - '1';

    for(i = delete_index; i < (alarm_count - 1); i++)
    {
        alarms[i] = alarms[i + 1];
        alarms[i].medicine_no = i + 1;
    }

    alarm_count--;

    UART0_SendString("\r\nAlarm Deleted Successfully\r\n");
    LCD_Command(0x01);
    LCD_Command(0x80);
    LCD_String("Alarm Deleted");

    RTC_Delay_Seconds(2);

    current_mode = MODE_MENU;
}

void Alarm_On(unsigned char index)
{
    char buf[40];

    alarm_ringing = 1;
    active_alarm_index = index;

    /* Common anode LEDs glow when pins are LOW */
    IO0CLR = 0x000F0000;   // P0.16 to P0.19 ON
    //Buzzer_On();

    sprintf(buf, "\r\nALARM %d  %02d:%02d\r\n",
            alarms[index].medicine_no,
            alarms[index].hour,
            alarms[index].minute);
    UART0_SendString(buf);

    LCD_Command(0x01);
    LCD_Command(0x80);
    sprintf(buf, "ALARM %d", alarms[index].medicine_no);
    LCD_String(buf);

    LCD_Command(0xC0);
    sprintf(buf, "%02d:%02d", alarms[index].hour, alarms[index].minute);
    LCD_String(buf);
		
		UART0_SendString("1. OFF\r\n");
    UART0_SendString("2. SNOOZE 10s\r\n");
}



void Check_Alarm(void)
{
    unsigned char i;
    unsigned int rtc_hour;
    unsigned int rtc_min;

    rtc_hour = BCD_To_Decimal(HOUR);
    rtc_min  = BCD_To_Decimal(MIN);

    /* If alarm is already ringing, keep LED and buzzer ON */
    if(alarm_ringing)
    {
        IO0CLR = 0x000F0000;   // LEDs ON
        //Buzzer_On();
        return;
    }

    /* Check all stored alarms */
    for(i = 0; i < alarm_count; i++)
    {
        if(alarms[i].active == 1 &&
           alarms[i].hour == rtc_hour &&
           alarms[i].minute == rtc_min)
        {
            Alarm_On(i);
						Alarm_Response();
            return;
        }
    }

    /* No alarm, keep LEDs OFF */
    IO0SET = 0x000F0000;   // LEDs OFF for common anode
}



float temperature = 0;
unsigned int tick = 0;

int main(void)
{
    /* UART0 on P0.0 and P0.1
       I2C0 on P0.2 and P0.3
    */
    PINSEL0 = 0x00000055;

    /* P0.16 to P0.31 as GPIO */
    PINSEL1 = 0x00000000;

    /* P1.16 to P1.23 as GPIO */
    PINSEL2 = 0x00000000;

    /* PORT0
       P0.16 to P0.23 -> 7-segment data
       P0.28 to P0.31 -> 7-segment control
       P0.0, P0.1 -> UART0
       P0.2, P0.3 -> I2C0 LCD
    */
    IO0DIR = 0xF0FFF000;


    /* PORT1
       P1.16 to P1.19 -> keypad columns (input)
       P1.20 to P1.23 -> keypad rows (output)
    */
    IO1DIR = 0x00F00000;
IOSET0 = 0xF0000000;
    /* Make keypad rows HIGH initially */
    IOSET1 = 0x00F00000;

    /* Initialize UART and LCD */
    /*serial();
    I2C_Init();
    LCD_Init();*/
		
		VPBDIV = 0x02;
		
		Welcome_Display();
		serial();
UART0_SendString("UART started\r\n");
I2C_Init();
UART0_SendString("I2C started\r\n");
LCD_Init();
UART0_SendString("LCD started\r\n");

    ADC_Init();   // thermistor



Know_Current_Time();

while(1)
{
			Check_Alarm();
			
    if(current_mode == MODE_MENU)
    {
        Menu_Mode();
    }

    else if(current_mode == MODE_VIEW_ALARM)
    {
        View_Alarm_Mode();
    }

    else if(current_mode == MODE_ADD_ALARM)
    {
        Add_Alarm_Mode();
    }

    else if(current_mode == MODE_DELETE_ALARM)
    {
        Delete_Alarm_Mode();
    }

    else if(current_mode == MODE_VIEW_TIME)
    {
        View_Time_Mode();
    }

    else if(current_mode == MODE_VIEW_TEMP)
    {
        View_Temp_Mode();
    }
}
}
