//Modified by Martin McDonough to work with TurboSphere

EvaluateSystemScript("colors.js");

function ClearScreen()
{
  ApplyColorMask(Black);
}

// old font stuff
var system_font = GetSystemFont();


function SetFont(filename)
{
  system_font = new Font(filename);
}

function DrawText(x, y, text)
{
  system_font.drawText(x, y, text);
}

function SetTextColor(color)
{
  system_font.setColorMask(color);
}

function StringWidth(str)
{
  system_font.getStringWidth(str);
}

function GetFontHeight()
{
  system_font.getHeight();
}

function DrawTextBox(x, y, w, h, offset, text)
{
  system_font.drawTextBox(x, y, w, h, offset, text);
}

// old window stuff

var system_windowstyle = GetSystemWindowStyle();

function SetWindowStyle(filename)
{
  system_windowstyle = new WindowStyle(filename);
}

function DrawWindow(x,y, w, h)
{
  system_windowstyle.drawWindow(x, y, w, h);
}
