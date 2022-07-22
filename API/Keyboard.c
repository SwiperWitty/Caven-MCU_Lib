/*
 * Keyboard.c
 *
 *  Created on: 2022年7月22日
 *      Author: gxwl
 */


char ASCII_TO_Keyboard(char *Source,char *Target)
{
    int Length = strlen(Source);
    if (Length > 50)
        return 0;
    for (int i = 0;i < Length; i++)
    {
        if (Source[i] >= 'A' && Source[i] <= 'Z')       //全部变成小写
        {
            Source[i] += 32;
        }
        else if (Source[i] >= 'a' && Source[i] <= 'z')
        {
            Target[i] = Source[i] - 93;         //97 - 04(a - z)
        }
        else if (Source[i] >= '1' && Source[i] <= '9')
        {
            Target[i] = Source[i] - 19;         //49 - 30(1 - 9)
        }
        else if (Source[i] == '0')
        {
            Target[i] = Source[i] - 9;          //48 - 39(0)
        }
        else if (Source[i] == '\n')     //Enter
        {
            Target[i] = Source[i] -(-27);       //13 - 40( )
        }
        else if (Source[i] == 27)       //ESC
        {
            Target[i] = Source[i] -(-14);       //27 - 41()
        }
        else if (Source[i] == 8)        //BACKSPACE
        {
            Target[i] = Source[i] -(-34);       //08 - 42
        }
        else if (Source[i] == 11)     //TAB
        {
            Target[i] = Source[i] -(-32);       // 11- 43
        }
        else if (Source[i] == '-')     //UNDERSCORE
        {
            Target[i] = Source[i] -(0);        // 45- 45
        }
        else if (Source[i] == '+')     //PLUS
        {
            Target[i] = Source[i] -(-3);        // 43- 46
        }
        else if (Source[i] == '{')     //OPEN_BRACKET
        {
            Target[i] = Source[i] - 76;        // 123- 47
        }
        else if (Source[i] == '}')     //CLOSE_BRACKET
        {
            Target[i] = Source[i] - 77;        // 125- 48
        }
        else if (Source[i] == 127)     //DEL
        {
            Target[i] = Source[i] - 51;        // 127- 76
        }
        else {
            return 0;
        }
        return 1;
    }
}

void Keyboard_Send (char *Target,char *Target)
{

}
