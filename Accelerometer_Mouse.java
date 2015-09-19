import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.awt.AWTException; 
import java.awt.Robot; 
import java.awt.Rectangle; 
import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class Acc_Mouse extends PApplet {






Robot robot;
Serial Port;  

int i=0;
int X = 0, Y = 0;
int X_C = 0, Y_C = 0;

int[] Data;

public void setup() 
{
  Data = new int[4];
  size(100,100);
  background(255);

  String Port_Name = Serial.list()[0];
  Port = new Serial(this, Port_Name, 9600);
  
  try 
  {
    robot = new Robot();
  }
  catch (AWTException e)
  {
    e.printStackTrace();
  }
  
}

public void draw()
{
  for(i=0;i<4;i++)
  {  
    if(Port.available()>0) 
    Data[i] = Port.read(); 
  }
  
  X = Data[0];
  Y = Data[1];
  
  if(Data[2] == 0)
  {
    X_C = X_C + X;
  }
  
  if(Data[2] == 1)
  {
    X_C = X_C - X;
  }
  
  if(Data[3] == 0)
  {
    Y_C = Y_C + Y;
  }
  
  if(Data[3] == 1)
  {
    Y_C = Y_C - Y;
  }

  robot.mouseMove(X_C,Y_C);

}
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "--full-screen", "--bgcolor=#666666", "--stop-color=#cccccc", "Acc_Mouse" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
