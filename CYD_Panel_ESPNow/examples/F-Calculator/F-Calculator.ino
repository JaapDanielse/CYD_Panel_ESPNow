
// CYD_Panel_ESPNow G-Calculator example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel


#include "CYD_Panel_ESPNow.h"

double  EntryValue = 0; // floating entry value
int64_t Entry = 0; // integer part of the entry
int64_t EntryDecimal = 0; // decimal part of the entry value
int16_t EntryDecimalPlaces = -1; // number of decimal places used -1 indicates no decimal point 0 just a decimal point
bool    EntryPositive = true; // indicates negative entry value
bool    EntryNew = true; // indicates Entry gets a new value 

double  OperandValue = 0; // operand value
int16_t OperandDecimalPlaces = 0; // number of decimal places to display

double  ResultValue = 0; // result value
int16_t ResultDecimalPlaces = -2; // -2 indicates strip trailing 0's 

enum Operation_t { _NOP, _ADD, _SUB, _MUL, _DIV}; // operation enumerator
const char * OppString[] = {" ", "+", "-", "X", "/"}; // corresponding string
Operation_t Operation = _NOP; // currently set opperation
Operation_t LastOperation = _NOP; // last operation to support repeated operations pressing = : 2 x 2  = = = = = etc.

const int DtostrBufSize = 30; // double to string buffer size
char DtostrfBuffer_1[DtostrBufSize]; // double to string buffer 1 
char DtostrfBuffer_2[DtostrBufSize]; // double to string buffer 2


//--------------------------------------------------------------------------------
void setup() 
{
  Panel.begin(CYD_LAYOUT_3); // init the panel using layout 3 (18 buttons)
}


//--------------------------------------------------------------------------------
void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)
}


//--------------------------------------------------------------------------------
void AddDigit(int newDigit)
{
  if(ResultValue != 0) AllClear(); // if a digit is enterd after = : clear all

  if(EntryDecimalPlaces == -1) // no decimal paces
  { // add digit to the number
    Entry = Entry * 10; // multiply Entry by 10
    Entry += newDigit; // add new digit
    
    if(EntryPositive)
      EntryValue = ((double) Entry); // positive value
    else
      EntryValue = -((double) Entry); // negative value

    Panel.sendf(CYD_Display_2, "%s", DString(EntryValue, EntryDecimalPlaces, DtostrfBuffer_1, EntryPositive)); //output
    MonitorOutput();
  }
  else 
  { // add decimal to the number
    EntryDecimal = EntryDecimal * 10; // Multiply entryDeimal by 10
    EntryDecimal += newDigit; // add new digit
    EntryDecimalPlaces++; // increase digital places
    
    if(EntryPositive)
      EntryValue = ((double) Entry + ((double) EntryDecimal / pow(10, EntryDecimalPlaces))); // positive value
    else
      EntryValue = -((double) Entry + ((double) EntryDecimal / pow(10, EntryDecimalPlaces))); // negative value
    
    Panel.sendf(CYD_Display_2, "%s", DString(EntryValue, EntryDecimalPlaces, DtostrfBuffer_1, EntryPositive)); // output
    MonitorOutput();
  }

  EntryNew = false; // something was typed
}


//--------------------------------------------------------------------------------
void Decimal()
{
  if(ResultValue != 0) AllClear(); // entering a decimal after result means clear

  if(EntryDecimalPlaces == -1)
  { // setup for decimap
    EntryDecimalPlaces = 0; // flags to print decimal point

    if(EntryPositive)
      EntryValue = ((double) Entry); // positive value
    else
      EntryValue = -((double) Entry); // negative value

    Panel.sendf(CYD_Display_2, "%s", DString(EntryValue, 0, DtostrfBuffer_1, EntryPositive)); // output
    MonitorOutput();
  } 

  EntryNew = false; // something was typed
}


//--------------------------------------------------------------------------------
void DelDigit()
{
  if(ResultValue != 0) // del after result means clear
  {
    AllClear(); 
  }

  if(EntryDecimalPlaces == -1)
  { // remove a digit
    Entry = Entry / 10; // divide entry by 10 (last digit gets lost)
    
    if(Entry == 0 && !EntryPositive)
    {
      EntryPositive = true;  // when deleting the last digit from a negative number: set positive
      EntryNew = true; // and new entry
    } 
    
    if(EntryPositive)
      EntryValue = ((double) Entry); // positive value
    else
      EntryValue = -((double) Entry); // negative value
  }  
  else if (EntryDecimalPlaces == 0)
  { // remove the dot
    EntryDecimalPlaces = -1; // set no decimal dot
    EntryDecimal = 0; // EntryDecimal to 0
  }
  else
  { // remove a decimal
    EntryDecimal = EntryDecimal / 10; // divide entryDecimal by 10 (last decimal gets lost)
    EntryDecimalPlaces--; // decrease the decimal places

    if(EntryPositive)
      EntryValue = ((double) Entry + ((double) EntryDecimal / pow(10, EntryDecimalPlaces))); // positive value
    else
      EntryValue = -((double) Entry + ((double) EntryDecimal / pow(10, EntryDecimalPlaces))); // negative value
  } 

  Panel.sendf(CYD_Display_2, "%s", DString(EntryValue, EntryDecimalPlaces, DtostrfBuffer_1, EntryPositive)); //output
  MonitorOutput();
}


//--------------------------------------------------------------------------------
void AllClear()
{
  OperandValue = 0;
  OperandDecimalPlaces = 0;
  Operation = _NOP;
  LastOperation = _NOP;
  EntryValue = 0;
  Entry = 0;
  EntryDecimal = 0;
  EntryDecimalPlaces = -1;
  EntryPositive = true;
  EntryNew = true;
  ResultValue = 0;

  Panel.sendf(CYD_Display_1, ""); // reset display too
  Panel.sendf(CYD_Display_2, "0");
  MonitorOutput();
}


//--------------------------------------------------------------------------------
void SetOpp(Operation_t Opp)
{
  if(isinf(ResultValue)) return; // don't operate on inf

  if(EntryNew == true && Opp == _SUB) // if _SUB is pressed on EntryNew it is a - sign for the value
  {
    EntryPositive = false; // set negative value
    EntryNew = false; // something has been enterd
    Panel.sendf(CYD_Display_2, "-0"); // show the - in the display
    return; // exit
  }

  if(Operation != _NOP)
  {
    ExecOpp(); // execute pending operation set OperandValue to result
    LastOperation = _NOP; // set Last operation is not valid
  } 

  if(ResultValue == 0) // a new number has come in bring EntryValue to OperandValue
  {
    OperandValue = EntryValue;
    OperandDecimalPlaces = EntryDecimalPlaces;
  }

  Operation = Opp; // new Operand

  ResultValue = 0; // rest result and input
  EntryValue = 0;
  Entry = 0;
  EntryDecimal = 0;
  EntryDecimalPlaces = -1;
  EntryPositive = true;
  EntryNew = true;

  Panel.sendf(CYD_Display_1, "%s %s", DString(OperandValue, OperandDecimalPlaces, DtostrfBuffer_1, true), OppString[(uint8_t)Operation]); // output result and pending opperation
  Panel.sendf(CYD_Display_2, "%s", DString(EntryValue, EntryDecimalPlaces, DtostrfBuffer_1, EntryPositive)); // output Entry (0)
  MonitorOutput();
}


//--------------------------------------------------------------------------------
void ExecOpp()
{
  if(isinf(ResultValue)) return; // don't operate on inf
  
  if(Operation == _NOP) Operation = LastOperation; // if = is pressed again repeat prev. opperation on the result.

  switch(Operation)
  {
    case _NOP: return; break;
    case _ADD: ResultValue = OperandValue + EntryValue; break; // add Opperand and Entry
    case _SUB: ResultValue = OperandValue - EntryValue; break; // subtract Opperand and Entry
    case _MUL: ResultValue = OperandValue * EntryValue; break; // Multiply Operand and Entry
    case _DIV: ResultValue = OperandValue / EntryValue; break; // Divide Opperand and Entry
  }

  Panel.sendf(CYD_Display_1, "%s %s %s =", DString(OperandValue, OperandDecimalPlaces, DtostrfBuffer_1, true), OppString[(uint8_t)Operation], DString(EntryValue, EntryDecimalPlaces, DtostrfBuffer_2, true));
  Panel.sendf(CYD_Display_2, "%s", DString(ResultValue, ResultDecimalPlaces, DtostrfBuffer_1, true)); // display the result on the entry place 
  MonitorOutput();
  
  OperandValue = ResultValue; // setup for next operation
  OperandDecimalPlaces = -2; // we don't know the number of decimal places

  LastOperation = Operation; // save this operation
  Operation = _NOP; // operation is done
  EntryNew = true; // Entry is new but EntryValue can still be used in repeat =
}


//--------------------------------------------------------------------------------
char * DString(double FloatValue, int16_t DecimalPlaces, char * Buffer, bool Positive)
{
  if(DecimalPlaces > 0)
  {
    dtostrf(FloatValue, DecimalPlaces + 2 , DecimalPlaces, Buffer); // with decimal places
  }
  else
  {
    dtostrf(FloatValue, 13, 10, Buffer);
    if(!Positive && FloatValue == 0) // if entry is negative overwrite preceding space with -
    {
      Buffer[0] = '-';
    }
    if(DecimalPlaces == -1)
      strchr(Buffer, '.')[0] = 0; // no decimal point
    else if(DecimalPlaces == 0)
      strchr(Buffer, '.')[1] = 0; // decimal point
    else if(DecimalPlaces == -2)  // we don't know how many decimal places so cleanup from the rear
    {
      for(int i = strlen(Buffer) - 1; i > 0; i--) // go backwards through the buffer
      {
        if(Buffer[i] == '0')
        {
          Buffer[i] = 0; // remove trailing 0's by place 0 value (not '0' character)
        } 
        else
        { // anything but a 0
          if(Buffer[i]=='.') 
            Buffer[i] = 0; // remove decimal point if not needed
          break; // leave the rest
        }

      }
    } 
  }

  return(Buffer);
}


//--------------------------------------------------------------------------------
void MonitorOutput() 
{ 
  Panel.sendf(CYD_MonitorField_1, "EntryValue %.5f", EntryValue);
  Panel.sendf(CYD_MonitorField_2, "Entry %lld  . EntryDecimal %lld", Entry, EntryDecimal);
  Panel.sendf(CYD_MonitorField_3, "EntrySign %d dec.pl. %d", EntryPositive, EntryDecimalPlaces);
  Panel.sendf(CYD_MonitorField_4, "Operand %f dec.pl. %d", OperandValue, OperandDecimalPlaces);
  Panel.sendf(CYD_MonitorField_5, "Operation %d (%s)" , Operation, OppString[(uint8_t)Operation]);
  Panel.sendf(CYD_MonitorField_6, "Result %.5f" , ResultValue);
}

//--------------------------------------------------------------------------------
void PanelCallback(CYD_channel event) 
{ 
  switch (event) // panel events come in when connected and when a button is pressed
  {
    case CYD_PanelConnected: InitPanel(); break; 

    case CYD_Button_1:  AddDigit(7); break; // add a digit to the entry
    case CYD_Button_2:  AddDigit(8); break;
    case CYD_Button_3:  AddDigit(9); break;
    case CYD_Button_7:  AddDigit(4); break;
    case CYD_Button_8:  AddDigit(5); break;
    case CYD_Button_9:  AddDigit(6); break;
    case CYD_Button_13: AddDigit(1); break;
    case CYD_Button_14: AddDigit(2); break;
    case CYD_Button_15: AddDigit(3); break;
    case CYD_Button_16: AddDigit(0); break;

    case CYD_Button_17: Decimal(); break; // add a decimal point to the entry

    case CYD_Button_4:  AllClear(); break; // clear all values
    case CYD_Button_10: DelDigit(); break; // delete one digit (from the back)

    case CYD_Button_5:  SetOpp(_DIV); break; // set the required operation (and if ther is a pending operation execute this)
    case CYD_Button_6:  SetOpp(_MUL); break;
    case CYD_Button_11: SetOpp(_ADD); break;
    case CYD_Button_12: SetOpp(_SUB); break; // can also be a sign for the value

    case CYD_Button_18: ExecOpp(); break; // Execute teh operation

    default: break; // Other events have no meaning.
  }
}


//--------------------------------------------------------------------------------
void InitPanel()
{
  Panel.send(CYD_ApplicationName,"CYD-Panel Calculator"); // Set the application name
  Panel.send(CYD_MonitorField_1, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_MonitorField_2, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_MonitorField_3, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_MonitorField_4, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_MonitorField_5, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_MonitorField_6, CYD_FONT, CYD_SMALL);

  Panel.send(CYD_Button_1, CYD_FONT, CYD_BIG); // fill in the buttons (Font, BackColor, Text)
  Panel.send(CYD_Button_1, "7");
  Panel.send(CYD_Button_2, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_2, "8");
  Panel.send(CYD_Button_3, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_3, "9");

  Panel.send(CYD_Button_7, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_7, "4");
  Panel.send(CYD_Button_8, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_8, "5");
  Panel.send(CYD_Button_9, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_9, "6");

  Panel.send(CYD_Button_13, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_13, "1");
  Panel.send(CYD_Button_14, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_14, "2");
  Panel.send(CYD_Button_15, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_15, "3");

  Panel.send(CYD_Button_16, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_16, "0");

  Panel.send(CYD_Button_17, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_17, ".");

  Panel.send(CYD_Button_4, CYD_BACKCOLOR, CYD_PINK);
  Panel.send(CYD_Button_4, "AC");

  Panel.send(CYD_Button_10, CYD_BACKCOLOR, CYD_PINK);
  Panel.send(CYD_Button_10, "DEL");

  Panel.send(CYD_Button_5, CYD_BACKCOLOR, CYD_LBLUE);
  Panel.send(CYD_Button_5, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_5, "/");
  Panel.send(CYD_Button_6, CYD_BACKCOLOR, CYD_LBLUE);
  Panel.send(CYD_Button_6, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_6, "x");
  Panel.send(CYD_Button_11, CYD_BACKCOLOR, CYD_LBLUE);
  Panel.send(CYD_Button_11, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_11, "+");
  Panel.send(CYD_Button_12, CYD_BACKCOLOR, CYD_LBLUE);
  Panel.send(CYD_Button_12, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_12, "-");

  Panel.send(CYD_Button_18, CYD_BACKCOLOR, CYD_ORANGE);
  Panel.send(CYD_Button_18, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_18, "=");

  Panel.send(CYD_Display_2, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Display_2, "0");

  Panel.send(CYD_InfoPanel, CYD_TITLE, "CYD_Panel Calculator");
  Panel.send(CYD_InfoPanel, "CYD_Panel V1.0.0 Example / Showcase");
  Panel.send(CYD_InfoPanel, "(c)2026 - J.C. Danielse");
  Panel.send(CYD_InfoPanel, "Published under GPL-3.0 licence");
  Panel.send(CYD_InfoPanel, "");
  Panel.send(CYD_InfoPanel, "Generic calculator built on the CYD_Panel lib.");
  Panel.send(CYD_InfoPanel, "Uses ESP32 double precision (64 bit) float.");
  Panel.send(CYD_InfoPanel, "For value entry int64_t values are used.");
  Panel.send(CYD_InfoPanel, "");
  Panel.send(CYD_InfoPanel, "Allows to use the minus '-' as both");
  Panel.send(CYD_InfoPanel, "operation and as sign.");
}

