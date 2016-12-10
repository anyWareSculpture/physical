import processing.serial.*;
import controlP5.*;

ControlP5 cp5;

Serial myPort;        // The serial port
boolean started = false;
int numsensors = 1;

int xPos = 1;         // horizontal position of the graph
 
int canvassize[] = {0,0};
int canvasoffset[] = {0,0};
int graphindex = 0;
float baselines[][];
float strengths[][];
boolean touches[][];
 

// ECR 0x5E
// Auto0 0x7B
// Auto1 0x7C
// CDC
// CDT
// Debounce

// Global CDC
// Global CDT
// Global Touch thres
// Global Release thres

void setup () {
    // set the window size:
    size(1000, 500);
    canvassize[0] = 900;
    canvassize[1] = 300;
 
    baselines = new float[12][canvassize[0]];
    strengths = new float[12][canvassize[0]];
    touches = new boolean[12][canvassize[0]];

 
    // List all the available serial ports
    String[] list = Serial.list();
    println(Serial.list());
    // I know that the first port in the serial list on my mac
    // is always my  Arduino, so I open Serial.list()[0].
    // Open whatever port is the one you're using.
//    myPort = new Serial(this, list[list.length - 1], 115200);
    myPort = new Serial(this, "/dev/cu.usbmodem14121", 115200);
    // don't generate a serialEvent() unless you get a newline character:
    myPort.bufferUntil('\n');
    // set inital background:
    background(0);

    PFont font = createFont("monaco",12);
    cp5 = new ControlP5(this);

    Textfield ecr = cp5.addTextfield("ECR")
     .setPosition(20,canvassize[1] + 120)
     .setSize(20,20)
     .setFont(font)
     .setAutoClear(false);

    Textfield cdc = cp5.addTextfield("CDC")
     .setPosition(60,canvassize[1] + 120)
     .setSize(20,20)
     .setFont(font)
     .setAutoClear(false);

    Textfield cdt = cp5.addTextfield("CDT")
     .setPosition(100,canvassize[1] + 120)
     .setSize(20,20)
     .setFont(font)
     .setAutoClear(false);

    Textfield auto0 = cp5.addTextfield("Auto0")
     .setPosition(140,canvassize[1] + 120)
     .setSize(20,20)
     .setFont(font)
     .setAutoClear(false);

    Textfield auto1 = cp5.addTextfield("Auto1")
     .setPosition(180,canvassize[1] + 120)
     .setSize(20,20)
     .setFont(font)
     .setAutoClear(false);

   cp5.addButton("Restart")
     .setPosition(20,canvassize[1] + 170);
   cp5.addButton("Reset")
     .setPosition(110,canvassize[1] + 170);
}

public void sendString(String str) {
  println("Sending: " + str);
  myPort.write(str + "\n");
}

public void controlEvent(ControlEvent theEvent) {

  if (theEvent.getController().getName().equals("ECR")) {
      sendString("5E " + ((Textfield)theEvent.getController()).getText());
  }
  else if (theEvent.getController().getName().equals("CDC")) {
      sendString("5C " + ((Textfield)theEvent.getController()).getText());
  }
  else if (theEvent.getController().getName().equals("CDT")) {
      sendString("5D " + ((Textfield)theEvent.getController()).getText());
  }
  else if (theEvent.getController().getName().equals("Auto0")) {
      sendString("7B " + ((Textfield)theEvent.getController()).getText());
  }
  else if (theEvent.getController().getName().equals("Auto1")) {
      sendString("7C " + ((Textfield)theEvent.getController()).getText());
  }
  else if (theEvent.getController().getName().equals("Restart")) {
    sendString("5E 00");
    sendString("5E " + ((Textfield)cp5.getController("ECR")).getText());
    sendString("reg");
  }
  else if (theEvent.getController().getName().equals("Reset")) {
    sendString("reset");
    sendString("reg");
  }
  else {
    println(theEvent.getController().getName());
    println(unhex(theEvent.getController().getStringValue()));
  }
}

void draw () {
    background(0);

       stroke(127,34,255);
//      int maxHeight = canvassize[1]/20;
      int maxHeight = canvassize[1]/6;
      int ypos = int(canvassize[1]*0.5+100);
      drawFloatValues(strengths[0], ypos, maxHeight);
      stroke(255,0,0, 128);
      drawBoolValues(touches[0], ypos);
      fill(127,34,255);
      drawStringValue(strengths[0][(graphindex+canvassize[0]-1)%canvassize[0]], ypos);
//      ypos += maxHeight*4;
//      stroke(0,127,0);
      drawFloatValues(baselines[0], ypos, maxHeight);
//      fill(0,127,0);
//      drawStringValue(baselines[i][(graphindex+canvassize[0]-1)%canvassize[0]], ypos);
}

void drawFloatValues(float[] values, int ypos, int size) {
    for (int i=0; i<canvassize[0]; ++i) {
      line(i, ypos, i, ypos - values[(i+graphindex)%canvassize[0]]*size);
    }
}

void drawBoolValues(boolean[] values, int ypos) {
  for (int i=0; i<canvassize[0]; ++i) {
    if (values[(i+graphindex)%canvassize[0]]) {
      point(i, ypos);
    }
  }
}

void drawStringValue(float value, int ypos) {
    textSize(24);
    text(value, canvassize[0], ypos);
}

void serialEvent (Serial myPort) {
      // get the ASCII string:
    String inString = myPort.readStringUntil('\n');
    
    if (inString != null) {
        // trim off any whitespace:
        inString = trim(inString);

        if (!started && inString.contains("Hello Touch")) started = true;
        if (!started) return;
        String[] strvalues  = split(inString, ' ');
        if (strvalues.length > 0) {
          if (strvalues[0].equals("data")) handleData(strvalues);
          else if (strvalues[0].equals("reg")) handleRegisters(strvalues);
          else println(inString);
        }
    }
}

void handleRegisters(String[] strvalues) {
  if (strvalues.length != 0x81) {
    println("handleRegisters: Wrong # of values: " + strvalues.length + " (expected 0x81)");
    return;
  }
  int[] registers = new int[0x80];
  for (int i=0;i<0x80;i++) {
    registers[i] = int(strvalues[i+1]);
  }
  
  cp5.get(Textfield.class, "ECR").setText(strvalues[0x5E+1]);
  cp5.get(Textfield.class, "CDC").setText(strvalues[0x5C+1]);
  cp5.get(Textfield.class, "CDT").setText(strvalues[0x5D+1]);
  cp5.get(Textfield.class, "Auto0").setText(strvalues[0x7B+1]);
  cp5.get(Textfield.class, "Auto1").setText(strvalues[0x7C+1]);
  
  
  println(strvalues);
}

void handleData(String[] strvalues) {
        if (strvalues.length != 4) {
            println("handleData: Wrong # of values: " + strvalues.length + " (expected 25)");
            return;
        }
        int ypos = 0;
        int[] values = new int[24];
        for (int i=0;i<3;i++) {
          values[i] = int(strvalues[i+1]);
        }
        for (int i=0;i<values.length;i+=3) {
          strengths[i/3][graphindex] = 1.0*values[i]/255;
          baselines[i/3][graphindex] = 1.0*values[i+1]/255;
          touches[i/3][graphindex] = values[i+2] == 1;
        }
        graphindex = (graphindex + 1)%canvassize[0];

}